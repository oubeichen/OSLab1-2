#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#define ANY -1
#include "kthread.h"
#define MSG_SZ 16//和DevMessage一样大小
#define MAXMSG 512//消息存储空间大小，因为没法动态分配，只好用一个是一个了
struct Message {
    pid_t src, dest; // 消息发送者和接收者的pid
    int type; // 消息的类型
    char payload[MSG_SZ];
    ListHead mbox_h;//用于放置在单个邮箱队列
};
typedef struct Message Message;

//需要实现的函数
void send(pid_t dst, Message *m);
void receive(pid_t src, Message *m);//应该是src，代表发送的PID
void msg_init(void);//初始化

Message msg_queue[MAXMSG];//MSG存储空间，实际按队列使用
int msg_now;//MSG存储空间没法动态分配，循环使用，每次找到一个当前指针之后的空闲空间
int msg_size;//存储空间当前存放的总消息數
//用MSG的type判断是否正在使用，非0（假设正常type均大于0）则为正在使用，否则就是空闲空间可以用于新建消息
ListHead mbox_head[MAX_TH_NUM];//作为每个线程的邮箱，也是队列
//Semaphore mutex;//receive和send访问消息队列互斥量
Semaphore full[MAX_TH_NUM];//阻塞receive，邮箱中没有消息
#endif
