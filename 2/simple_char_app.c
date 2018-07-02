#include <stdio.h>
#include <sys/fcntl.h>

void main(void){
	int dev;
	
	dev = open("/dev/simple_char_dev", O_RDWR);
	
	write(dev, "hello_character_device_driver", 30);

	close(dev);
}
