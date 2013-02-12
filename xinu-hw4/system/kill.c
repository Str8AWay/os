/**
 * @file kill.c
 * Provides: kill
 *
 * COSC 125/ COEN 183 Assignment 4
 */

/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <kernel.h>
#include <proc.h>
#include <queue.h>

/*
 * kill  --  kill a process and remove it from the system
 */
syscall	kill(int pid)
{
	pcb	*ppcb;  /* points to process control block for pid */

	if ( isbadpid(pid) || (0 == pid)
		|| (PRFREE == (ppcb = &proctab[pid])->state) )
	{ return SYSERR; }

	ppcb = &proctab[pid];

	--numproc;

	switch (ppcb->state)
	{
	case PRCURR:
		ppcb->state = PRFREE;	/* suicide */
		resched();

	case PRREADY:
		remove(pid);

	default:
		ppcb->state = PRFREE;
	}

	return OK;
}
