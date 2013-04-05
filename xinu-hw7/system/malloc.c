/**
 * @file malloc.c
 * @provides malloc                                                       
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
 * Allocate heap storage plus accounting block, returning pointer to
 * assigned memory region.
 * @param nbytes number of bytes requested
 * @return pointer to region on success, SYSERR on failure
 */
void *malloc(ulong nbytes)
{
	// Pad request size with room for accounting info.
	// Call getmem() to allocate region.
	// Store accounting block at head of region, including size
	// of request.  Return pointer to space above accounting
	// block.

	ulong im = disable();
		
	memblk *chunk = (memblk *)getmem(nbytes+8);
	if ((void *)SYSERR == chunk)
	{
		restore(im);
		return (void *)SYSERR;
	}
	chunk->next = chunk;
	chunk->length = nbytes;

	restore(im);
	return (void *)(++chunk);
}
