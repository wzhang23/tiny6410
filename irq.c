#include "s3c6410.h"
#include "common.h"
#include "stdio.h"

/* External function prototype
 */
extern void rtc_tick_isr(void);

/* Interrupt Service Routine
 */
void eint0_isr(void)
{
	printf("do_irq\n\r");
	printf("IRQ:0x%x\n\r", VIC0INTIRQSTATUS_REG);
	printf("VECTADDR0:0x%x\n\r", VIC0VECTADDR0_REG);
	printf("VECTADDR:0x%x\n\r", VIC0VECTADDR_REG);
	/* Clear interrupt flag */
	EINT0PEND_REG |= 0x3f;
}

/* Interrupt Service Routine
 */
void eint1_isr(void)
{
	printf("do_irq\n\r");
	printf("IRQ:0x%x\n\r", VIC0INTIRQSTATUS_REG);
	printf("VECTADDR0:0x%x\n\r", VIC0VECTADDR0_REG);
	printf("VECTADDR:0x%x\n\r", VIC0VECTADDR_REG);
	/* Clear interrupt flag */
	EINT0PEND_REG |= 0x3f;
}

/*
 * Main IRQ handle
 */
void do_irq(void)
{
	void (*the_isr)(void);

	the_isr = VIC0VECTADDR_REG;

	the_isr();
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
	/* Setting interrupt service routine */
	VIC0VECTADDR0_REG = (INT32U)eint0_isr;
	VIC0VECTADDR1_REG = (INT32U)eint1_isr;
	/* Enable interrupt group int vector interrupt controller */
	VIC0INTENABLE_REG |= 0x3; /* Enable Group 0 & Group 1 */
	/* Set interrupt prority */
	/* ... */

	/* RTC Tick interrupt */
	/* step1: interrupt source init: Enable RTC Tick */
	RTCCON_REG &= 0x0f;	/* Tick sub clock selection as 32768hz */
	TICNT_REG = 0x411a;	/* Tick interrupt interval time is: (0x411a+1)/32768hz = 500ms */
	RTCCON_REG |= 0x100;	/* Enable Tick timer */
	/* step2: Enable interrupt group vector interrupt controller */
	VIC0VECTADDR2_REG = (INT32U)rtc_tick_isr;
	VIC0INTENABLE_REG |= 0x4; /* Enable Group 2 for RTC Tick interrupt */
}
