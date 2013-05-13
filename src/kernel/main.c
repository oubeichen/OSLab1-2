#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"

void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8259();
	printk("The OS is now working!\n");
	/*测试字符串输出开始*/
	int samplehex = -26;
	char *samplestr = "The hex of -26 is:\n";
	printk("%%d test:%d + %d = %d\n",-20,10,-10);
	printk("%%s test and %%x test:%s %x\n",samplestr,samplehex);
	printk("%%c test: %c is 'a' ,%c is 'c'",'a','c'); 
	/*测试字符串输出结束*/
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

