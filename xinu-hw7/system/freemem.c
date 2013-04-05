/**
 * @file freemem.c
 * @provides freemem                                                     
 *
 * COSC 3250 / COEN 4820 Project 8.
 * Modified By:
 * Jason Laqua
 * Kaleb Breault
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <platform.h>
#include <memory.h>
#include <interrupt.h>

/**
 * Free a memory block, returning it to free list.
 * @param pmem pointer to memory block
 * @param nbytes size of memory block
 * @return OK on success, SYSERR on failure
 *
 */
syscall	freemem(void *pmem, ulong nbytes)
{
	ulong im = disable();

	// Insert back into free list, and compact with adjacent blocks.
	if ((0 >= nbytes) || (pmem == NULL))
	{
		restore(im);
		return SYSERR;
	}
	
	memblk *newblock = (memblk *)pmem;
	if (((void *)newblock < minheap) || ((void *)newblock > platform.maxaddr) || ((void *)(newblock + nbytes/8) > platform.maxaddr))
	{
		restore(im);
		return SYSERR;
	}
	
	nbytes = (ulong)roundmb(nbytes);

	memblk *curr = freelist.next;
	memblk *prev = NULL;

	while((curr != NULL) && (curr < newblock))
	{
		prev = curr;
		curr = curr->next;
	}

	newblock->length = nbytes;
	newblock->next = curr;
	if (NULL == prev)
		freelist.next = newblock;
	else
		prev->next = newblock;
	
	if ((newblock + newblock->length/8) == newblock->next)
	{
		newblock->next = curr->next;
		newblock->length += curr->length;
	}
	if (NULL != prev)
	{
		if ((prev + prev->length/8) == newblock)
		{
			prev->next = newblock->next;
			prev->length += newblock->length;
		}
	}
	freelist.length += nbytes;
	restore(im);
	return OK;
}
