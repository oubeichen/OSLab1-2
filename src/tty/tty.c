#include "tty.h"

void init_tty(void) {
	//add_irq_handle(1, send_keymsg);
	init_console();
}

