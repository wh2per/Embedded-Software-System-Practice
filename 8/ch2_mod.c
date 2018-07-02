#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

#define LED1 4
#define LED2 5
#define LED3 6

static struct timer_list my_timer;

static void my_timer_func(unsigned long data){
	printk("simple_timer : %ld\n", data);

	switch(my_timer.data){
		case 4:
			gpio_set_value(LED1, 1);
			gpio_set_value(LED2, 0);
			gpio_set_value(LED3, 0);
			break;
		case 5:
			gpio_set_value(LED1, 0);
			gpio_set_value(LED2, 1);
			gpio_set_value(LED3, 0);
			break;
		case 6:
			gpio_set_value(LED1, 0);
			gpio_set_value(LED2, 0);
			gpio_set_value(LED3, 1);
			break;
		default:
			break;	 
	}	
	

	my_timer.data = 4L + (data%3);
	my_timer.expires = jiffies + (1*HZ);

	add_timer(&my_timer);
}

static int __init simple_timer_init(void){
	printk("HI TIMER\n");
	
	init_timer(&my_timer);

	my_timer.function = my_timer_func;
	my_timer.data = 4L;
	my_timer.expires = jiffies + (1*HZ);
	add_timer(&my_timer);

	gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1");
	gpio_request_one(LED2, GPIOF_OUT_INIT_LOW, "LED2");
	gpio_request_one(LED3, GPIOF_OUT_INIT_LOW, "LED3");
	return 0;
}

static void __exit simple_timer_exit(void) {
	printk("Bye TIMER\n");

	gpio_set_value(LED1,0);
	gpio_free(LED1);
	gpio_set_value(LED2,0);
	gpio_free(LED2);
	gpio_set_value(LED3,0);
	gpio_free(LED3);

	del_timer(&my_timer);
}

module_init(simple_timer_init);
module_exit(simple_timer_exit);
