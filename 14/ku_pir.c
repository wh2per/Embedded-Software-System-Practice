#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <asm/delay.h>
#include <linux/rculist.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "ku_pir.h"

MODULE_LICENSE("GPL");

struct ku_pir_msg{
	struct list_head msg_list;	// 메세지 리스트 헤드 
	struct ku_pir_data* data;		// 데이터 
};

struct ku_pir_msq{
	struct list_head que_list;	// 큐 리스트 포인터
	int fd;				// 큐의 FD
	struct ku_pir_msg msg_head; 	// 메세지 리스트 포인터 
	int size;			// 메세지 갯수
};
// 전역변수 
static int q_fd = 0;
struct ku_pir_msq kernel_msq;
static int irq_num;
spinlock_t ku_pir_lock;
wait_queue_head_t my_wq;
// 함수선언
static struct ku_pir_msq* ku_pir_msq_create(void);
static int ku_pir_msq_delete(struct ku_pir_msq* msq);
static struct ku_pir_msq* ku_pir_msq_get(int fd);
static int ku_pir_msq_flush(struct ku_pir_msq* msq);

static struct ku_pir_msg* ku_pir_msg_create(int fd, struct ku_pir_data* new);
static int ku_pir_msg_delete(struct ku_pir_msg* msg);

static long ku_pir_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int ku_pir_read(struct file* file, char* buf, size_t len, loff_t* lof);
static int ku_pir_write(struct file* file, const char* buf, size_t len, loff_t* lof);
static int ku_pir_open(struct inode *inode, struct file *file);
static int ku_pir_release(struct inode *inode, struct file *file);

static struct ku_pir_msq* ku_pir_msq_create(void){
	struct ku_pir_msq* msq = NULL;
	
	msq = (struct ku_pir_msq*)kmalloc(sizeof(struct ku_pir_msq),GFP_KERNEL);
	msq->fd = q_fd;
	INIT_LIST_HEAD(&msq->msg_head.msg_list);
	msq->size = 0;

	q_fd++;

	return msq;
}

static int ku_pir_msq_delete(struct ku_pir_msq* msq){
	struct list_head* next = 0;
	struct list_head* pos = 0;
	struct ku_pir_msg* msg_head = &msq->msg_head;
	
	list_for_each_safe(pos, next, &msg_head->msg_list){
		ku_pir_msg_delete(list_entry(pos, struct ku_pir_msg, msg_list));
	}

	kfree(msq);

	return 0;
}

static struct ku_pir_msq* ku_pir_msq_get(int fd){
	struct ku_pir_msq* temp = NULL;
	struct ku_pir_msq* find = NULL;
	
	rcu_read_lock();	//락걸기

	list_for_each_entry(temp, &kernel_msq.que_list, que_list){
		if(temp->fd == fd){
			find = temp;
			break;		
		}
	}
	rcu_read_unlock();	//락풀기 
	
	return find;
}

static int ku_pir_msq_flush(struct ku_pir_msq* msq){
	struct list_head* next = NULL;
	struct list_head* pos = NULL;
	struct ku_pir_msg* msg_head = &msq->msg_head;
	
	list_for_each_safe(pos, next, &msg_head->msg_list){
		list_del(pos);
		ku_pir_msg_delete(list_entry(pos, struct ku_pir_msg, msg_list));
	}
	msq->size = 0;
	return 0;
}

static struct ku_pir_msg* ku_pir_msg_create(int fd, struct ku_pir_data* new){
	struct ku_pir_msg* msg = NULL;

	msg = (struct ku_pir_msg*)kmalloc(sizeof(struct ku_pir_msg), GFP_KERNEL);

	msg->data = (struct ku_pir_data*)kmalloc(sizeof(struct ku_pir_data), GFP_KERNEL);
	msg->data->ts = new->ts;
	msg->data->rf_flag = new->rf_flag;

	return msg;	
}

static int ku_pir_msg_delete(struct ku_pir_msg* msg){
	kfree(msg->data);
	kfree(msg);
		
	return 0;
}

static long ku_pir_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	int result = -1;
	int fd;
	struct ku_pir_msq *msq;
	
	switch(cmd) {
	case PIR_IOCTL_CREATE:
		msq = ku_pir_msq_create();
		list_add_tail(&msq->que_list, &kernel_msq.que_list);
		result = msq->fd;
		break;
	case PIR_IOCTL_CLOSE:
		fd = *(int*)arg;
		msq = ku_pir_msq_get(fd);
		list_del(&msq->que_list);
		result = ku_pir_msq_delete(msq);
		break;
	case PIR_IOCTL_FLUSH:
		fd = *(int*)arg;
		msq = ku_pir_msq_get(fd);
		result = ku_pir_msq_flush(msq);
		break;
	default:
		break;
	}

	return result;
}

static int ku_pir_read(struct file* file, char* buf, size_t len, loff_t* lof){
	struct ku_pir_msq* msq = NULL;
	struct ku_pir_msg* kernel_buf = NULL;
	struct ku_pir_gift* user_buf = NULL;

	struct list_head* next = NULL;
	struct list_head* pos = NULL;
	struct ku_pir_msg* msg_head = NULL;
	
	spin_lock(&ku_pir_lock);	// 락걸기 
	user_buf = (struct ku_pir_gift*)buf;
	msq = ku_pir_msq_get(user_buf->fd);	// find queue
	wait_event(my_wq, msq->size>0);
	msg_head = &msq->msg_head;
	list_for_each_safe(pos, next, &msg_head->msg_list){
		kernel_buf = list_entry(pos,struct ku_pir_msg, msg_list);
		break;
	}
	
	// copy to user
	copy_to_user(user_buf->data, kernel_buf->data, sizeof(struct ku_pir_data));
	// clean
	
	list_del(pos);
	ku_pir_msg_delete(kernel_buf);
	msq->size--;
	spin_unlock(&ku_pir_lock);	//락풀기 
	
	return 5;
	
}

static int ku_pir_write(struct file* file, const char* buf, size_t len, loff_t* lof){
	struct ku_pir_msq* msq = NULL;
	struct ku_pir_gift* user_buf = NULL;
	struct ku_pir_msg* kernel_buf = 0;
	int result = 5;	

	user_buf = (struct ku_pir_gift*)buf;

	spin_lock(&ku_pir_lock);	//락걸기 
	
	msq = ku_pir_msq_get(user_buf->fd);		//find queue
	
	kernel_buf = (struct ku_pir_msg*)kmalloc(sizeof(struct ku_pir_msg), GFP_KERNEL);
	kernel_buf->data = (struct ku_pir_data*)kmalloc(sizeof(struct ku_pir_data), GFP_KERNEL);
	// copy from user
	copy_from_user(kernel_buf->data, user_buf->data, sizeof(struct ku_pir_data));	
	
	list_add_tail(&kernel_buf->msg_list, &msq->msg_head.msg_list);
	msq->size++;	

	spin_unlock(&ku_pir_lock);	//락풀기 
	
	return result;
}

static int ku_pir_open(struct inode *inode, struct file *file){
	printk("open ku_pir\n");
	
	return 0;
}

static int ku_pir_release(struct inode *inode, struct file *file){
	printk("release ku_pir\n");
	
	return 0;
}

static irqreturn_t simple_sensor_isr(int irq, void* dev_id){
	struct ku_pir_msq* temp = NULL;
	struct ku_pir_msg* msg = NULL;
	
	if(gpio_get_value(KUPIR_SENSOR)==1){
		printk("rising \n");

		rcu_read_lock();	//락걸기
		list_for_each_entry(temp, &kernel_msq.que_list, que_list){
			msg = (struct ku_pir_msg*)kmalloc(sizeof(struct ku_pir_msg), GFP_KERNEL);

			msg->data = (struct ku_pir_data*)kmalloc(sizeof(struct ku_pir_data), GFP_KERNEL);
			msg->data->ts = jiffies;
			msg->data->rf_flag = '0';

			list_add_tail(&msg->msg_list, &temp->msg_head.msg_list);
			temp->size++;
		}
		wake_up(&my_wq);
		rcu_read_unlock();	//락풀기 		
	}else if(gpio_get_value(KUPIR_SENSOR)==0){
		printk("falling \n");

		rcu_read_lock();	//락걸기
		list_for_each_entry(temp, &kernel_msq.que_list, que_list){
			msg = (struct ku_pir_msg*)kmalloc(sizeof(struct ku_pir_msg), GFP_KERNEL);

			msg->data = (struct ku_pir_data*)kmalloc(sizeof(struct ku_pir_data), GFP_KERNEL);
			msg->data->ts = jiffies;
			msg->data->rf_flag = '1';

			list_add_tail(&msg->msg_list, &temp->msg_head.msg_list);
			temp->size++;
		}
		wake_up(&my_wq);
		rcu_read_unlock();	//락풀기
	}else{
					
	}

	return IRQ_HANDLED;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

struct file_operations ku_pir_fops = {
	.unlocked_ioctl = ku_pir_ioctl,
	.read = ku_pir_read,
	.write = ku_pir_write,
	.open = ku_pir_open,
	.release = ku_pir_release,
};

static int __init ku_pir_init(void){
	int ret;
	
	printk("INIT ku_pir\n");
	
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ku_pir_fops);
	ret = cdev_add(cd_cdev, dev_num, 1);
	if(ret < 0){
		printk("fail to add character device\n");
		return -1;
	}
	
	spin_lock_init(&ku_pir_lock);
	init_waitqueue_head(&my_wq);

	INIT_LIST_HEAD(&kernel_msq.que_list);

	//sensor init
	gpio_request_one(KUPIR_SENSOR, GPIOF_IN, "sensor1");
	irq_num = gpio_to_irq(KUPIR_SENSOR);
	ret = request_irq(irq_num, simple_sensor_isr, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "sensor_irq", NULL);
	if(ret){
		printk(KERN_ERR "Unableto request IRQ: %d\n", ret);
		free_irq(irq_num, NULL);
	}
	enable_irq(irq_num);

	return 0;
}

static void __exit ku_pir_exit(void){
	printk("EXIT ku_pir\n");
	
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
	//senser exit
	disable_irq(irq_num);
	free_irq(irq_num, NULL);
	gpio_free(KUPIR_SENSOR);

}

module_init(ku_pir_init);
module_exit(ku_pir_exit);


