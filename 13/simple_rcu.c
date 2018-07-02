#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/rculist.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "simple_rcu_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ	_IOWR(SIMPLE_IOCTL_NUM,IOCTL_NUM1, unsigned long*)
#define IOCTL_READ_RCU	_IOWR(SIMPLE_IOCTL_NUM,IOCTL_NUM2, unsigned long*)

struct test_list{
	struct list_head list;
	int id;
};

static dev_t dev_num;
static struct cdev *cd_cdev;
spinlock_t my_lock;
struct test_list mylist;

static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	struct test_list *pos;
	unsigned int i;	

	switch(cmd){
	case IOCTL_READ:
		spin_lock(&my_lock);
		printk("spinlock read");
		i=0;
		list_for_each_entry(pos, &mylist.list, list){
			printk("spinlock : %d",pos->id);
			i++;
		}
		spin_unlock(&my_lock);
		break;
	case IOCTL_READ_RCU:
		rcu_read_lock();
		printk("rcu read");
		i=0;
		list_for_each_entry_rcu(pos, &mylist.list, list){
			printk("rcu : %d",pos->id);
			i++;
		}
		rcu_read_unlock();
		break;
	default:
		return -1;	
	}

	return 0;
}

static int simple_block_open(struct inode *inode, struct file *file){
	return 0;
}

static int simple_block_release(struct inode *inode, struct file *file){
	return 0;
}

struct file_operations simple_block_fops ={
	.unlocked_ioctl = simple_ioctl,
	.open = simple_block_open,
	.release = simple_block_release
};

static int __init simple_rcu_init(void) {
	int i;
	struct test_list *pos;

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_block_fops);
	cdev_add(cd_cdev, dev_num, 1);

	spin_lock_init(&my_lock);
	
	INIT_LIST_HEAD(&mylist.list);

	for(i=0; i<5; i++){
		pos = (struct test_list*)kmalloc(sizeof(struct test_list), GFP_KERNEL);
		pos->id = i;
		list_add_rcu(&(pos->list), &(mylist.list));
	}
	
	printk("init module\n");

	return 0;
}

static void __exit simple_rcu_exit(void){
	struct test_list *pos;
	struct test_list *tmp;
	
	list_for_each_entry_safe(tmp, pos, &mylist.list, list){
		list_del_rcu(&(tmp->list));
		kfree(tmp);
	}

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);

	printk("exit module\n");
}

module_init(simple_rcu_init);
module_exit(simple_rcu_exit);
