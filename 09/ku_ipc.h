#define KUIPC_MAXMSG 10
#define KUIPC_MAXVOL 20

#define IPC_CREAT	0x80
#define IPC_EXCL	0x70
#define IPC_NOWAIT	0x60
#define MSG_NOERROR	0x50

#define IOCTL_START_NUM 0x80
#define IOCTL_CREAT IOCTL_START_NUM+1
#define IOCTL_EXCL IOCTL_START_NUM+2
#define IOCTL_CLOSE IOCTL_START_NUM+3

#define SIMPLE_IOCTL_NUM 'z'
#define IPC_CREAT _IOWR(SIMPLE_IOCTL_NUM, IOCTL_CREAT, unsigned long *)
#define IPC_EXCL _IOWR(SIMPLE_IOCTL_NUM, IOCTL_EXCL, unsigned long *)
#define IPC_CLOSE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_CLOSE, unsigned long *)

#define DEV_NAME "ku_ipc_dev"

struct msg_queue{
	struct list_head list;
	int msqid;
	void *msg[KUIPC_MAXMSG];
	long type[KUIPC_MAXMSG];	
	int size[KUIPC_MAXMSG];
	int length;
};

struct data{
	int msqid;
	void *msgp;
	int msgsz;
	long msgtyp;
	int msgflg;
};
