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

extern void main(int, char *);

int testmain(int argc, char **argv) 
{
	int i = 0;
	kprintf("Hello XINU World!\r\n");
	
	for (i = 0; i < 10; i++)
	{
		kprintf("This is process %d\r\n", currpid);

		/* Uncomment the resched() line for cooperative scheduling. */
		resched();
	}
	return 0;
}

void testbigargs(int a, int b, int c, int d, int e, int f, int g, int h)
{
	kprintf("Testing bigargs...\r\n");
	kprintf("a = 0x%08X\r\n", a);
	kprintf("b = 0x%08X\r\n", b);
	kprintf("c = 0x%08X\r\n", c);
	kprintf("d = 0x%08X\r\n", d);
	kprintf("e = 0x%08X\r\n", e);
	kprintf("f = 0x%08X\r\n", f);
	kprintf("g = 0x%08X\r\n", g);
	kprintf("h = 0x%08X\r\n", h);
}

int fib(int n)
{
	kprintf("n = %d\r\n", n);
	if (n <= 0)
		return 0;
	if (n == 1)
		return 1;
	resched();
	return fib(n-1)+fib(n-2);
}

void printfib(int n)
{
	kprintf("fib(%d) = %d\r\n", n, fib(n));
}

void printpcb(int pid)
{
	pcb *ppcb = NULL;

	/* Using the process ID, access it in the PCB table. */
	ppcb = &proctab[ pid ];
	
	/* Printing PCB */
	kprintf("Process name		  : %s \r\n", ppcb->name);
	
	switch (ppcb->state)
	{
	case PRFREE:
		kprintf("State of the process	  : FREE \r\n");
		break;
	case PRCURR:
		kprintf("State of the process 	  : CURRENT \r\n");
		break;
	case PRSUSP:
		kprintf("State of the process	  : SUSPENDED \r\n");
		break;
	case PRREADY:
		kprintf("State of the process	  : READY \r\n");
		break;
	default:
		kprintf("ERROR: Process state not correctly set!\r\n");
		break;
	}

	/* Print PCB contents and registers */
	kprintf("Base of run time stack    : 0x%08X \r\n", ppcb->stkbase);
	kprintf("Stack pointer of process  : 0x%08X \r\n", ppcb->stkptr);
	kprintf("Stack length of process   : %8u \r\n", ppcb->stklen);
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	int c, pid;

	kprintf("0) Test creation of one process\r\n");
	kprintf("1) Test passing of many args\r\n");
	kprintf("2) Create three processes and run them\r\n");
	kprintf("3) Create 2 testbigargs processes, one maintest and a printfib\r\n");

	kprintf("===TEST BEGIN===");

	// TODO: Test your operating system!

	c = kgetc();
	switch(c)
	{
	case	'0':
		pid = create((void *)testmain, INITSTK, "MAIN1", 2, 0, NULL);
		printpcb(pid);
		break;

	case	'1':
		pid = create((void *)testbigargs, INITSTK, "MAIN1", 8, 
				0x11111111, 0x22222222, 0x33333333, 0x44444444,
				0x55555555, 0x66666666, 0x77777777, 0x88888888);
		printpcb(pid);
		// TODO: print out stack with extra args
		kprintf("\r\nStack:\r\n");
		pcb *ppcb = &proctab[pid];
		ulong *saddr = ppcb->stkptr;
		ulong stkhi = ((ulong)ppcb->stkbase) + ppcb->stklen;
		int i;
		for (i=0; i < (stkhi - ((ulong)ppcb->stkptr))/sizeof(ulong); i++)
		{
			kprintf("0x%08X:\t0x%08X\r\n", saddr, *saddr);
			saddr++;
		}
		// TODO: 
		ready(pid, 0);
		break;

	case	'2':
		// Create three copies of a process, and let them play.
		ready(create((void *)main, INITSTK, "MAIN1", 2, 0, NULL), 0);
		ready(create((void *)main, INITSTK, "MAIN2", 2, 0, NULL), 0);
		ready(create((void *)main, INITSTK, "MAIN3", 2, 0, NULL), 0);
		while (numproc > 1) resched();
		break;
		
	default:
		// Create different processes
		ready(create((void *)main, INITSTK, "MAIN", 2, 0, NULL), 0);
		ready(create((void *)testbigargs, INITSTK, "Big args1", 8,
					0x11111111, 0x22222222, 0x33333333, 0x44444444,
					0x55555555, 0x66666666, 0x77777777, 0x88888888), 0);
		ready(create((void *)testbigargs, INITSTK, "Big args2", 10,
					0x12345678, 0xABCDEF12, 0x11223344, 0x99999999, 
					0x98765432, 0xFEDCBABC, 0xFF00FF00, 0xAAAAAAAA), 0);
		ready(create((void *)printfib, INITSTK, "fib", 1, 10), 0);
	}

	kprintf("\r\n===TEST END===\r\n");
	return;
}
