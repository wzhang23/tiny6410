.global _start
_start:
	b reset	/* CPU reset, jump to address 0x0 */
	ldr pc, _undefined_instruction /* undefined instruction jump address 0x4 */
	ldr pc, _vector_swi	/* SWI instruction jump address, enter SVC mode. 0x8 */
	b halt	@ldr pc, _prefetch_abort	/* prefetch abort exp. 0xc */
	b halt	@ldr pc, _data_abort	/* data access exp. 0xf */
	b halt	@ldr pc, _not_used	/* not used. 0x14 */
	ldr pc, _irq	/* interrupt exp. 0x18 */
	b halt 	@ldr pc, _fiq	/* fast interrupt exp. 0x1c */

_undefined_instruction:
	.word vector_und
_vector_swi:
	.word vector_swi
_irq:
	.word vector_irq

reset:
	/* Peripheral port setup */
	ldr r0, =0x70000000
	orr r0, r0, #0x13
	mcr p15, 0, r0, c15, c2, 4	@ 256M(0x70000000-0x7fffffff)

	/* Write WTCON(0x7E004000) with 0 */
	ldr r0, =0x7e004000
	mov r1, #0
	str r1, [r0]

	/* Initialize sp and call c funtion */
	ldr sp, =8*1024 /* ARM stack pointer is Full Descending */
									/* Means descending the sp firstly, then store data to stack */
									/* So, here is 8192, not 8191 */

	/* Initialize Clock */
	bl clock_init 

	/* Initialize SDRAMC */
	bl sdram_init

	/* Initialize UART */
	bl uart_init

	/* relocation code and data sections */
	adr r0, _start	/* _start current address: 0x0 */
	ldr r1, =_start /* Link address: 0x50000000 */
	ldr r2, =bss_start /* bss section start address */
	sub r2, r2, r1 /* r2 pass the third parameter of copy2ddr function */
	
	cmp r0, r1
	beq clean_bss

	bl copy2ddr /* copy code and data section from Nand flash to DDR */
	
	/* clean bss section */
clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	mov r2, #0
clean_loop:
	str r2, [r0], #4
	cmp r0, r1
	bne clean_loop
	ldr pc, =on_ddr /* using location-relation code */

on_ddr:
	/* Initialize needed interrupt */
	bl irq_init
	mrs r0, cpsr
	bic r0, r0, #0x9f /* Enable IRQ */
	orr r0, r0, #0x10
	msr cpsr, r0 /* Enter user mode */

	/* Initialize sp and call c funtion */
	ldr sp, =0x50000000 + 0x10000000
	ldr r1, =usr_str
	bl print_cpsr

	swi 0	/*
				 * cpu enter svc mode
				 * store cpsr to spsr_svc
				 * switch to r13_svc, r14_svc
				 * store the next instruction of 'swi' to r14(lr)_svc
				 * jump to address 0x8
				 */
	/* bl hello */

undef:
	.word 0xff000000 /*
										* cpu enter undefined mode
										* store cpsr to spsr_und
										* switch to r13_und, r14_und
										* store the next instruction of 'undef' to r14(lr)_und
										* jump to address 0x4
										*/
	/* Jump runtime address */
	ldr pc, =main /* using location-relation code */
usr_str:
	.word 0x00727375 /* usr */
	
halt:
	b halt

vector_und:
	/* 1. protect current state */
	ldr sp, =0x56000000
	stmdb sp!, {r0-r12, lr}
	/* 2. Handle exp. */
	mrs r0, cpsr
	ldr r1, =und_str
	bl print_cpsr
	/* 3. Restore state */
	ldmia sp!, {r0-r12, pc}^
und_str:
	.word 0x00646e75 /* und */

vector_swi:
	/* 1. protect currect state */
	ldr sp, =0x55000000
	stmdb sp!, {r0-r12, lr} /* lr be the next instruction address */
													/* stmdb would descending firstly, 'db' means decrement before. */
													/* [lower address] 
														 r0, 
														 r1, 
														 r2, 
														 r3, 
														 r4, 
														 r5, 
														 r6, 
														 r7, 
														 r8, 
														 r9, 
														 r10, 
														 r11, 
														 r12, 
														 lr 
														 [higher address] 
													 */
	/* 2. Handle exp. */
	mrs r0, cpsr
	ldr r1, =swi_str
	bl print_cpsr
	/* 3. Restore state */
	ldmia sp!, {r0-r12, pc}^ /* ^ means restore spsr to cpsr. 'ia' means Increment after. */
swi_str:
	.word 0x00697773	/* swi */

vector_irq:
	/* 1. ... */
	ldr sp, =0x54000000
	sub lr, lr, #4 /* Be different from other exp. */
	stmdb sp!, {r0-r12, lr}
	/* 2. ... */
	bl do_irq
	/* 2.1 Find interrupt source */
	/* 2.2 Invoke ISR */
	/* 3. ... */
	ldmia sp!, {r0-r12, pc}^
