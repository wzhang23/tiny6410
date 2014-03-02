CC_DIR=/opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-
CC=$(CC_DIR)gcc
CC_CMD=-Iinclude -Wall
LD=$(CC_DIR)ld
AR=$(CC_DIR)ar
NM=$(CC_DIR)nm
OBJCOPY=$(CC_DIR)objcopy
OBJDUMP=$(CC_DIR)objdump

INCLUDEDIR := $(shell pwd)/include
CFLAGS		:= -Wall -fno-builtin -nostdinc -I$(INCLUDEDIR) -fno-builtin-printf -nostdlib -mcpu=arm1176jzf-s

export CC AR LD NM OBJCOPY OBJDUMP INCLUDEDIR CFLAGS 

objs	:= startup.o clk.o uart.o main.o irq.o lib/libc.a sdram.o nand.o

main.bin:$(objs)
	$(LD) -T main.lds -o main.elf $^
	$(OBJCOPY) -O binary main.elf $@
	$(OBJDUMP) -D main.elf > main.dis
	$(NM) -B --numeric-sort main.elf > main.map

.PHONY :	lib/lib.a
lib/libc.a:
	cd lib; make; cd ..

%.o:%.S
	$(CC) $(CFLAGS) -c -o $@ $<
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	make clean -C lib
	rm -f *.o *.elf *.bin *.dis *.out *.map
