#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define DEV_NAME "module1_dev"

#define MODULE_START_NUM 0x80
#define MODULE_NUM1 MODULE_START_NUM+1
#define MODULE_NUM2 MODULE_START_NUM+2
#define MODULE_NUM3 MODULE_START_NUM+3

#define MODULE1_NUM 'z'
#define MODULE1 _IOWR(MODULE1_NUM, MODULE_NUM1, unsigned long*)
#define MODULE2 _IOWR(MODULE1_NUM, MODULE_NUM2, unsigned long*)
#define MODULE3 _IOWR(MODULE1_NUM, MODULE_NUM3, unsigned long*)

void main(void)
{
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/module1_dev", O_RDWR);
	
	ioctl(dev, MODULE1, &value);
	ioctl(dev, MODULE2, &value);
	ioctl(dev, MODULE3, &value);

	close(dev);
}
