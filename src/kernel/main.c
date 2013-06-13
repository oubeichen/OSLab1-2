#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"
#include "kthread.h"
#include "test.h"
#include "hal.h"
#include "time.h"
/*
#define NBUF 5
int buf[NBUF], f = 0, r = 0, g = 1, tid = 1;
Semaphore empty, full, mutex;

void
test_producer(void) {
    while (TRUE) {
        P(&empty);
        P(&mutex);
        buf[f ++] = g ++;
        f %= NBUF;
        V(&mutex);
        V(&full);
    }
}

void
test_consumer(void) {
    int id = tid ++;
    while (TRUE) {
        P(&full);
        P(&mutex);
        printk("#%d Got: %d\n", id, buf[r ++]);
        r %= NBUF;
        V(&mutex);
        V(&empty);
    }
}

void
test_setup(void) {
    new_sem(&full, 0);
    new_sem(&empty, NBUF);
    new_sem(&mutex, 1);
    wakeup(create_kthread(test_producer));
    wakeup(create_kthread(test_producer));
    wakeup(create_kthread(test_producer));
    wakeup(create_kthread(test_consumer));
    wakeup(create_kthread(test_consumer));
    wakeup(create_kthread(test_consumer));
    wakeup(create_kthread(test_consumer));
}
*/
void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8259();
	printk("The OS is now working!\n");
	/*测试字符串输出开始*/
	//int samplehex = -26;
	//char *samplestr = "The hex of -26 is:\n";
	//printk("%%d test:%d + %d = %d\n",-20,10,-10);
	//printk("%%s test and %%x test:%s %x\n",samplestr,samplehex);
	//printk("%%c test: %c is 'a' ,%c is 'c'\n",'a','c'); 
	/*测试字符串输出结束*/
	/*测试生产者消费者代码开始*/
	list_init(&runqh);//另一个指针用不到
	list_init(&freeqh);
	nowrun = &runqh;
	//test_setup();
	/*测试生产者消费者代码结束*/
	//终端测试代码开始
	msg_init();
	init_hal();
	init_timer();
	init_tty();
	PCB *temp = create_kthread(ttyd);
	TTY = temp->pid;
	printk("TTY:%d\n",TTY);
	wakeup(temp);
	printk("begin test\n");
	test();
	//终端测试代码结束
	sti();
	while (TRUE) {
		wait_intr();
	}
}

void
entry(void) {
	init_kvm();
	void(*volatile next)(void) = os_init;
	asm volatile("addl %0, %%esp" : : ""(KOFFSET));
	next();
	panic("init code should never return");
}

