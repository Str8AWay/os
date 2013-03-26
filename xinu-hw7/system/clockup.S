/**
 * @file clockup.S
 * @provides clock_update
 * Platform-dependent code for clock interrupt maintenance.
 */
/* Embedded XINU, Copyright (C) 2010.  All rights reserved. */

#include <mips.h>

.text
.align 4	

/**
 * @fn void clock_update(ulong ticks)
 * 
 * Reset hardware timer to go off again in 'ticks' clock cycles.
 */
.globl clock_update
clock_update:
	mfc0	v0, CP0_COUNT
	addu	v0, v0, a0
	mtc0	v0, CP0_COMPARE
	jr	ra
