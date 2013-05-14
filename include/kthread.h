#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "x86.h"
#include "adt/list.h"

#define STK_SZ 512 //Stack Size

struct PCB {
	TrapFrame *tf;
	ListHead runq, freeq;
	char kstack[STK_SZ];
};
typedef struct PCB PCB;
//指向当前PCB的指针
volatile PCB *current;

// 创建一个内核线程
PCB *create_kthread(void*);
// 使当前进程/线程立即阻塞，并可以在未来被唤醒
void sleep(void);
// 唤醒一个进程/线程
void wakeup(PCB*);
// 短临界区保护，实现关中断保护的原子操作
void lock(void);
void unlock(void);

#endif

