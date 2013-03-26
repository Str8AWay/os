/**
 * @file     interrupt.S
 * @provides enable, disable, restore, enable_irq, restore_intr
 * Functions to control disabling and enabling of interrupts.
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <mips.h>

	.text
	.globl enable
	.globl disable
	.globl restore
	.globl enable_irq
	.globl restore_intr

/**
 * @fn void enable(void)
 * Enable all interrupts.
 */
enable:
	.set noreorder
	mfc0	a1, CP0_STATUS
	nop
	/* IE = 1: Master enable bit for SW and HW  */
	li		a2, STATUS_IE  
	or		v0, a1, a2
	mtc0	v0, CP0_STATUS
	.set reorder
	jr		ra
	nop

/**
 * @fn irqmask disable(void)
 * Disable interrupts, return old state.
 * @returns state of interrupts before they were disabled
 */
disable:	
	.set noreorder
	mfc0	v0, CP0_STATUS
	li		a0, ~STATUS_IE 
	and		a1, v0, a0
	mtc0	a1, CP0_STATUS
	.set reorder
	jr		ra
	nop

/**
 * @fn irqmask restore(irqmask ps)
 * Restore interrupts to state in ps.
 * @param ps irqmask of interrupt state to restore
 * @return state of interrupts when called
 */
restore:
	.set noreorder
	mfc0	a1, CP0_STATUS
	nop
	or		v0, a1, a0
	mtc0	v0, CP0_STATUS
	.set reorder
	jr		ra
	nop

	
/**
 * @fn void enable_irq(uchar irq)
 * Mask on interrupt request source.
 * @param irq index of the interrupt to enable
 */
enable_irq:
	.set noreorder
	mfc0	a1, CP0_STATUS
	addi	a0, a0, 8
	li		a2, 1
	sllv	a2, a2, a0
	or		v0, a1, a2
	mtc0	v0, CP0_STATUS
	.set reorder
	jr		ra
	nop
		
/**
 * @fn void restore_intr(void) 
 * Turn EXL bit off, allowing exceptions to occur again.
 */
restore_intr:
	.set noreorder
	mfc0	a1, CP0_STATUS
	nop
	li		a2, ~STATUS_EXL
	and		v0, a1, a2
	mtc0	v0, CP0_STATUS
	.set reorder
	jr		a0
	nop
