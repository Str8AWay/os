/**
 * @file resched.c
 * @provides resched
 *
 * COSC 125 / COEN 183 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <kernel.h>
#include <proc.h>
#include <queue.h>
#include <clock.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
syscall resched(void)
{
	irqmask im;
	pcb *oldproc;   /* pointer to old process entry */
	pcb *newproc;   /* pointer to new process entry */
	short head, tail;

	im = disable();
	oldproc = &proctab[ currpid ];

	/* TODO: Add queue aging to the system.
	 */

	/* place current process at end of ready queue */
	if (PRCURR == oldproc->state) 
	{
		oldproc->state = PRREADY;
		enqueue(currpid, readylist);
	}

	/* remove first process in ready queue */
	currpid = dequeue(readylist);
	newproc = &proctab[ currpid ];
	newproc->state = PRCURR;	/* mark it currently running	*/

#if PREEMPT
	preempt = QUANTUM;          /* reset preemption counter     */
#endif
	ctxsw(&oldproc->stkptr, &newproc->stkptr);

	/* The OLD process returns here when resumed. */
	restore(im);
	return OK;
}
