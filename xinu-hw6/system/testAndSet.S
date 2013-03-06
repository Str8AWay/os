/**
 * @file     testAndSet.S
 * @provides testAndSet
 *
 */
/* Embedded XINU, Copyright (C) 2010.  All rights reserved. */
	
#include <mips.h>
#include <interrupt.h>

	.text
	.align 4

/**
 * @fn bool testAndSet(ulong *lock)
 * 
 * Atomic lock acquisition using MIPS platform opcodes LL and SC.
 * Follows semantics of TestAndSet, textbook Figure 6.4, page 231.
 * Returns 0 if lock acquired, else returns 1.
 */
	.globl testAndSet
	.ent testAndSet
testAndSet:
	ll	t0, 0(a0)
	ori	v0, t0, 1
	beq	v0, t0, tasdone		/* lock already held */
	sc	v0, 0(a0)		/* try atomic update */
	beq	v0, zero, testAndSet	/* failed, retry     */
	move	v0, zero		/* succeeded         */
tasdone:
	jr	ra
	.end testAndSet
