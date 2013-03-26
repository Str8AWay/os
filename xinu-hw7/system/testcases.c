/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 * Jason Laqua
 * and
 * Kaleb Breault
 */
/* Embedded XINU, Copyright (C) 2010.  All rights reserved. */

#include <kernel.h>
#include <device.h>
#include <stdio.h>
#include <uart.h>
#include <proc.h>
#include <semaphore.h>
#include <mutex.h>
#include <memory.h>
	
void printqueue(queue q)
{
	int head, tail, index;
	head = queuehead(q);
	tail = queuetail(q);
				//dot notation NOT ARROW!!
	index = queuetab[head].next;
	kprintf("/////////////////////////\r\n*queue contents: \r\n");
	while(index!=tail)
	{
	//make sure index is always legit, ie it is between head and tail
		kprintf("%d*[ %3d %3d %3d ]\r\n", 
					index,
					queuetab[index].prev,
					queuetab[index].next,
					queuetab[index].key);
		index = queuetab[index].next;	
	}
	kprintf("END OF QUEUE\r\n/////////////////////////\r\n");
}

void printFreelist(void)
{
	kprintf("//////////////// FREELIST ////////////////\r\n");
	memblk *curr = freelist.next;
	while (curr != NULL)
	{
		kprintf("location 0x%08X: size: %d\r\n", curr, curr->length);
		curr = curr->next;
	}
	kprintf("//////////// END OF FREELIST /////////////\r\n");
}
	
ulong rand(void)
{
	ulong a = 1664525UL;
	ulong b = 1013904223UL;
	static ulong c = 0;

	c = a * c + b;
	return c;
}

syscall sleep(int time)
{
	/* Dumbest sleep ever. */
	int i = 0, j = 0;
	for (i = 0; i < time; i++)
		for (j = 0; j < 1000; j++)
			;
	return 0;
}

/* BEGIN Textbook code from Programming Project 6.40, Silberschatz p. 274 */
typedef int buffer_item;
#define BUFFER_SIZE 5

struct boundedbuffer {
	buffer_item buffer[BUFFER_SIZE];
	int bufferhead;
	int buffertail;

	semaphore empty;
	semaphore full;
	semaphore mutex;
};

int insert_item(struct boundedbuffer *bb, buffer_item item)
{
	// TODO:
	/* insert item into buffer
	 * return 0 if successful, otherwise
	 * return SYSERR indicating an error condition */
	int im = disable();
	
	/*mutexAcquire();
	kprintf("wait full\r\n");
	mutexRelease();*/
	if (wait(bb->full)!=OK)
    {   
    	restore(im);
        return SYSERR;
    }
	//kprintf("I wait mutex\r\n");

	if (wait(bb->mutex)!=OK)
    {   
    	restore(im);
        return SYSERR;
    }
	
	bb->buffer[bb->buffertail] = item;
	bb->buffertail = (bb->buffertail + 1) % BUFFER_SIZE;
	//kprintf("I signal mutex\r\n");
		if (signal(bb->empty)!=OK)
    {   
    	restore(im);
        return SYSERR;
    }
    /*mutexAcquire();
	kprintf("signal empty\r\n");
	mutexRelease();*/

	if (signal(bb->mutex)!=OK)
    {   
    	restore(im);
        return SYSERR;
    }
    
	restore(im);
	return 0;
}

int remove_item(struct boundedbuffer *bb, buffer_item *item)
{
	// TODO:
	/* remove an object from buffer
	 * placing it in item
	 * return 0 if successful, otherwise
	 * return SYSERR indicating an error condition */
	int im = disable();
	
	/*mutexAcquire();	
	kprintf("\t\t\twait empty\r\n");
	mutexRelease();*/
	if (wait(bb->empty)!=OK)
    {   
    	restore(im);
        return SYSERR;
    }
	//kprintf("R wait mutex\r\n");
		if (wait(bb->mutex)!=OK)
    {   
    	restore(im);
        return SYSERR;
    }
	
	*item = bb->buffer[bb->bufferhead];
	bb->bufferhead = (bb->bufferhead + 1) % BUFFER_SIZE;
	/*mutexAcquire();
	kprintf("\t\t\tsignal full\r\n");
	mutexRelease();*/
	if (signal(bb->full)!=OK)
    {   
    	restore(im);
        return SYSERR;
    }
//		kprintf("R signal mutex\r\n");

	if (signal(bb->mutex)!=OK)
    {   
 	  	restore(im);
        return SYSERR;
    }
	
	
	
	restore(im);
	return 0;
}

void producer(struct boundedbuffer *bb)
{
	buffer_item item;
	enable();
	while (1)
	{
		//printqueue(readylist);
		/* sleep for a random period of time */
		sleep(rand() % 100);
		/* generate a random number */
		item = rand();
		if (insert_item(bb, item))
		{
			mutexAcquire();
			kprintf("report error condition\r\n");
			mutexRelease();
		}
		else
		{
			mutexAcquire();
			kprintf("producer %d produced %d\r\n", currpid, item);
			mutexRelease();
		}
	}
}

void consumer(struct boundedbuffer *bb)
{
	buffer_item item;
	enable();
	while (1)
	{
	//	printqueue(readylist);
		/* sleep for a random period of time */
		sleep(rand() % 100);
		if (remove_item(bb, &item))
		{
			mutexAcquire();
			kprintf("report error condition\r\n");
			mutexRelease();
		}
		else
		{
			mutexAcquire();
			kprintf("\t\t\tconsumer %d consumed %d\r\n", currpid, item);
			mutexRelease();
		}
	}
}
/* END Textbook code from Programming Project 6.40, Silberschatz p. 274 */

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	int c;
	mutexInit();
	mutexAcquire();
	kprintf("0) test print freelist\r\n");
	kprintf("Test 1: testing getmem(0)\r\n");
	kprintf("Test 2: testing freemem(<minheap, some size)\r\n");
	kprintf("Test 3: testing freemem(>maxRAM, some size)\r\n");
	kprintf("Test 4: testing getmem(word), freemem(ptr, word)\r\n");
	kprintf("Test 5: Get all of memory in 8 byte chunks, free in 8 byte chunks, get all of memory in 1 big chunk, free all of memory in 1 big chunk\r\n");
	kprintf("Test 6: Testing malloc and free\r\n");
	kprintf("Test 7: Grab 4 chunks of freelist.length/4\r\n");
	kprintf("===TEST BEGIN===\r\n");
	mutexRelease();
	// TODO: Test your operating system!
	ulong arraySize = freelist.length/12;
	ulong* tempArray[arraySize];
	int bignum = 1000;
	while('Q' != (c = kgetc()))
	{
	switch(c)
	{
	case '0':
		mutexAcquire();
		printFreelist();
		mutexRelease();
		break;
	case '1':
		mutexAcquire();
		kprintf("Test 1: testing getmem(0)...");
		mutexRelease();
		if ((void *)SYSERR == getmem(0))
			kprintf("Succeeded\r\n");
		else
			kprintf("Failed\r\n");
		printFreelist();
		break;
	case '2':
		kprintf("Test 2: testing freemem(<minheap, some size)...");
		if (SYSERR == freemem(0,8))
			kprintf("Succeeded\r\n");
		else
			kprintf("Failed\r\n");
		printFreelist();
		break;
	case '3':
		kprintf("Test 3: testing freemem(>maxRAM, some size)...");
		if (SYSERR == freemem((ulong *)0xFFFFFFFF,8))
			kprintf("Succeeded\r\n");
		else
			kprintf("Failed\r\n");
		printFreelist();
		break;
	case '4':
		kprintf("Test 4: testing getmem(word), freemem(ptr, word)...\r\n");
		ulong *dummymem = (ulong *)getmem(8);
		printFreelist();
		freemem(dummymem, 8);
		printFreelist();
		break;
	case '5':
		kprintf("Test 5: Get all of memory in 8 byte chunks, free in 8 byte chunks, get all of memory in 1 big chunk, free all of memory in 1 big chunk\r\n");
		int i;
		
		for (i = 0; i < arraySize; i++)
		{
			if (freelist.length < 8500000 && i % 256 == 0)
				//printFreelist();
				//kprintf("%d\r\n", i);
				kprintf("%d\r\n", freelist.length);
			void *dummy = getmem(8);
			if (dummy != (void *)SYSERR)
				tempArray[i] = dummy;	
		}
		printFreelist();
		for (i = 0; i < arraySize; i++)
		{
			if (i % 1024 == 0)
				//printFreelist();
				//kprintf("%d\r\n", i);
				kprintf("%d\r\n", freelist.length);
			freemem((ulong *)tempArray[i],8);
		}
		printFreelist();

	case 'b':
		kprintf("Test 5b: Get all of memory in 1 big chunk, free all of memory in 1 big chunk twice\r\n");
		ulong allSize = freelist.length;
		ulong *all = (ulong *)getmem(allSize);
		printFreelist();
		freemem(all,allSize);
		printFreelist();
		all = (ulong *)getmem(allSize);
		printFreelist();
		freemem(all,allSize);
		printFreelist();
		break;
	case '6':
		kprintf("Test 6: Testing malloc and free\r\n");
		ulong *dummy = (ulong *)malloc(32);
		if ((void *)SYSERR == dummy)
			kprintf("MALLOC ERROR\r\n");
		kprintf("dummy: 0x%08X 0x%08X\r\n", dummy, *dummy);
		printFreelist();
		if(SYSERR == free(dummy))
			kprintf("FREE ERROR\r\n");
		printFreelist();
		break;
	case '7':
		kprintf("Test 7: Grab 4 chunks of freelist.length/4\r\n");
		ulong freespace = freelist.length/4;
		printFreelist();
		kprintf("\r\nGetting memory\r\n");
		void *dummy1 = getmem(freespace);
		printFreelist();
		void *dummy2 = getmem(freespace);
		printFreelist();
		void *dummy3 = getmem(freespace);
		printFreelist();
		void *dummy4 = getmem(freespace);
		printFreelist();
		kprintf("\r\nFreeing memory\r\n");
		if (dummy1 != (void *)SYSERR)
			freemem(dummy1,freespace);
		printFreelist();
		if (dummy2 != (void *)SYSERR)
			freemem(dummy2,freespace);
		printFreelist();
		if (dummy3 != (void *)SYSERR)
			freemem(dummy3,freespace);
		printFreelist();
		if (dummy4 != (void *)SYSERR)
			kprintf("return code = %d\r\n", freemem(dummy4,freespace));						
		printFreelist();
		break;
	case '8':
		kprintf("Test 8: Test malloc and free with malloc 2 32 byte chunks and free 1 chunk and repeat\r\n");
		ulong *pointera;
		for (i = 0; i < bignum; i++)
		{
			pointera = malloc(32);
			malloc(32);
			if ((void *)SYSERR != pointera)
				free(pointera);
			printFreelist();
		}
	default:
		break;
	}
	}

	while (numproc > 2) { resched(); }
	kprintf("\r\n===TEST END===\r\n");
	return;
}
