#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#include "ku_pir.h"

int ku_pir_open(void);
int ku_pir_close(int fd);
void ku_pir_read(int fd, struct ku_pir_data *data);
void ku_pir_flush(int fd);
int ku_pir_insertData(int fd, long unsigned int ts, char rf_flag);

int ku_pir_open(void){
	int fd=0;
	int dev = open("/dev/ku_pir_dev",O_RDWR);

	if(dev<0){
		printf("dev open error\n");
		return -1;
	}
	
	fd = ioctl(dev, PIR_IOCTL_CREATE, NULL);

	close(dev);

	return fd;
}

int ku_pir_close(int fd){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	
	if(dev<0){
		return -1;
	}
	ioctl(dev, PIR_IOCTL_CLOSE, &fd);
	
	close(dev);

	return 0;
}

void ku_pir_flush(int fd){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	
	ioctl(dev, PIR_IOCTL_FLUSH, &fd);

	close(dev);
}

void ku_pir_read(int fd, struct ku_pir_data *data){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	int len=0;
	struct ku_pir_gift* temp = NULL;

	
	temp = (struct ku_pir_gift*)malloc(sizeof(struct ku_pir_gift));
	temp->fd = fd;
	temp->data = (struct ku_pir_data*)malloc(sizeof(struct ku_pir_data));
	temp->data = data;
	
	len = read(dev,temp, 0);
	
	printf("ts : %lu, rf : %c \n",temp->data->ts, temp->data->rf_flag);
	free(temp);
	close(dev);
}

int ku_pir_insertData(int fd, long unsigned int ts, char rf_flag){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	int len=-1;
	struct ku_pir_gift* temp = NULL;

	if(dev<0){
		return -1;
	}

	temp = (struct ku_pir_gift*)malloc(sizeof(struct ku_pir_gift));
	temp->fd = fd;
	temp->data = (struct ku_pir_data*)malloc(sizeof(struct ku_pir_data));
	temp->data->ts = ts;
	temp->data->rf_flag = rf_flag;

	printf("insert ts : %lu, rf : %c \n",temp->data->ts, temp->data->rf_flag);

	len = write(dev, temp, 0);

	if(len != 0)
		return -1;
	else
		return 0;	
}

