/**
 * @file clkintr.c
 * @provides clockintr
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <queue.h>
#include <clock.h>

extern void wakeup(void);
extern syscall resched(void);
extern void restore_intr(void *);

/**
 * XINU Clock handler.
 */
interrupt clockintr(void)
{
	clock_update(time_intr_freq);

	/* Another millisecond has passed. */
	ctr_mS--;

	/* Update global second counter. */
	if (0 == ctr_mS) 
	{
		clocktime++;
		ctr_mS = 1000;
	}

#if PREEMPT
	/* Check to see if this proc should be preempted. */
	if (--preempt <= 0)
	{  
		restore_intr(&resched);
	}
#endif
}
