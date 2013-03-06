/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 * Kaleb Breault
 * and
 * Jason Laqua
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <device.h>
#include <stdio.h>
#include <uart.h>
#include <proc.h>
#include <queue.h>

void printqueue(queue q);
void infiniteProcess(void);

void bigargs(int a, int b, int c, int d, int e, int f)
{
	kprintf(" bigargs(%d, %d, %d, %d, %d, %d) == %d\r\n",
			a, b, c, d, e, f, a+b+c+d+e+f);
}
 
void printpid(int times)
{
	int i = 0;
	   
	enable();
	for (i = 0; i < times; i++)
	{
		kprintf("This is process %d\r\n", currpid);
		printqueue(readylist);
		resched();
	}
}

void infiniteProcess(void)
{
	enable();
	while(1)
	{
		kprintf("This is inf process %d\r\n", currpid);
	}
}

void printqueue(queue q)
{
	int head, tail, index;
	head = queuehead(q);
	tail = queuetail(q);
				//dot notation NOT ARROW!!
	index = queuetab[head].next;
	kprintf("*queue contents: \r\n");
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
	kprintf("END OF QUEUE\r\n");
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	int c;

	kprintf("0) Test priority scheduling\r\n");
	kprintf("1) Test priority FCFS\r\n");
	kprintf("A) Test aging\r\n");
	kprintf("P) Test preemption\r\n");
	kprintf("===TEST BEGIN===\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch(c)
	{
	case	'0':
		ready(create((void *)printpid, INITSTK, 2, "PRINTER-A", 1, 5), 0);
		ready(create((void *)printpid, INITSTK, 5, "PRINTER-B", 1, 5), 0);
		ready(create((void *)printpid, INITSTK, 10, "PRINTER-C", 1, 5), 0);
		ready(create((void *)printpid, INITSTK, 5, "PRINTER-D", 1, 5), 0);

		ready(create((void *)bigargs, INITSTK, 20, "BIGARGS", 6, 10, 20, 30, 40, 50, 60), 0);
		break;

	case	'1':
		ready(create((void *)printpid, INITSTK, 5, "PRINTER-A", 1, 5), 0);
        ready(create((void *)printpid, INITSTK, 5, "PRINTER-B", 1, 5), 0);
        ready(create((void *)printpid, INITSTK, 5, "PRINTER-C", 1, 5), 0);
        ready(create((void *)printpid, INITSTK, 5, "PRINTER-D", 1, 5), 0);
		break;

	case 'A':
		ready(create((void *)printpid, INITSTK, 8, "PRINTER-a", 1, 5), 0);
		ready(create((void *)printpid, INITSTK, 9, "PRINTER-b", 1, 5), 0);
		ready(create((void *)printpid, INITSTK, 10, "PRINTER-c", 1, 5), 0);
		ready(create((void *)printpid, INITSTK, 13, "PRINTER-d", 1, 5), 0);
		break;
	
	case 'P':
		ready(create((void *)infiniteProcess, INITSTK, 1, "Inf1", 0, NULL), 0);
		ready(create((void *)infiniteProcess, INITSTK, 1, "Inf2", 0, NULL), 0);
		break;
		
	default:
		break;
	}

	while (numproc > 2) { resched(); }
	kprintf("\r\n===TEST END===\r\n");
	return;
}
