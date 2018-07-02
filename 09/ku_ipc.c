#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "ku_ipc.h"

MODULE_LICENSE("GPL");
struct msg_queue my_queue;
spinlock_t slock;

static long ku_ipc_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	struct msg_queue *mq=0;
	struct list_head *pos=0;
	int i;
	
	switch(cmd){
		case IOCTL_CREAT:
			printk("IOCTL CREAT!\n");
			//있는지 검사한다 
			list_for_each(pos, &(my_queue.list)){
				mq = list_entry(pos, struct msg_queue, list);
				if(mq->msqid == *arg)
					return mq->msqid;
			}
			mq = (struct msg_queue *)kmalloc(sizeof(struct msg_queue), GFP_KERNEL);
			mq->msqid = *arg;
			
			//큐 초기화 
			for(i=0; i<KUIPC_MAXMSG; i++){
				mq->msg[i] = (void*)kmalloc(KUIPC_MAXMSG*KUIPC_MAXVOL, GFP_KERNEL);
				memset(mq->msg[i], 0, KUIPC_MAXMSG);
				mq->type[i] = 0;
				mq->size[i] = 0;
			}
			mq->length = 0;
			
			//큐 추가 
			list_add(&(mq->list),&(my_queue.list));

			return *arg;

		case IOCTL_EXCL:
			printk("IOCTL EXCL!\n");
			//있는지 검사한다 
			list_for_each(pos, &(my_queue.list)){
				mq = list_entry(pos, struct msg_queue, list);
				if(mq->msqid == *arg)
					return -1;
			}
	
			mq = (struct msg_queue *)kmalloc(sizeof(struct msg_queue), GFP_KERNEL);
			mq->msqid = *arg;
			//큐 초기화 
			for(i=0; i<KUIPC_MAXMSG; i++){
				mq->msg[i] = (void*)kmalloc(KUIPC_MAXMSG*KUIPC_MAXVOL, GFP_KERNEL);
				memset(mq->msg[i], 0, KUIPC_MAXMSG);
				mq->type[i] = 0;
				mq->size[i] = 0;
			}
			mq->length = 0;

			//큐 추가 
			list_add(&(mq->list),&(my_queue.list));

			return *arg;
		case IOCTL_CLOSE:
			printk("IOCTL CLOSE!\n");
			//있는지 검사한다 
			list_for_each(pos, &(my_queue.list)){
				mq = list_entry(pos, struct msg_queue, list);
				if(mq->msqid == *arg)
					return 0;
			}
			//없으면 
			return -1;

		default:
			printk("WRONG INPUT!\n");
			break;
	}
	
	return 0;
}

static int ku_ipc_open(struct inode *inode, struct file *file){
	printk("ku_ipc open\n");
	return 0;
}

static int ku_ipc_release(struct inode *inode, struct file *file){
	printk("ku_ipc release\n");
	return 0;
}

static int ku_ipc_write(struct file *file, char *buf, size_t len, loff_t *lof) {
	struct msg_queue *mq=0;
	struct list_head *pos=0;
	struct data *tmp;

	int i;

	tmp = (struct data*)kmalloc(sizeof(struct data), GFP_KERNEL);
	copy_from_user(tmp->msqid,((struct data*)buf)->msqid,sizeof(int));
	copy_from_user(tmp->msgp,((struct data*)buf)->msgp,len*(sizeof(char)));
	copy_from_user(tmp->msgsz,((struct data*)buf)->msgsz,sizeof(int));
	copy_from_user(tmp->msgtyp,((struct data*)buf)->msgtyp,sizeof(long));
	copy_from_user(tmp->msgflg,((struct data*)buf)->msgflg,sizeof(int));

	list_for_each(pos, &(my_queue.list)){
		mq= list_entry(pos, struct msg_queue, list);
		if(mq->msqid == tmp->msqid){
			spin_lock(slock);
			if(mq->length >= KUIPC_MAXMSG){
				if(tmp->msgflg == IPC_NOWAIT) {
					kfree(tmp);
					spin_unlock(slock);
					return -1;
				}else{
					spin_unlock(slock);
					while(mq->length >= KUIPC_MAXMSG);
						spin_lock(slock);
				}
			}
			mq->type[mq->lenegth] = tmp->msgtyp;
			mq->msg[mq->lenegth] = tmp->msgp;
			mq->size[mq->lenegth] = tmp->length;
			mq->length++;
			spin_unlock(slock);

			kfree(tmp);
			return 0;
			}
		}
	}
}

static int ku_ipc_read(struct file *file, char *buf, size_t len, loff_t *lof){
	struct msg_queue *mq=0;
	struct list_head *pos=0;
	struct data *tmp;
	int i,j;
	int result=0;
	
	tmp = (struct data*)kmalloc(sizeof(struct data), GFP_KERNEL);
	//tmp->msgp = (void*)kmalloc(sizeof(void));	
	copy_from_user(tmp->msqid,((struct data*)buf)->msqid,sizeof(int));
	copy_from_user(tmp->msgp,((struct data*)buf)->msgp,len*(sizeof(char)));
	copy_from_user(tmp->msgsz,((struct data*)buf)->msgsz,sizeof(int));
	copy_from_user(tmp->msgtyp,((struct data*)buf)->msgtyp,sizeof(long));
	copy_from_user(tmp->msgflg,((struct data*)buf)->msgflg,sizeof(int));
						
	list_for_each(pos, &(my_queue.list)){
		mq= list_entry(pos, struct msg_queue, list);
		if(mq->msqid == tmp->msqid){	//큐 아이디가 같으면 
			spin_lock(slock);		//락걸고 
			if(tmp->len <=0)		//내용없으면 
				return -1;
			else{		//내용 있으면 
				for(i=0; i<mq->len; i++){
					if(mq->type[i] == tmp->msgtyp){
						copy_to_user(tmp->msgtyp, &(mq->type[i]), sizeof(long));
						copy_to_user(tmp->msgp, mq->msg[i], mq->size[i]);
						result = tmp->size[i];
						mq->length--;

						for(j=i; j<mq->len; j++) {
							mq->msg[j] = mq->msg[j+1];
							mq->size[j] = mq->size[j+1];
							mq->type[j] = mq->type[j+1];
						}

						kfree(tmp);
						spin_unlock(slock);
						return result;
					}
				}
			}
			kfree(tmp);
			spin_unlock(slock);
			return -1;
		}
	}
	kfree(tmp);
	return -1;	
}

struct file_operations ku_ipc_fops =
{
	.unlocked_ioctl = ku_ipc_ioctl,
	.open = ku_ipc_open,
	.release = ku_ipc_release,
	.read = ku_ipc_read,
	.write = ku_ipc_write,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_ipc_init(void){
	printk("Init Module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ku_ipc_fops);
	cdev_add(cd_cdev, dev_num, 1);
	
	INIT_LIST_HEAD(&(my_queue.list));
	spin_lock_init(&slock);

	return 0;
}

static void __exit ku_ipc_exit(void){
	printk("Exit Module\n");
	
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(ku_ipc_init);
module_exit(ku_ipc_exit);
