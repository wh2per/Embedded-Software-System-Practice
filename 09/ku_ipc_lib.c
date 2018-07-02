#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#include "ku_ipc.h"

int ku_msgget(int key, int msgflg){
	int dev;
	int result;
	unsigned long value = key;	

	dev = open("/dev/ku_ipc_dev",O_RDWR);
	
	switch(msgflg){
		case IPC_CREAT:
			result = ioctl(dev, IOCTL_CREAT, &value);
			break;
		case IPC_EXCL:
			result = ioctl(dev, IOCTL_EXCL, &value);
			break;
		default:
			result = -1;
			break;
	}
	
	close(dev);

	return result;
}

int ku_msgclose(int msqid){
	int dev;
	int result;
	unsigned long value = msqid;
	
	dev = open("/dev/ku_ipc_dev", O_RDWR);
	
	result = ioctl(dev, IOCTL_CLOSE, &value);
	
	close(dev);

	return result;
}

int ku_msgsnd(int msqid, void *msgp, int msgsz, int msgflg){
	int dev;
	int result;
	
	struct data msgData;

	dev = open("/dev/ku_ipc_dev", O_RDWR);
	
	msgData.msqid = msqid;
	msgData.msgp = msgp;
	msgData.msgsz = msgsz;
	msgData.msgflg = msgflg;

	result = write(dev, (char*)&msgData,msgsz);
	
	close(dev);

	return result;
}

int ku_msgrcv(int msqid, void *msgp, int msgsz, long msgtyp, int msgflg){
	int dev;
	int result;

	struct data msgData;

	dev = open("/dev/ku_ipc_dev",O_RDWR);

	msgData.msqid = msqid;
	msgData.msgp = msgp;
	msgData.msgsz = msgsz;
	msgData.msgtyp = msgtyp;
	msgData.msgflg = msgflg;

	result = read(dev, (char*)&msgData, msgsz);

	close(dev);

	return result;
}
