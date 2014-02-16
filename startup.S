.global _start
_start:
	/* Peripheral port setup */
	ldr r0, =0x70000000
	orr r0, r0, #0x13
	mcr p15, 0, r0, c15, c2, 4	@ 256M(0x70000000-0x7fffffff)

	/* Write WTCON(0x7E004000) with 0 */
	ldr r0, =0x7e004000
	mov r1, #0
	str r1, [r0]

	/* Initialize sp and call c funtion */
	ldr sp, =8*1024

	/* Initialize Clock */
	bl clock_init 

	/* Initialize SDRAMC */
	bl sdram_init

	/* relocation code and data sections */
	adr r0, _start	/* _start current address: 0x0 */
	ldr r1, =_start /* Link address: 0x50000000 */
	ldr r2, =bss_start /* bss section start address */
	sub r2, r2, r1
	
	cmp r0, r1
	beq clean_bss

	bl copy2ddr
	
	/* clean bss section */
clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	mov r2, #0
clean_loop:
	str r2, [r0], #4
	cmp r0, r1
	bne clean_loop

	/* Initialize sp and call c funtion */
	ldr sp, =0x50000000 + 0x10000000
	/* Jump runtime address */
	ldr pc, =main /* using location-relation code */
	
halt:
	b halt
