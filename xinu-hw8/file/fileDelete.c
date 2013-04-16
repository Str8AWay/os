/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/* Kaleb Breault                                                   */
/* and                                                             */
/* Jason Laqua                                                     */
/*                                                                 */

#include <kernel.h>
#include <memory.h>
#include <file.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.

    if ((NULL == supertab) || (isbadfd(fd)))
    {
        return SYSERR;
    }
	if (wait(supertab->sb_dirlock) != OK)
	{
		return SYSERR;
	}
	sbFreeBlock(supertab,supertab->sb_dirlst->db_fnodes[fd].fn_blocknum);

	supertab->sb_dirlst->db_fnodes[fd].fn_state = FILE_FREE;
	seek(DISK0, supertab->sb_dirlst->db_blocknum);
    syscall result = write(DISK0, supertab->sb_dirlst, sizeof(struct dirblock));
    
    signal(supertab->sb_dirlock);

    if (SYSERR == result)
    {
        return SYSERR;
    }
    return OK;
}
