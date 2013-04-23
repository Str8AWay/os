/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/* Kaleb Breault                                                   */
/* and                                                             */
/* Jason Laqua                                                     */
/*                                                                 */

#include <kernel.h>
#include <device.h>
#include <memory.h>
#include <disk.h>
#include <file.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */
devcall sbFreeBlock(struct superblock *psuper, int block)
{
    // TODO: Add the block back into the filesystem's list of
    //  free blocks.  Use the superblock's locks to guarantee
    //  mutually exclusive access to the free list, and write
    //  the changed free list segment(s) back to disk.
    
    if ((block < 0) || (block > DISKBLOCKTOTAL))
    	return SYSERR;

	struct freeblock *next_ptr;
	struct dirblock *dirlst;
    struct dentry *phw;
    int diskfd;
    if (NULL == psuper)
    {
        return SYSERR;
    }
    phw = psuper->sb_disk;
    if (NULL == phw)
    {
        return SYSERR;
    }
    diskfd = phw - devtab;
    // Updating the free list must be mutually exclusive.
    wait(psuper->sb_freelock);
    
    struct freeblock *freeblk = psuper->sb_freelst;
    
    if (freeblk == NULL)
    {
    	printf("Freeblk is NULL\n");
    	freeblk = psuper->sb_freelst = malloc(sizeof(struct freeblock));
    	freeblk->fr_blocknum = block;
        freeblk->fr_count = 0;
        freeblk->fr_next = NULL;
        seek(diskfd, block);
        if (SYSERR == write(diskfd, freeblk, sizeof(struct freeblock)))
    	{
 	   		return SYSERR;
    	}
    	next_ptr = freeblk;
    	dirlst = psuper->sb_dirlst;
    	psuper->sb_freelst = block;
    	psuper->sb_dirlst = psuper->sb_dirlst->db_blocknum;
    	seek(diskfd, psuper->sb_blocknum);
    	if (SYSERR == write(diskfd, psuper, sizeof(struct superblock)))
    	{
 	   		return SYSERR;
    	}
    	psuper->sb_freelst = next_ptr;
    	psuper->sb_dirlst = dirlst;
    	signal(psuper->sb_freelock);
    	return OK;
    }
    while(freeblk->fr_next != NULL)
    {
    	if (freeblk->fr_blocknum == block)
    	{
			signal(psuper->sb_freelock);
			return SYSERR;
		}
    	int i=0;
    	for (i=0;i<freeblk->fr_count;i++)
    	{
			if (freeblk->fr_free[i]== block)
			{
				signal(psuper->sb_freelock);
				return SYSERR;
			}
    	}
    	freeblk = freeblk->fr_next;
    }
    if (freeblk->fr_blocknum == block)
    {
			signal(psuper->sb_freelock);
			return SYSERR;
	}
	int i;
    for (i=0;i<freeblk->fr_count;i++)
    {
		if (freeblk->fr_free[i]== block)
		{
			signal(psuper->sb_freelock);
			return SYSERR;
		}
    }
    
    if (freeblk->fr_count >= FREEBLOCKMAX)
    {
	   	struct freeblock *newblk = freeblk->fr_next = malloc(sizeof(struct freeblock));
    	newblk->fr_blocknum = block;
    	newblk->fr_count = 0;
    	newblk->fr_next = NULL;
   		
    	seek(diskfd, newblk->fr_blocknum);
    	if (SYSERR == write(diskfd, newblk, sizeof(struct freeblock)))
    	{
 	   		return SYSERR;
    	}
    }
    else
    {
		freeblk->fr_free[freeblk->fr_count] = block;
   		freeblk->fr_count++;
   	}
    ///////

	next_ptr = freeblk->fr_next;
	if (freeblk->fr_next != NULL)
		freeblk->fr_next = freeblk->fr_next->fr_blocknum;
    seek(diskfd, freeblk->fr_blocknum);
    if (SYSERR == write(diskfd, freeblk, sizeof(struct freeblock)))
    {
 	   return SYSERR;
    }
    freeblk->fr_next = next_ptr;
    ///////
    signal(psuper->sb_freelock);
    return OK;
}
