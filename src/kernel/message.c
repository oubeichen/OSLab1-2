#include "message.h"
#include "kthread.h"
#include "kernel.h"
#include "string.h"
#include "tty.h"

void send(pid_t dst, Message *m)
{
	int ISHW = 1;
	if(msg_size == MAXMSG)
	{
		panic("Msg storage is full!");
	}
	while(msg_queue[msg_now].type != 0)
	{
		msg_now = (msg_now + 1) % MAXMSG;
	}
	msg_queue[msg_now].src = m->src;
	if(m->src!=MSG_HWINTR)
	{
		lock();
		msg_queue[msg_now].src = current->pid;//不是中断发送的消息
		ISHW = 0;
	}
	printk("onsending to %d ishw = %d current= %d \n",dst,ISHW,current->pid);
	msg_queue[msg_now].dest = dst;
	msg_queue[msg_now].type = m->type;
	memcpy(msg_queue[msg_now].payload,m->payload,MSG_SZ);
	list_init(&msg_queue[msg_now].mbox_h);
	list_add_before(&mbox_head[dst],&msg_queue[msg_now].mbox_h);//加入到对应邮箱
	msg_size++;
	printk("onsending to %d ishw = %d current= %d \n",dst,ISHW,current->pid);
	V(&full[dst]);
	printk("onsending to %d ishw = %d current= %d \n",dst,ISHW,current->pid);
	if(!ISHW)
	{
		unlock();
	}
	printk("onsending to %d ishw = %d current= %d \n",dst,ISHW,current->pid);
}
void receive(pid_t src, Message *m)
{
	pid_t dst;
	//P(&full[current->pid]);
	dst = current->pid;
	lock();
	printk("onreceiving %d from %d\n",dst,src);
	Message *srcmsg = NULL;
	if(src == ANY && !list_empty(&mbox_head[dst]))
	{
		srcmsg = list_entry(mbox_head[dst].next,Message,mbox_h);
	}
	else{
		while(1)
		{
			if(!list_empty(&mbox_head[dst]))
			{
				ListHead *iter = NULL;
				assert(mbox_head[dst].next != NULL);
				list_foreach(iter,&mbox_head[dst])
				{
					printk("listforeach\n");
					//assert(iter != NULL);
					srcmsg = list_entry(iter,Message,mbox_h);
					if(srcmsg->src == src)
					{
						break;
					}
				}	
				if(iter != &mbox_head[dst])
				{
					break;
				}
			}
			P(&full[dst]);//对应邮箱没对应来源消息，则阻塞
		}
	}
	m->dest = dst;
	m->type = srcmsg->type;
	m->src = srcmsg->src;
	memcpy(m->payload,srcmsg->payload,MSG_SZ);
	list_del(&srcmsg->mbox_h);
	srcmsg->type = 0;
	msg_size--;
	printk("receive successful\n");
	unlock();
}
void msg_init()
{
	int i;
	msg_now = 0;	
	msg_size = 0;
	for(i = 0;i < MAX_TH_NUM;i++)
	{
		new_sem(&full[i],0);
		list_init(&mbox_head[i]);//初始化每个邮箱队列
	}
	for(i = 0;i < MAXMSG;i++)
	{
		msg_queue[i].type = 0;
	}
}

