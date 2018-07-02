#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
MODULE_LICENSE("GPL");

int number=0;
EXPORT_SYMBOL(number);
#define DEV_NAME "module1_dev"

#define MODULE_START_NUM 0x80
#define MODULE_NUM1 MODULE_START_NUM+1
#define MODULE_NUM2 MODULE_START_NUM+2
#define MODULE_NUM3 MODULE_START_NUM+3

#define MODULE1_NUM 'z'
#define MODULE1 _IOWR(MODULE1_NUM, MODULE_NUM1, unsigned long*)
#define MODULE2 _IOWR(MODULE1_NUM, MODULE_NUM2, unsigned long*)
#define MODULE3 _IOWR(MODULE1_NUM, MODULE_NUM3, unsigned long*)

static long ch1_mod(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case MODULE1:
			printk("MODULE 1!!!\n");
			number++;
			break;
		case MODULE2:
			printk("MODULE 2!!!\n");
			number++;
			break;
		case MODULE3:
			printk("MODULE 3!!!\n");
			number++;
			break;
		default:
			return -1;
	}
	
	return 0;
}

static int module1_open(struct inode *inode, struct file *file)
{
	printk("module1 open\n");
	return 0;
}

static int module1_release(struct inode *inode, struct file *file)
{
	printk("module1 release\n");
	return 0;
}

struct file_operations module1_char_fops = 
{
	.unlocked_ioctl = ch1_mod,
	.open = module1_open,
	.release = module1_release,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init module1_init(void)
{
	printk("Init Module\n");
	
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &module1_char_fops);
	cdev_add(cd_cdev, dev_num, 1);

	return 0;
}

static void __exit module1_exit(void)
{
	printk("Exit Module\n");
	
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(module1_init);
module_exit(module1_exit);
