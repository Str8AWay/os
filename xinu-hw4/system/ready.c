/**
 * @file ready.c
 * @provides ready
 *
 * COSC 125 / COEN 183 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <kernel.h>
#include <proc.h>
#include <queue.h>

/**
 * Make a process eligible for CPU service.
 * @param pid process id of the process to move to the ready state
 * @param resch if TRUE, reschedule will be called
 * @return OK if the process has been added to the ready list, else SYSERR
 */
syscall ready(long pid, bool resch)
{
	register pcb *ppcb;

	ASSERT(!isbadpid(pid));

	ppcb = &proctab[pid];
	ppcb->state = PRREADY;

	enqueue(pid, readylist);

	if (resch) { resched(); }
	return OK;
}
