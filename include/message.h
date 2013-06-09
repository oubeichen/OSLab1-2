#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define ANY -1
#define MSG_SZ 10 //暂时10吧，貌似用不上
struct Message {
    int type; // 消息的类型
    pid_t src, dest; // 消息发送者和接收者的pid
    char payload[MSG_SZ];
};
typedef struct Message Message;

//需要实现以下函数

void send(pid_t dst, Message *m);
void receive(pid_t dst, Message *m);
#endif

