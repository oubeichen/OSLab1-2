CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -I./include -O2 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer
ASFLAGS = -m32 -MD -I./include
LDFLAGS = -melf_i386
QEMU    = qemu-system-i386

CFILES  = $(shell find src/ -name "*.c")
SFILES  = $(shell find src/ -name "*.S")
OBJS    = $(CFILES:.c=.o) $(SFILES:.S=.o)

kernel.img: kernel
	@cd boot; make
	cat boot/bootblock kernel > kernel.img

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -e entry -Ttext 0xC0100000 -o kernel $(OBJS)

-include $(OBJS:.o=.d)

.PHONY: play debug clean

play: kernel.img
	$(QEMU) -serial stdio kernel.img --no-kvm

debug: kernel.img
	$(QEMU) -serial stdio -s -S kernel.img --no-kvm

clean:
	@cd boot; make clean
	rm -f kernel kernel.img $(OBJS) $(OBJS:.o=.d)
	rm -f *.d
