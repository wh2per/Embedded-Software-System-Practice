#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/kthread.h>

#include "pi2.h"

MODULE_LICENSE("GPL");

void forward(int round, int delay){
	int i=0;
	int degree;
	int f_delay=delay;

	if(round>360){
		degree = 512*(round/360);
	}else{
		degree = 512/(360/round);
	}

	for(i=0; i<degree; i++){
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);		
		udelay(f_delay);
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);		
		udelay(f_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);		
		udelay(f_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,0);		
		udelay(f_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,0);		
		udelay(f_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,1);		
		udelay(f_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,1);		
		udelay(f_delay);
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,1);		
		udelay(f_delay);
	}		
}

void backward(int round, int delay){
	int i=0;
	int degree;
	int b_delay=delay;

	if(round>360){
		degree = 512*(round/360);
	}else{
		degree = 512/(360/round);
	}

	for(i=0; i<degree; i++){
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,1);		
		udelay(b_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,1);		
		udelay(b_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,1);		
		udelay(b_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,0);		
		udelay(b_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,0);		
		udelay(b_delay);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);		
		udelay(b_delay);
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);		
		udelay(b_delay);
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);		
		udelay(b_delay);
	}		
}

void moveDegree(int degree, int delay, int direction){
	if(direction==0){
		forward(degree,delay);
	}else if(direction==1){
		backward(degree,delay);
	}
}


void forwardSpeaker(int round, int delay){
	int i=0;
	int degree;
	int f_delay=delay;
	int playNote = 0;
	int playDelay = 0;

	if(round>360){
		degree = 512*(round/360);
	}else{
		degree = 512/(360/round);
	}

	for(i=0; i<degree; i++){
		if(playNote%50 >= 0 && playDelay==0)
			gpio_set_value(SPEAKER,1);
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);
		udelay(f_delay);
;
		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);
		udelay(f_delay);
		if(playNote%50 >= 0 && playDelay==0)
			gpio_set_value(SPEAKER,0);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,0);
		udelay(f_delay);

		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,1);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,0);	
		udelay(f_delay);
		if(playNote%50 >= 0 && playDelay==0)
			gpio_set_value(SPEAKER,1);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,0);		
		udelay(f_delay);

		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,1);
		gpio_set_value(PIN4,1);		
		udelay(f_delay);
		if(playNote%50 >= 0 && playDelay==0)
			gpio_set_value(SPEAKER,0);
		gpio_set_value(PIN1,0);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,1);		
		udelay(f_delay);

		gpio_set_value(PIN1,1);
		gpio_set_value(PIN2,0);
		gpio_set_value(PIN3,0);
		gpio_set_value(PIN4,1);		
		udelay(f_delay);
		playNote++;
		if(playNote>50 && playNote <53)
			playDelay=1;
		else if(playNote==53){
			playDelay=0;
			playNote=0;
		}
	}		
}

void playNote(int note){
	int i=0;
	for(i=0; i<100; i++){
		gpio_set_value(SPEAKER,1);
		udelay(note);
		gpio_set_value(SPEAKER,0);
		udelay(note);
	}
}

void play(void){
	int i;
	int notes[] = {952, 952, 952, 952, 952, 952, 952, 952};

	for(i=0; i<8; i++){
		playNote(notes[i]);
		mdelay(100);
	}
	
}

void turnOn(void){
	gpio_set_value(LED1, 1);
	gpio_set_value(LED2, 1);
	gpio_set_value(LED3, 1);
	
}

void turnOff(void){
	gpio_set_value(LED1, 0);
	gpio_set_value(LED2, 0);
	gpio_set_value(LED3, 0);
	
}

static long pi2_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case PI2_DOOROPEN:
			printk("Motor!!!\n");
			moveDegree(3600, 850,1);
			break;
		case PI2_DOORCLOSE:
			moveDegree(360, 850,0);
			break;
		case PI2_SPEAKER:
			printk("Speaker!!!\n");
			play();
			break;
		case PI2_LEDON:
			printk("Light!!!\n");
			turnOn();
			break;
		case PI2_LEDOFF:
			turnOff();
			break;
		case PI2_DOORSPEAKER:
			forwardSpeaker(3600,850);
			break;
		default:
			return -1;
	}
	
	return 0;
}

static int pi2_open(struct inode *inode, struct file *file)
{
	printk("pi2 open\n");
	return 0;
}

static int pi2_release(struct inode *inode, struct file *file)
{
	printk("pi2 release\n");
	return 0;
}


struct file_operations pi2_char_fops = 
{
	.unlocked_ioctl = pi2_ioctl,
	.open = pi2_open,
	.release = pi2_release,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init pi2_init(void){
	
	//ioctl 초기화 
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &pi2_char_fops);
	cdev_add(cd_cdev, dev_num, 1);
	
	//LED 초기화 
	gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1"); 
	gpio_request_one(LED2, GPIOF_OUT_INIT_LOW, "LED2");
	gpio_request_one(LED3, GPIOF_OUT_INIT_LOW, "LED3");
	
	//모터 초기화 
	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");
	
	//스피커 초기화 
	gpio_request_one(SPEAKER, GPIOF_OUT_INIT_LOW, "SPEAKER");
	
	
	return 0;
}

static void __exit pi2_exit(void){
	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);

	gpio_set_value(SPEAKER,0);
	gpio_free(SPEAKER);

	gpio_set_value(LED1, 0);
	gpio_set_value(LED2, 0);
	gpio_set_value(LED3, 0);

	gpio_free(LED1);
	gpio_free(LED2);
	gpio_free(LED3);	
	
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(pi2_init);
module_exit(pi2_exit);
