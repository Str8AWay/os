/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 / COEN 4820 Assignment 4
 * Modified by
 * Kaleb Breault
 * and 
 * Jason Laqua
 */
/* Embedded XINU, Copyright (C) 2010.  All rights reserved. */
  
#include <kernel.h>
#include <proc.h>
#include <stdarg.h>
#include <mips.h>
#include <string.h>

local newpid(void);
void userret(void);
void *getstk(ulong);

/**
 * Create a process to start running a procedure.
 * @param procaddr procedure address
 * @param ssize stack stack size in words
 * @param name name of the process, used for debugging
 * @param nargs, number of arguments that follow
 * @return the new process' process id
 */
syscall	create(void *procaddr, ulong ssize,
			   char *name, ulong nargs, ...)
{
	ulong   *saddr;     /* stack address                */
	ulong   *savargs;   /* pointer to arg saving region */
	ulong   pid;        /* stores new process id        */
	pcb     *ppcb;      /* pointer to proc control blk  */
	ulong   i;
	va_list	ap;         /* points to list of var args   */
	ulong   pads;       /* padding entries in record.   */
	void	INITRET(void);

	if (ssize < MINSTK) ssize = MINSTK;
	ssize = (ulong)(ssize + 3) & 0xFFFFFFFC;  
                                    /* round up to even boundary    */
	saddr = (ulong *)getstk(ssize); /* allocate new stack and pid   */
	pid   = newpid();
	                                /* a little error checking      */ 
	if ( (((ulong *)SYSERR) == saddr) || (SYSERR == pid) )
	{ return SYSERR; }

	numproc++;
	ppcb = &proctab[ pid ];
                                    /* setup PCB entry for new proc */
	ppcb->state = PRSUSP;

	// TODO: Setup PCB entry for new process.
	strncpy(ppcb->name, name, strlen(name));
	ppcb->stklen = ssize;
	ppcb->stkbase = saddr - ssize/sizeof(ulong);
	//kprintf("name: %s\r\n",ppcb->name);
	//kprintf("&procaddr: 0x%x\r\n", procaddr);
	//kprintf("ppcb->stklen: %d\r\n", ppcb->stklen);
	//kprintf("ppcb->stkbase: 0x%x\r\n", ppcb->stkbase);

	/* Initialize stack with accounting block. */
	*saddr   = STACKMAGIC;
	*--saddr = pid;
	*--saddr = ppcb->stklen;
	*--saddr = (ulong)ppcb->stkbase;
	
	if (0 == nargs)
	{ pads = 4; } 
	else if (0 == (nargs % 4))           /* Must pad record size to      */
	{ pads = 0; }                        /*  multiple of native memory   */
	else                                 /*  transfer size.              */
	{ pads = 4 - (nargs % 4); }

	for (i = 0; i < pads; i++) { *--saddr = 0; }
                                    /* Reserve space for all args.  */
	for (i = nargs ; i > 0 ; i--) { *--saddr = 0; }
	savargs = saddr;          

	// TODO: Initialize process context.
	
	for (i=0;i < CONTEXT_WORDS;i++)
	{
		saddr--;
		*saddr = 0;
	}
	
	saddr[15] = (ulong)procaddr;
	saddr[14] = (ulong)&userret;
	//kprintf("saddr[15] address: 0x%x, value: 0x%x\r\n", saddr+15, *(saddr+15));

	
	// TODO:  Place arguments into activation record.
	//        See K&R 7.3 for example using va_start, va_arg and
	//        va_end macros for variable argument functions.
	
	va_start(ap, nargs);
	for (i=0; i < nargs;i++)
	{
			savargs[i]=va_arg(ap,int);
	}
	va_end(ap);

	
	ppcb->stkptr = saddr;
	//kprintf("stkptr = 0x%x\r\n", saddr);

	return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
local newpid(void)
{
	int	pid;			/* process id to return     */
	static int nextpid = 0;

	for (pid = 0 ; pid < NPROC ; pid++) 
	{	                /* check all NPROC slots    */
		nextpid = (nextpid + 1) % NPROC;
	    if (PRFREE == proctab[nextpid].state)
		{ return nextpid; }
	}
	return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
	kill( currpid );
}
