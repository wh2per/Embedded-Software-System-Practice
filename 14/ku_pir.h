#ifndef __KU_PIR_HEADER__
#define __KU_PIR_HEADER__

#define KUPIR_MAX_MSG 10
#define KUPIR_SENSOR 17

#define DEV_NAME "ku_pir_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3

#define PIR_IOCTL_NUM 'z'
#define PIR_IOCTL_CREATE _IOWR(PIR_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define PIR_IOCTL_CLOSE _IOWR(PIR_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define PIR_IOCTL_FLUSH _IOWR(PIR_IOCTL_NUM, IOCTL_NUM3, unsigned long *)

struct ku_pir_data {
	long unsigned int ts;		
	char rf_flag; 		
};

struct ku_pir_gift{
	int fd;					// 큐의 FD
	struct ku_pir_data* data;		// 데이터 
};
#endif

