#ifndef __PI2_HEADER__
#define __PI2_HEADER__

#define DEV_NAME "pi2_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4
#define IOCTL_NUM5 IOCTL_START_NUM+5
#define IOCTL_NUM6 IOCTL_START_NUM+6

#define PI2_IOCTL_NUM 'z'
#define PI2_DOOROPEN _IOWR(PI2_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define PI2_DOORCLOSE _IOWR(PI2_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define PI2_SPEAKER _IOWR(PI2_IOCTL_NUM, IOCTL_NUM3, unsigned long *)
#define PI2_LEDON _IOWR(PI2_IOCTL_NUM, IOCTL_NUM4, unsigned long *)
#define PI2_LEDOFF _IOWR(PI2_IOCTL_NUM, IOCTL_NUM5, unsigned long *)
#define PI2_DOORSPEAKER _IOWR(PI2_IOCTL_NUM, IOCTL_NUM6, unsigned long *)

#define PIN1 6
#define PIN2 13
#define PIN3 19
#define PIN4 26

#define SPEAKER 12

#define LED1 16
#define LED2 20
#define LED3 21

#endif

