#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define SENSOR1 17
#define LED1 4
#define DEV_NAME "simple_sensor_dev"

static int irq_num;
static struct timer_list my_timer;


static int simple_sensor_open(struct inode *inode, struct file* file){
	printk("simple_sensor open\n");
	enable_irq(irq_num);
	return 0;
}

static int simple_sensor_release(struct inode *inode, struct file* file){
	printk("simple_sensor close\n");
	disable_irq(irq_num);
	return 0;
}

struct file_operations simple_sensor_fops = {
	.open = simple_sensor_open,
	.release = simple_sensor_release,
};

static void my_timer_func(unsigned long data){
	gpio_set_value(LED1, 0);
}	

static irqreturn_t simple_sensor_isr(int irq, void* dev_id){
	printk("detect \n");
	gpio_set_value(LED1, 1);
	
	my_timer.function = my_timer_func;
  	my_timer.expires = jiffies + (2*HZ);
	add_timer(&my_timer);	
	return IRQ_HANDLED;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_sensor_init(void){
	int ret;

	printk("Init Module\n");

	//timer init
	init_timer(&my_timer);


	gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1");
	//////////////////////

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_sensor_fops);
	cdev_add(cd_cdev, dev_num, 1);

	gpio_request_one(SENSOR1, GPIOF_IN, "sensor1");
	irq_num = gpio_to_irq(SENSOR1);
	ret = request_irq(irq_num, simple_sensor_isr, IRQF_TRIGGER_FALLING, "sensor_irq", NULL);
	if(ret){
		printk(KERN_ERR "Unableto request IRQ: %d\n", ret);
		free_irq(irq_num, NULL);
	}
	
	return 0;
}

static void __exit simple_sensor_exit(void){
	printk("Exit Module \n");
	
	//timer exit
	gpio_free(LED1);
	del_timer(&my_timer);
	/////////

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
	
	free_irq(irq_num, NULL);
	gpio_free(SENSOR1);
}	

module_init(simple_sensor_init);
module_exit(simple_sensor_exit);

