/**
 * @file kprintf.c
 * @provides kputc, kgetc, kungetc, kcheckc, kprintf
 *
 * $Id: kprintf.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 * Kaleb Breault
 * and
 * Jason Laqua
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <stdarg.h>
#include <device.h>
#include <stdio.h>
#include <uart.h>

#define UNGETMAX 10  /* Can un-get at most 10 characters. */

static unsigned char ungetArray[UNGETMAX];
unsigned char counter=0; 
/**
 * kputc - perform a synchronous kernel write to the serial console
 * @param *pdev device pointer (unused for the synchronous driver)
 * @param c character to write
 * @return c on success, SYSERR on failure.
 */
syscall kputc(device *pdev, unsigned char c)
{
	volatile struct uart_csreg *pucsr = (struct uart_csreg *)0xB8000300;

	// TODO: Check UART line status.
	//       When the transmitter is empty, send character c.
	
	while (!(UART_LSR_THRE & pucsr->lsr))
	{// wait here for buffer to be empty 
	}
	if (UART_LSR_THRE & pucsr->lsr) {
		pucsr->buffer = c;
		return c;
	}
	
	return SYSERR;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
	volatile struct uart_csreg *pucsr = (struct uart_csreg *)0xB8000300;

	// TODO: Check the unget buffer and the UART for characters.
	if (counter>0)
	{
		return TRUE;
	}
	if ( UART_LSR_DR & pucsr->lsr)
	{
		return TRUE;
	}

	return FALSE;  // false
}

/**
 * kgetc - perform a synchronous kernel read from the serial console,
 *         or from the local buffer if there are "ungotten" characters.
 * @return c on success, SYSERR on failure.
 */
syscall kgetc(void)
{
	volatile struct uart_csreg *pucsr = (struct uart_csreg *)0xB8000300;

	// TODO: First, check the unget buffer for a character.
	//       Otherwise, check UART line status, and
	//       when the receiver has something, get character c.
	if(counter>0)
	{
		counter--;
		return ungetArray[counter];
	}
	while(!kcheckc()){}
	if(kcheckc())
	{
		return pucsr->buffer;
	}

	return SYSERR;
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
	// TODO: Check for room in unget buffer, put the character in.
	if(counter<UNGETMAX)
	{
		ungetArray[counter]=c;
		counter++;
		return c;
	}
	return SYSERR;
}

/**
 * kprintf - kernel printf: formatted, unbuffered output to CONSOLE
 * @param *fmt pointer to string being printed
 * @return OK on success
 */
syscall kprintf(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_doprnt(fmt, ap, (int (*)(int, int))kputc, 0);
	va_end(ap);
	return OK;
}

