/**
 * @file getmem.c
 * @provides getmem                                                       
 *
 * COSC 3250 / COEN 4820 Project 8.
 * Modified By:
 * Jason Laqua
 * Kaleb Breault
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <interrupt.h>
#include <memory.h>

/**
 * Allocate heap storage, returning pointer to assigned memory region.
 * @param nbytes number of bytes requested
 * @return pointer to region on success, SYSERR on failure
 */
void *getmem(ulong nbytes)
{
	ulong im = disable();
	// Search free list for first chunk large enough to fit.
	// Break off region of requested size; return pointer
	// to new memory region, and add any remaining chunk
	// back into the free list.
	if (0 >= nbytes)
	{
		restore(im);
		return (void *)SYSERR;
	}
	
	nbytes = (ulong)roundmb(nbytes);

	memblk *curr = freelist.next;
	memblk *prev = NULL;
	while(TRUE)
	{
		if (curr == NULL)
		{
			restore(im);
			return (void *)SYSERR;
		}
		if (curr->length < nbytes)
		{
			prev = curr;
			curr = curr->next;
		}
		else 
		{
			memblk *newblock;
			newblock = (memblk *)(curr + nbytes/8);
			newblock->next = curr->next;
			newblock->length = curr->length - nbytes;
			if (newblock->length <= 0)
				newblock = curr->next;
			if (NULL == prev)
			{
				freelist.next = newblock;
			} else {
				prev->next = newblock;
			}
			freelist.length -= nbytes;
			restore(im);
			return curr;
		}
	}
}
