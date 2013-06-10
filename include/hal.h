#ifndef __MOD_HAL_H__
#define __MOD_HAL_H__
#include "x86.h"
#include "adt/list.h"
#include "kthread.h"
#define MSG_DEVRD  1
#define MSG_DEVWR  2
struct MsgHead{
	pid_t src,dest;
	int type;	
};
typedef struct MsgHead MsgHead;

struct DevMessage {
	MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
	//pid_t src,dest;
	int dev_id;
	off_t offset;
	void *buf;
	size_t count;
};
typedef struct DevMessage DevMessage;

struct Device {
	const char *name;
	pid_t pid;
	int dev_id;
	
	ListHead list;
};
typedef struct Device Device;

size_t dev_read(Device *dev, off_t offset, void *buf, size_t count);
size_t dev_write(Device *dev, off_t offset, void *buf, size_t count);

void hal_register(const char *name, pid_t pid, int dev_id);
Device *hal_get(const char *name);
void hal_list(void);
#endif
