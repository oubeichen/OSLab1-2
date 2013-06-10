#ifndef __TIME_H__
#define __TIME_H__
#include "common.h"
#include "assert.h"
#include "x86.h"
#include "debug.h"
#include "string.h"
#include "kthread.h"
#define PORT_TIME 0x40
#define PORT_RTC  0x70
#define FREQ_8253 1193182

#define HZ        100

long jiffy;
struct Time {
	int year, month, day;
	int hour, minute, second;
};
typedef struct Time Time;

void get_time(Time *tm);
void update_sched(void);
void update_jiffy(void);

pid_t TIME;

void init_timer(void);
void timerd(void);

void init_i8253(void);
int read_rtc(int);
void init_timer(void);
#endif
