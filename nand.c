/*
 * Nand flash driver
 * History:	2/9/2014	nand flash driver initial version
 */

/*
 * Include files
 */
#include "s3c6410.h"

/*
 * File scope macro define
 */
/* Custom section for nand code */
#define _init	__attribute__((section(".startup")))
#define PAGE_SIZE	(8192)

/*
 * Local function prototype
 */
static _init void nand_select(void);
static _init void nand_deselect(void);
static _init void nand_cmd(unsigned char cmd);
static _init void nand_addr(unsigned char addr);
static _init unsigned char nand_get_data(void);
static _init void nand_send_data(unsigned char data);
static _init void nand_wait_ready(void);
static _init void nand_reset(void);
static _init void nand_send_addr(unsigned long addr);

/*
 * Function Name:	nand_select
 * Parameters		: None
 * Description	:	nand chip select
 * Invoked			:	
 * Return				: None
 */
static _init void nand_select(void)
{
	NFCONT_REG &= ~(1<<1);
}

/*
 * Function Name:	nand_deselect
 * Parameters		: None
 * Description	:	nand chip deselect
 * Invoked			:	
 * Return				: None
 */
static _init void nand_deselect(void)
{
	NFCONT_REG |= (1<<1);
}

/*
 * Function Name:	nand_cmd
 * Parameters		: unsigned char cmd
 * Description	:	send command to nand flash
 * Invoked			:	
 * Return				: None
 */
static _init void nand_cmd(unsigned char cmd)
{
	NFCMD_REG = cmd;
}

/*
 * Function Name:	nand_addr
 * Parameters		: unsigned char addr
 * Description	:	send address data to nand flash
 * Invoked			:	
 * Return				: None
 */
static _init void nand_addr(unsigned char addr)
{
	NFADDR_REG = addr;
}

/*
 * Function Name:	nand_get_data
 * Parameters		: None
 * Description	:	Get data from nand flash
 * Invoked			:	
 * Return				: unsigned char - nand data
 */
static _init unsigned char nand_get_data(void)
{
	/* NOTE: NFDATA register must be defined as 1Byte data */
	return NFDATA_REG;
}

/*
 * Function Name:	nand_send_data
 * Parameters		: unsigned char data
 * Description	:	send data to nand flash
 * Invoked			:	
 * Return				: None
 */
static _init void nand_send_data(unsigned char data)
{
	NFDATA_REG = data;
}

/*
 * Function Name:	nand_wait_ready
 * Parameters		: None
 * Description	:	Waiting for nand flash ready
 * Invoked			:	
 * Return				: None
 */
static _init void nand_wait_ready(void)
{
	while ((NFSTAT_REG & 0x1) == 0);
}

/*
 * Function Name:	nand_reset
 * Parameters		: None
 * Description	:	Reset nand flash
 * Invoked			:	
 * Return				: None
 */
static _init void nand_reset(void)
{
	nand_select();
	
	nand_cmd(0xff);

	nand_wait_ready();
	
	nand_deselect();
}

/*
 * Function Name:	nand_init
 * Parameters		: None
 * Description	:	Initialize nand flash controller
 * Invoked			:	
 * Return				: None
 */
void _init nand_init(void)
{
	GPOCON_REG = (GPOCON_REG & ~0x0f) | 0xa; // nCS[3:2]
	GPOPUD_REG &= ~0xf;
	GPPCON_REG = (GPPCON_REG & ~(0xfff << 4)) | (0xaaa << 4);// nWAIT, FALE, FCLE,
	GPPPUD_REG &= ~(0xfff << 4); // FWEn, FREn, FRnB

	/* xm0csn2 nand flash cs0 */
	MEM_SYS_CFG_REG &= ~(1<<1);

#define TACLS     7
#define TWRPH0    7
#define TWRPH1    7
	NFCONF_REG &= ~((1<<30) | (7<<12) | (7<<8) | (7<<4));
	NFCONF_REG |= ((TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4));

	NFCONT_REG |= 1;
	NFCONT_REG &= ~(1<<16);

	nand_reset();
}

/*
 * Function Name:	nand_send_addr
 * Parameters		: unsigned long addr
 * Description	:	write address data to nand flash
 * Invoked			:	
 * Return				: None
 */
static _init void nand_send_addr(unsigned long addr)
{
#if 0
	nand_addr(addr & 0xff);         /* a0~a7 */
	nand_addr((addr >> 8) & 0x3f);  /* a8~a13 */

	nand_addr((addr >> 14) & 0xff); /* a14~a21 */
	nand_addr((addr >> 22) & 0xff); /* a22~a29 */
	nand_addr((addr >> 30) & 0xff); /* a30~    */
#endif
	unsigned long row = addr/PAGE_SIZE;
	unsigned long column = addr%PAGE_SIZE;

	NFADDR_REG = column&0xff;
	NFADDR_REG = (column>>8)&0xff;
	NFADDR_REG = row&0xff;
	NFADDR_REG = (row>>8)&0xff;
	NFADDR_REG = (row>>16)&0xff;
}

/*
 * Function Name:	nand_read
 * Parameters		: unsigned int nand_start
 * 								unsigned int ddr_start
 * 								unsigned int len
 * Description	:	Read data from nand flash
 * Invoked			:	
 * Return				: None
 */
void _init nand_read(unsigned int nand_start, unsigned int ddr_start, unsigned int len)
{
	unsigned long rest = len;
	unsigned long addr = nand_start;
	unsigned long page;
	unsigned char *dest = (unsigned char *)ddr_start;
	int i;
	
	nand_select();
	while(rest > 0)
	{
		nand_cmd(0x00);
		nand_send_addr(addr);
		nand_cmd(0x30);
		nand_wait_ready();
		page = rest > PAGE_SIZE ? PAGE_SIZE:rest;
		for (i = 0; i != page; ++i)
		{
		#if 0 /* This code is also correct */
			*dest++ = *((volatile unsigned char *)0x70200010);
		#endif
			*dest++ = nand_get_data();
		}
		rest -= page;
		addr += page;
	}
	nand_deselect();
}

/*
 * Function Name:	nand_erase_block
 * Parameters		: unsigned long address
 * Description	:	Erase a block of nand flash
 * Invoked			:	
 * Return				: None
 */
_init void nand_erase_block(unsigned long addr)
{
	int page = addr / PAGE_SIZE;
	
	nand_select();
	nand_cmd(0x60);
	
	nand_addr(page & 0xff);
	nand_addr((page >> 8) & 0xff);
	nand_addr((page >> 16) & 0xff);

	nand_cmd(0xd0);
	nand_wait_ready();

	nand_deselect();
}

/*
 * Function Name:	nand_write
 * Parameters		: unsigned int nand_start
 * 								unsigned char * buf
 * 								unsigned int len
 * Description	:	write data to nand flash
 * Invoked			:	
 * Return				: None
 */
_init void nand_write(unsigned int nand_start, unsigned char * buf, unsigned int len)
{
	unsigned long count = 0;
	unsigned long addr  = nand_start;
	int i = nand_start % PAGE_SIZE;
	
	nand_select();
	while (count < len)
	{
		nand_cmd(0x80);
		nand_send_addr(addr);
		for (; i < PAGE_SIZE && count < len; i++)
		{
			nand_send_data(buf[count++]);
			addr++;
		}

		nand_cmd(0x10);
		nand_wait_ready();
		i = 0;		
	}
	nand_deselect();
}

/*
 * Function Name:	copy2ddr
 * Parameters		: unsigned int nand_start
 * 								unsigned int ddr_start
 * 								unsigned int len
 * Description	:	copy code from nand flash to SDRAM
 * Invoked			:	Only by startup code
 * Return				:	The result of read nand flash 
 * Notes				: S3C6410 copy the first 8k code from nand flash to SDRAM automatically.
 * 								If the nand flash page is less than 2k, the code placed into page0,1,2,3,...
 * 								Otherwise, the code placed into page0(2k), page1(2k),..., even the page is
 * 								more than 2k(for example 8k). 
 */
_init int copy2ddr(unsigned long nand_start, unsigned long ddr_start, unsigned long len)
{
	unsigned long rest = len;
	unsigned long size;
	unsigned long i;

	nand_init();
	/* Even the nand page size is 8k, the first 4 page hold 2k code. */
	for(i = 0; i != 8; ++i)
	{
		size = rest > 2048 ? 2048 : rest;
		nand_read(PAGE_SIZE*i, ddr_start+i*2048, size);
		rest -= size;
		if(rest == 0)
			return 0;
	}

	/* Read the rest code from nand flash to SDRAM */
	nand_read(PAGE_SIZE*8, ddr_start+PAGE_SIZE*2, rest);

	return 0;
}
