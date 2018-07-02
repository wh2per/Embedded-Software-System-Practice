#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>
#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");

#define PIR 17
#define LED1 4

struct my_data{
	int year;
	int month;
	int day;
};

struct task_struct *test_task = NULL;
static struct workqueue_struct *test_wq;

typedef struct{
	struct work_struct my_work;
	struct my_data data;
}my_work_t;

my_work_t *work;

static int irq_num;

static void my_wq_func(struct work_struct *work){
	my_work_t *my_work = (my_work_t*)work;
	(my_work->data).year = 2018;
	(my_work->data).month = 5;
	(my_work->data).day = 16;
	printk("today is %d/%d/%d\n", (my_work->data).year, (my_work->data).month, (my_work->data).day);
	kfree((void*)work);
}

static irqreturn_t simple_pir_isr(int irq, void* data){
	int ret;
	int a;
	printk("ISR start \n");
	
	if(test_wq){
		work = (my_work_t*)kmalloc(sizeof(my_work_t), GFP_KERNEL);
		if(work){
			INIT_WORK((struct work_struct*)work, my_wq_func);
			ret = queue_work(test_wq, (struct work_struct*)work);
		}
	}
	
	gpio_set_value(LED1, 1);

	return IRQ_HANDLED;
}

int thread_func(void *data){
	while(!kthread_should_stop()){
		printk("jiffies in thread :%ld \n", jiffies);
		
		msleep(1000);	
	}
	return 0;
}

static int __init kernthread_init(void){
	int ret;
	
	printk("Init Module\n");

	test_wq = create_workqueue("tets_workqueue");

	gpio_request_one(PIR, GPIOF_IN, "PIR");
	irq_num = gpio_to_irq(PIR);
	ret = request_irq(irq_num, simple_pir_isr, IRQF_TRIGGER_FALLING, "pir_irq", NULL);
	if(ret){
		printk(KERN_ERR "Unable to request IRQ:%d\n", ret);
		free_irq(irq_num, NULL);	
	}
	
	test_task = kthread_create(thread_func, NULL,"my_thread");
	if(IS_ERR(test_task)){
		test_task = NULL;
		printk("test kernel thread ERROR \n");
	}

	wake_up_process(test_task);

	gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1");
	gpio_set_value(LED1, 0);

	return 0;
}	

static void __exit kernthread_exit(void){
	printk("EXIT MODULE \n");

	free_irq(irq_num, NULL);
	gpio_free(PIR);
	gpio_free(LED1);
	if(test_task1){
		kthread_stop(test_task);
		printk("test kernel thread STOP");
	}

	flush_workqueue(test_wq);
	destroy_workqueue(test_wq);
}

module_init(kernthread_init);
module_exit(kernthread_exit);
