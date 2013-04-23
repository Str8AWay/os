/**
 * @file     xsh_test.c
 * @provides xsh_test
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (test) is testing hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_test(int nargs, char *args[])
{
	int allocTable[254];
    //TODO: Test your O/S.
    if (nargs > 1)
    {
    	if (0 == strcmp("allocall",args[1]))
    	{
    		int i;
	    	for (i = 0; i < 254; i++)
	    	{
		    	allocTable[i] = sbGetBlock(supertab);
		    }
		    xsh_diskstat(0,NULL);
		    for (i = 0; i < 254; i++)
		    {
		    	sbFreeBlock(supertab,allocTable[i]);
		    }
		}
		else if (0 == strcmp("alloc1dealloc1",args[1]))
	    {
	    	xsh_diskstat(0,NULL);	
	    	int foo = sbGetBlock(supertab);
	    	xsh_diskstat(0,NULL);
	    	sbFreeBlock(supertab,foo);
	    	xsh_diskstat(0,NULL);
	    }
	    else if (0 == strcmp("cycle",args[1]))
	    {
	    	int i;
	    	for (i = 0; i < 1000; i++)
	    	{
	    		int bar = sbGetBlock(supertab);
	    		xsh_diskstat(0,NULL);
	    		sbFreeBlock(supertab,bar);
	    		xsh_diskstat(0,NULL);
	    	}
	    }
	    else if (0 == strcmp("freed",args[1]))
	    {
	    	int baz = sbGetBlock(supertab);
	    	xsh_diskstat(0,NULL);
    		sbFreeBlock(supertab,baz);
    		xsh_diskstat(0,NULL);
    		int error = sbFreeBlock(supertab,baz);
    		printf("error is %d\n",error);
    	}	
	   	else
		   	printf("Usage: test <command>\n\tallocall\n\\talloc1dealloc1\n\tcycle\n\tfreed\n");
    } else
    	printf("Usage: test <command>\n\tallocall\n\talloc1dealloc1\n\tcycle\n\tfreed\n");

	
    
    return OK;
}
