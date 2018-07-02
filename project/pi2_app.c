#include <stdio.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "pi2.h"

int pi2_turnOn(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi2_dev", O_RDWR);
	
	ioctl(dev, PI2_LEDON, &value);
	
	close(dev);

	return 0;
}

int pi2_turnOff(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi2_dev", O_RDWR);
	
	ioctl(dev, PI2_LEDOFF, &value);
	
	close(dev);

	return 0;
}

int pi2_doorOpen(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi2_dev", O_RDWR);
	
	ioctl(dev, PI2_DOOROPEN, &value);
	
	close(dev);

	return 0;
}

int pi2_doorClose(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi2_dev", O_RDWR);
	
	ioctl(dev, PI2_DOORCLOSE, &value);
	
	close(dev);

	return 0;
}

int pi2_speakerOn(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi2_dev", O_RDWR);
	
	ioctl(dev, PI2_SPEAKER, &value);
	
	close(dev);

	return 0;
}

int pi2_doorSpeakerOn(){
	int dev;
	int cmd;
	unsigned long value=0;

	dev = open("/dev/pi2_dev", O_RDWR);
	
	ioctl(dev, PI2_DOORSPEAKER, &value);
	
	close(dev);

	return 0;
}


int main(void){
	pid_t pid;
	pid_t end;
	int status;

	pi2_turnOn();
	//pi2_speakerOn();
	//pi2_doorClose();
	//pi2_doorOpen();

	pi2_doorSpeakerOn();
	pi2_turnOff();
	return 0;
}



