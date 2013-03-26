/**
 * @file free.c
 * @provides free                                                     
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
 * @param *pmem pointer to memory block
 * @return OK on success, SYSERR on failure
 */
syscall	free(void *pmem)
{
	memblk *block;

	// TODO: Perform some sanity checks to see if pmem is feasible and
	//       could be from a malloc() request:
	//       1) is ptr within heap region of memory?
	//       2) is ptr + length within heap region of memory?
	//       3) does accounting block mnext field point to itself?
	//       4) is accounting block mlen field nonzero?
	//       Call freemem() to put back into free list.
	if ((void *)SYSERR == pmem)
		return SYSERR;
	block = (memblk *)pmem;
	block--;
	if (((void *)block < minheap) || ((void *)(block + block->length/8) > platform.maxaddr))
		return SYSERR;
	if ((block->next != block) || (block->length == 0))
		return SYSERR;
		
	return freemem(block,block->length+8);
}
