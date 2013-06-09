#include "message.h"
#include "kthread.h"
#include "kernel.h"

void send(pid_t dst, Message *m)
{
	int i;
	if(msg_size == MAXMSG)
	{
		panic("Msg storage is full!");
	}
	while(msg_queue[msg_now].type != 0)
	{
		msg_now = (msg_now + 1) % MAXMSG;
	}
	msg_queue[msg_now].dest = dst;
	msg_queue[msg_now].src = current->pid;
	msg_queue[msg_now].type = m->type;
	for(i = 0;i < MSG_SZ && m->payload[i];i++)
	{
		msg_queue[msg_now].payload[i] = m->payload[i];//拷贝payload
	}
	list_init(&msg_queue[msg_now].mbox_h);
	list_init(&msg_queue[msg_now].msg_h);
	list_add_before(&mbox_head[dst],&msg_queue[msg_now].mbox_h);//加入到对应邮箱
	list_add_before(&msg_head,&msg_queue[msg_now].msg_h);//加入到总队列
	msg_size++;
	V(&full_any);
	V(&full[dst]);
}
void receive(pid_t dst, Message *m)
{
	int i;
	Message *srcmsg;
	P(&full_any);//若总队列都没消息，则阻塞
	if(dst == ANY)
	{
		srcmsg = list_entry(msg_head.next,Message,msg_h);
	}
	dst = srcmsg->dest;//获取要取的邮箱
	P(&full[dst]);//对应邮箱没消息，则阻塞
	m->dest = dst;
	m->type = srcmsg->type;
	m->src = srcmsg->src;
	for(i = 0;i < MSG_SZ && srcmsg->payload[i];i++)
	{
		m->payload[i] = srcmsg->payload[i];//拷贝消息内容
	}
	list_del(&srcmsg->mbox_h);
	list_del(&srcmsg->msg_h);
	srcmsg->type = 0;
	msg_size--;
}
void msg_init()
{
	int i;
	msg_now = 0;	
	msg_size = 0;
	list_init(&msg_head);//初始化总队列
	for(i = 0;i < MAX_TH_NUM;i++)
	{
		list_init(&mbox_head[i]);//初始化每个邮箱队列
	}
	for(i = 0;i < MAXMSG;i++)
	{
		msg_queue[i].type = 0;
	}
	/*for(i = 0;i < MAXMSG;i++)
	{
		list_init(&msg_queue[i].msg_head);
		list_init(&msg_queue[i].mbox_head);
	}*/
	//本想用表头來判断是否存储消息的空间是否被使用，还是用效率高的type來判断吧
}

