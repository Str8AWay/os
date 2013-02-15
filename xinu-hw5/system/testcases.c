/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 *
 * and
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <device.h>
#include <stdio.h>
#include <uart.h>
#include <proc.h>


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
		resched();
	}
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	int c;

	kprintf("0) Test priority scheduling\r\n");

	kprintf("===TEST BEGIN===");

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

	default:
		break;
	}

	while (numproc > 2) { resched(); }
	kprintf("\r\n===TEST END===\r\n");
	return;
}
