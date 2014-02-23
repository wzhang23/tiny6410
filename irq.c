#include "s3c6410.h"
#include "common.h"
#include "stdio.h"

/*
 * Main IRQ handle
 */
void do_irq(void)
{
	printf("do_irq\n\r");
	printf("IRQ:0x%x\n\r", VIC0INTIRQSTATUS_REG);
	printf("VECTADDR0:0x%x\n\r", VIC0VECTADDR0_REG);
	printf("VECTADDR:0x%x\n\r", VIC0VECTADDR_REG);
	/* Clear interrupt flag */
	EINT0PEND_REG |= 0x3f;
	VIC0VECTADDR_REG = 0;
}

/*
 * Initialize needed interrupt here
 */
void irq_init(void)
{
	/* interrupt source init: Set GPIO mode as external interrupt */
	GPNCON_REG &= ~(0xfff);
	GPNCON_REG |= 0xaaa;
	/* interrupt mode init: Set GPIO interrupt trigger mode as both edge */
	EINT0CON0_REG |= 0x7ff;
	/* Enable interrupt */
	EINT0MASK_REG &= ~(0x3f);
	/* Enable interrupt group int vecter interrupt controller */
	VIC0INTENABLE_REG |= 0x3; /* Enable Group 0 & Group 1 */
	/* Set interrupt prority */

}
