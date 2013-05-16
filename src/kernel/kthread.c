#include "kthread.h"
#include "kernel.h"
#include "x86.h"

PCB *creat_kthread(void *entry){
	if(pcb_stor_top >= MAX_TH_NUM){//空间不够
		panic("The PCB storage is full!");
		return NULL;
	}	
	PCB *pcb = pcb_stor + pcb_stor_top++;//分配完后top加一
	TrapFrame *tf = ((TrapFrame*)(pcb->kstack + STK_SZ)) - 1;
       	assert(tf != NULL);
	pcb->tf = tf;
	tf->cs = KSEL(SEG_KCODE);
	tf->ds = tf->es = KSEL(SEG_KDATA);
	tf->eax = tf->ebx = tf->ecx = tf->edx = tf->edi = tf->esi = tf->ebp = 0;
	tf->eflags = 0x00000202;//仅IF设置成1
	tf->eip = (uint32_t)entry;
	list_init(&pcb->freeq);
	list_init(&pcb->runq);
	list_init(&pcb->semq);//初始化各链表节点
	pcb->lockcnt = 0;//lock次数初始为0
	if(freeqh == NULL){//可运行线程链表为空
		freeqh = pcb;
	}
	list_add_before(&freeqh->freeq,&pcb->freeq);//插入到可运行线程链表
	assert(pcb != NULL);
	//加入链表尾部
	return pcb;
}
/*其中，属于寄存器现场的有：
 *
 *     EAX, EBX, ECX, EDX, EDI, ESI, EBP。注意这里没有ESP，这是因为恢复寄存器现场的ESP值实际是TrapFrame的地址。
 *     DS, ES。这两个段寄存器的数值指定了数据的位置，应当赋值为KSEL(SEG_KDATA)。
 *     EIP, CS, EFLAGS。机器状态字必须小心处理，其中CS应当指向内核段，即为KSEL(SEG_KCODE)，而EIP则是线程启动后执行第一条指令的地址。通常，我们都会编写一个函数来代表一个内核线程，而第一条指令就存储在函数的入口地址。EFLAGS寄存器在设定时，要注意IF位必须为1，否则外部中断将永远无法进入系统。
 *     ESP和SS是在ring3进程切换时所用的(当CS的DPL不为0时，硬件会自动执行堆栈切换)，由于内核线程运行在内核态，我们暂时不需要使用它。相关内容请参考i386手册。
 */
void sleep(void){
	list_del(&current->runq);//从正在运行链表中删除
	if(freeqh == NULL){
		freeqh = current;
	}
	list_add_before(&freeqh->freeq,&current->freeq);//插入到可运行线程链表
	asm volatile("int $0x80");
}

void wakeup(PCB *pcb){
	list_del(&current->freeq);//从可运行链表中间删除
	if(runqh == NULL){
		runqh = current;
	}
	list_add_before(&runqh->runq,&current->runq);//插入到正在运行线程链表

}

void lock(void){
	cli();
	current->lockcnt++;
}

void unlock(void){
	current->lockcnt--;
	if(current->lockcnt == 0)
		sti();
}

/*信号量的实现*/
void
new_sem(Semaphore *sem, int value) {
	sem->count = value;
        list_init(&sem->queue);
}

void
P(Semaphore *sem) {
	sem->count --;
	if (sem->count < 0) {
		list_add_before(&sem->queue, &current->semq);
	        sleep(); // 令当前进程立即进入睡眠
	}
}

void
V(Semaphore *sem) {
	sem->count ++;
        if (sem->count <= 0) {
	        assert(!list_empty(&sem->queue));
	        PCB *pcb = list_entry(sem->queue.next, PCB, semq);
	        list_del(sem->queue.next);
	        wakeup(pcb); // 唤醒PCB所对应的进程
	}
}

