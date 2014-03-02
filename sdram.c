#include "common.h"
#include "s3c6410.h"

#define _init	__attribute__((section(".startup")))

#define HCLK	133000000

#define nstoclk(ns)	(ns/( 1000000000/HCLK)+1)

void _init sdram_init( void )
{
	// tell dramc to configure				
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_MEMC_CMD), 0x4 );

	// set refresh period	
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_REFRESH_PRD), nstoclk(7800) );

	// set timing para		
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_CAS_LATENCY), ( 3 << 1 ) );  
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_DQSS), 0x1 );	// 0.75 - 1.25
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_MRD), 0x2 );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_RAS), nstoclk(45) );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_RC), nstoclk(68) );		

	INT32U trcd = nstoclk( 23 );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_RCD), trcd | (( trcd - 3 ) << 3 ) );
	INT32U trfc = nstoclk( 80 );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_RFC), trfc | ( ( trfc-3 ) << 5 ) );   
	INT32U trp = nstoclk( 23 );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_RP), trp | ( ( trp - 3 ) << 3 ) ); 
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_RRD), nstoclk(15) );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_WR), nstoclk(15) );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_WTR), 0x7 );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_XP), 0x2 );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_XSR), nstoclk(120) );
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_T_ESR), nstoclk(120) );
	
	// set mem cfg 
	set_nbit( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG), 0, 3, 0x2 );  /* 10 column address */
	set_nbit( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG), 3, 3, 0x3 );  /* 14 row address */
	set_zero( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG), 6 );		  /* A10/AP */
	set_nbit( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG), 15, 3, 0x2 ); /* Burst 4 */
	set_nbit( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG2), 0, 4, 0x5 );
	set_2bit( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG2), 6, 0x1 );		/* 32 bit */
	set_nbit( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG2), 8, 3, 0x3 );	/* Mobile DDR SDRAM */
	set_2bit( (ELFIN_DMC1_BASE + INDEX_DMC_MEMORY_CFG2), 11, 0x1 );
	set_one( (ELFIN_DMC1_BASE + INDEX_DMC_CHIP_0_CFG), 16 );		/* Bank-Row-Column organization */

	// memory init
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_DIRECT_CMD), 0xc0000 ); // NOP
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_DIRECT_CMD), 0x000 );	// precharge
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_DIRECT_CMD), 0x40000 );// auto refresh
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_DIRECT_CMD), 0x40000 );// auto refresh
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_DIRECT_CMD), 0xa0000 ); // EMRS
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_DIRECT_CMD), 0x80032 ); // MRS

	set_val( (ELFIN_CLOCK_POWER_BASE + MEM_SYS_CFG_OFFSET), 0x0 );
					
	// set dramc to "go" status	
	set_val( (ELFIN_DMC1_BASE + INDEX_DMC_MEMC_CMD), 0x000 );

	// wait ready
	while( !(( read_val( (ELFIN_DMC1_BASE + INDEX_DMC_MEMC_STATUS )) & 0x3 ) == 0x1));
}
