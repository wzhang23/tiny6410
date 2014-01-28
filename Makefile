CC_DIR=/opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-
CC=$(CC_DIR)gcc
CC_CMD=-Iinclude -Wall
LD=$(CC_DIR)ld
OBJCOPY=$(CC_DIR)objcopy
OBJDUMP=$(CC_DIR)objdump

main.elf:startup.o clk.o main.o sdram.o
	$(LD) -T main.lds -o main.elf startup.o clk.o main.o sdram.o
	$(OBJCOPY) -O binary main.elf main.bin
	$(OBJDUMP) -D main.elf > main.dis
%.o:%.S
	$(CC) $(CC_CMD) -o $@ $< -c
%.o:%.c
	$(CC) $(CC_CMD) -o $@ $< -c
clean:
	rm -f *.o *.elf *.bin *.dis *.out
