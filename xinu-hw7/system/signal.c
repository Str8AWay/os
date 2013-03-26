/**
 * @file signal.c
 * @provides signal.
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <proc.h>
#include <semaphore.h>

/**
 * signal a semaphore, releasing one waiting thread
 * @param sem  target semaphore
 * @return OK on success, SYSERR on failure
 */
syscall signal(semaphore sem)
{
    register semblk *semptr = NULL;
    irqmask im;

    im = disable();
    if (isbadsem(sem))
    {
        restore(im);
        return SYSERR;
    }
    semptr = &semtab[sem];
    if ((semptr->count++) < 0)
    {
        ready(dequeue(semptr->queue), RESCHED_YES);
    }
    restore(im);
    return OK;
}
