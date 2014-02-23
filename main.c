#include "common.h"
#include "s3c6410.h"
#include "stdio.h"

vu_long j = 0x1234567;
vu_long i = 0;
void delay(INT32U ctr)
{
	INT32U ix;

	for(ix = 0; ix < ctr; ix++)
		;
}

INT32S main(void)
{
	i = 0xf0;

	uart_init();
	GPKDAT_REG = 0xf0;
	GPKCON0_REG = (GPKCON0_REG & ~0xffff0000) | 0x11110000;
	printf("Hello world!\n");

	for(;;)
	{
		printf("Hello world\n");
		if(0x00 == i)
			i = 0xf0;
		i -= 0x10;
		GPKDAT_REG = (GPKDAT_REG & ~0xf0) | i;
		delay(3000000);
	}

	return 0;
}
