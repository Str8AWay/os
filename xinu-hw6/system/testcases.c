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
	
	/*mutexAcquire();
	kprintf("wait full\r\n");
	mutexRelease();*/
	if (wait(bb->full)!=OK)
    {   
        return SYSERR;
    }
	//kprintf("I wait mutex\r\n");

	if (wait(bb->mutex)!=OK)
    {   
        return SYSERR;
    }
	
	bb->buffer[bb->buffertail] = item;
	bb->buffertail = (bb->buffertail + 1) % BUFFER_SIZE;
	//kprintf("I signal mutex\r\n");
		if (signal(bb->empty)!=OK)
    {   
        return SYSERR;
    }
    /*mutexAcquire();
	kprintf("signal empty\r\n");
	mutexRelease();*/

	if (signal(bb->mutex)!=OK)
    {   
        return SYSERR;
    }
    
	return 0;
}

int remove_item(struct boundedbuffer *bb, buffer_item *item)
{
	// TODO:
	/* remove an object from buffer
	 * placing it in item
	 * return 0 if successful, otherwise
	 * return SYSERR indicating an error condition */
	
	/*mutexAcquire();	
	kprintf("\t\t\twait empty\r\n");
	mutexRelease();*/
	if (wait(bb->empty)!=OK)
    {   
        return SYSERR;
    }
	//kprintf("R wait mutex\r\n");
		if (wait(bb->mutex)!=OK)
    {   
        return SYSERR;
    }
	
	*item = bb->buffer[bb->bufferhead];
	bb->bufferhead = (bb->bufferhead + 1) % BUFFER_SIZE;
	/*mutexAcquire();
	kprintf("\t\t\tsignal full\r\n");
	mutexRelease();*/
	if (signal(bb->full)!=OK)
    {   
        return SYSERR;
    }
//		kprintf("R signal mutex\r\n");

	if (signal(bb->mutex)!=OK)
    {   
        return SYSERR;
    }
	
	
	
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
	struct boundedbuffer bbuff;
	mutexInit();
	mutexAcquire();
	kprintf("0) Test 1 producer, 1 consumer, same priority\r\n");
	kprintf("1) Test 1 producer, 1 consumer, producer higher priority\r\n");
	kprintf("2) Test 1 producer, 1 consumer, consumer higher priority\r\n");
	kprintf("3) Test 2 producers, 1 consumer, same priority\r\n");
	kprintf("4) Test 2 producers, 1 consumer, producer higher priority\r\n");
	kprintf("5) Test 2 producers, 1 consumer, consumer higher priority\r\n");
	kprintf("6) Test 1 producer, 2 consumer, same priority\r\n");
	kprintf("7) Test 1 producer, 2 consumer, consumer higher priority\r\n");
	kprintf("8) Test 1 producer, 2 consumer, producer higher priority\r\n");
	kprintf("9) Test many producers and many consumers with various priority\r\n");

	kprintf("===TEST BEGIN===\r\n");
	mutexRelease();
	// TODO: Test your operating system!

	c = kgetc();
	switch(c)
	{
	case '0':
		// TODO:
		// Initialize bbuff, and create producer and consumer processes

		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		break;
		
	case '1':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)producer, INITSTK, 50, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		break;

	case '2':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 50, "Consumer", 1, &bbuff),0);
		break;
		
	case '3':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		break;
		
	case '4':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)producer, INITSTK, 50, "Producer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		break;

	case '5':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 50, "Consumer", 1, &bbuff),0);
		break;
		
	case '6':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		break;
		
	case '7':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)consumer, INITSTK, 50, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 1, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		break;

	case '8':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 50, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		break;
		
	case '9':
		bbuff.bufferhead = 0;
		bbuff.buffertail = 0;
		bbuff.empty = semcreate(0);
		bbuff.full = semcreate(BUFFER_SIZE);
		bbuff.mutex= semcreate(1);
		ready(create((void *)consumer, INITSTK, 1, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 50, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 5, "Consumer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 20, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 25, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 30, "Consumer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 14, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 19, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 15, "Consumer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 5, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 40, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 19, "Consumer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 9, "Consumer", 1, &bbuff),0);
		ready(create((void *)producer, INITSTK, 65, "Producer", 1, &bbuff),0);
		ready(create((void *)consumer, INITSTK, 100, "Consumer", 1, &bbuff),0);
		break;

	default:
		break;
	}

	while (numproc > 2) { resched(); }
	kprintf("\r\n===TEST END===\r\n");
	return;
}
