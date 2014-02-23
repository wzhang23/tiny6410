/* s3c6410 UART driver
 */
/*
 *  inlucde file
 */
#include "s3c6410.h"
#include "common.h"

/* Get char from uart buffer
 */
INT8S getc(void)
{
	while ((UFSTAT0_REG & (1<<6)) == 0 && (UFSTAT0_REG & 0x3f) == 0);
	return URXH0_REG;
}

/* Put char to uart tx buffer
 */
void putc(INT8S ch)
{
	while ((UFSTAT0_REG & (1<<14)));
	UTXH0_REG = ch;
}

/* initialize uart module
 */
void uart_init(void)
{
	GPACON_REG &= ~0xff;
	GPACON_REG |= 0x22;
	
	/* ULCON0 */
	ULCON0_REG = 0x3;
	UCON0_REG  = 0x5;
	UFCON0_REG = 0x01;

	UMCON0_REG = 0;
	
	/* DIV_VAL = (PCLK / (bps x 16 ) ) - 1 
	 * bps = 57600
	 * DIV_VAL = (66500000 / (115200 x 16 ) ) - 1 
	 *         = 35.08
	 */
	UBRDIV0_REG   = 35;

	/* x/16 = 0.08
	 * x = 1
	 */
	UDIVSLOT0_REG = 0x1;
}
