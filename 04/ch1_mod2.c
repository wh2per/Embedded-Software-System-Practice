#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
MODULE_LICENSE("GPL");

extern int number;
#define DEV_NAME "module2_dev"

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init module2_init(void)
{
	printk("Init module2\n");

	printk("moudle1 number : %d\n",number);
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_add(cd_cdev, dev_num, 1);

	return 0;
}	

static void __exit module2_exit(void)
{
	printk("Exit module2\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(module2_init);
module_exit(module2_exit);
