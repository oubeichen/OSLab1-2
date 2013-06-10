#include "kernel.h"
#include "mods/tty.h"
#include "mods/irq.h"
#include "debug.h"

pid_t TTY;

void init_tty(void);
void ttyd(void);
void send_keymsg(void);

void init_console();

void init_tty(void) {
	add_irq_handle(1, send_keymsg);
	init_console();
}

