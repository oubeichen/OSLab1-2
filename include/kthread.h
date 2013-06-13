#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "x86.h"
#include "adt/list.h"

#define MAX_TH_NUM 512//最多线程数
#define STK_SZ 512 //Stack Size

struct PCB {
	TrapFrame *tf;
	uint32_t lockcnt;//用于记录lock函数执行次数
	pid_t pid;//进程号
	ListHead runq,freeq;//在正在运行线程和可运行线程循环链表中的节点 
	//不做成同一个节点类型的原因是防止出现不在想要的链表而进行错误的操作
	ListHead semq;//信号量链表中节点
	char kstack[STK_SZ];
};
typedef struct PCB PCB;//PCB结构体定义
struct Semaphore {
    	int count;
	ListHead queue;
};
typedef struct Semaphore Semaphore;//信号量结构定义

//指向当前PCB的指针
PCB *current; 
ListHead runqh,freeqh;//正在运行的线程和可运行线程循环链表表头，每次插在表头的前面，即尾进头出,带一个空的表头
ListHead *nowrun;//记录正在运行线程链表中当前运行的线程
PCB pcb_stor[MAX_TH_NUM];//没找到动态分配的方式，所以用静态存储
int pcb_stor_top;//pcb_stor已经使用个数

// 创建一个内核线程
PCB *create_kthread(void *entry);
// 使当前进程/线程立即阻塞，并可以在未来被唤醒
void sleep(void);
// 唤醒一个进程/线程
void wakeup(PCB*);
// 短临界区保护，实现关中断保护的原子操作
void lock(void);
void unlock(void);

int need_sched;
void schedule();//线程调度

//信号量相关函数
void new_sem(Semaphore*,int);
void P(Semaphore*);
void V(Semaphore*);

#endif

