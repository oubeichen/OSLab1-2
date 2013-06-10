#include "x86.h"
#include "kernel.h"
#include "kthread.h"
#include "time.h"
#include "tty.h"
static void
send_updatemsg(void) {
	if (jiffy % (HZ / 10) == 0) {
		Message m;
		m.type = MSG_TTY_UPDATE;
		send(TTY, &m);
	}
}
void irq_handle(TrapFrame *tf) {
	int irq = tf->irq;
	assert(irq >= 0);

	if (irq < 1000) {
		if(irq == 0)
		{
			update_sched();
			update_jiffy();
			send_updatemsg();	
		}
		else if(irq == 1)
		{
			send_keymsg();
		}
		else if(irq == 0x80){//从sleep进来的
			current->tf = tf;//时间片轮，每次运行下一个运行线程链表中的线程
			schedule();
		}else{
			// exception
			cli();
			printk("Unexpected exception #%d\n", irq);
			printk(" errorcode %x\n", tf->err);
			printk(" location  %d:%x, esp %x\n", tf->cs, tf->eip, tf);
			panic("unexpected exception");
		}
	} else if (irq >= 1000) {//其他情况进来的，暂且当成正确的
		// external interrupt
		current->tf = tf;
		//同上时间片轮
		schedule();	
	}
}

