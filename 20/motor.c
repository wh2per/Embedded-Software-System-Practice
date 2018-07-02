#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define PIN1 6
#define PIN2 13
#define PIN3 19
#define PIN4 26

#define STEPS 8

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

static int __init simple_motor_init(void){
	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");
	
	moveDegree(360, 850,0);
		
	
	return 0;
}

static void __exit simple_motor_exit(void){
	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);
}

module_init(simple_motor_init);
module_exit(simple_motor_exit);
