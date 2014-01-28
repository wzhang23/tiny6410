#define APLL_LOCK		(*(volatile unsigned long *)0x7e00f000)
#define MPLL_LOCK		(*(volatile unsigned long *)0x7e00f004)
#define EPLL_LOCK		(*(volatile unsigned long *)0x7e00f008)
#define OTHERS			(*(volatile unsigned long *)0x7e00f900)
#define APLL_CON		(*(volatile unsigned long *)0x7e00f00c)
#define APLL_MDIV		266
#define APLL_PDIV		3
#define APLL_SDIV		1
#define APLL_CON_VAL	((1<<31) | (APLL_MDIV<<16) | (APLL_PDIV<<8) | (APLL_SDIV)) /* 532MHz */
#define MPLL_CON		(*(volatile unsigned long *)0x7e00f010)
#define MPLL_MDIV		266
#define MPLL_PDIV		3
#define MPLL_SDIV		1
#define MPLL_CON_VAL	((1<<31) | (MPLL_MDIV<<16) | (MPLL_PDIV<<8) | (MPLL_SDIV)) /* 532MHz */
#define CLK_DIV0		(*(volatile unsigned long *)0x7e00f020)
#define ARM_RATIO		0	/* ARMCLK 		= DOUT(APLL)/(ARM_RATIO + 1) */
#define MPLL_RATIO		0	/* DOUT(MPLL) 	= MOUT(MPLL)/(MPLL_RATIO + 1) */
#define HCLK_RATIO		1	/* HCLK			= HCLKX2/(HCLK_RATIO + 1) */
#define HCLKX2_RATIO	1	/* HCLKX2		= HCLKX2(in)/(HCLKX2_RATIO + 1) */
#define PCLK_RATIO		3	/* PCLK			= HCLKX2/(PCLK_RATIO + 1) */
#define CLK_DIV0_VAL	((PCLK_RATIO<<12) | (HCLKX2_RATIO<<9) | (HCLK_RATIO<<8) |\
						 (MPLL_RATIO<<4) | ARM_RATIO)
#define CLK_SRC			(*(volatile unsigned long *)0x7e00f01c)

void clock_init(void)
{
	/* Configure ARM PLL lock time */
	APLL_LOCK = 0xffff;
	MPLL_LOCK = 0xffff;
	EPLL_LOCK = 0xffff;

	/* Set async mode when cpu_clock != HCLK */
	OTHERS |= 0xc0;
	while(!(OTHERS & 0xf00));

	/* Fout = MDIV * Fin/(PDIV * 2^SDIV) */
	/* Configure APLL output frequency */
	APLL_CON = APLL_CON_VAL;
	/* Configure MPLL output frequency */
	MPLL_CON = MPLL_CON_VAL;
	/* Configure Clock divider control */
	CLK_DIV0 = CLK_DIV0_VAL;
	/* Select Clock source */
	CLK_SRC |= 0x3;	/* APLL_SEL = Fout(APLL), MPLL_SEL = Fout(MPLL) */
}
