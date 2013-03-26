/**
 * @file mutex.c
 * @provides mutexInit, mutexAcquire, mutexRelease
 *
 * Modified by:
 * Jason Laqua
 * and
 * Kaleb Breault
 */

#include <kernel.h>
#include <proc.h>
#include <mutex.h>

static volatile bool waiting[NPROC];
static volatile ulong lock;
/**
 * Initialize bounded-waiting mutex subsystem.
 */
void mutexInit(void)
{
	int i = 0;
	for (i = 0; i < NPROC; i++)
		waiting[i] = FALSE;
	lock = FALSE;
}

/**
 * Acquire lock using MIPS testAndSet.
 * Adapted from textbook Figure 6.8, page 233.
 */
void mutexAcquire(void)
{
	bool key = TRUE;
	// TODO:
	// Implement mutex acquire using MIPS testAndSet().
	// This function will loop until it acquires the lock.
	waiting[currpid] = TRUE;
	while (waiting[currpid] && key)
		key = testAndSet(&lock);
	waiting[currpid] = FALSE;
}

/**
 * Release acquired lock, fairly select next proc to enter critical section.
 * Adapted from textbook Figure 6.8, page 233.
 */
void mutexRelease(void)
{
	int j;
	j = (currpid + 1) % NPROC;
	// TODO:
	// Implement mutex release and select the next process
	//   to admit into its critical section.
	
	while((j != currpid) && !waiting[j])
		j = (j + 1) % NPROC;
		
	if (j == currpid)
		lock = FALSE;
	else
		waiting[j] = FALSE;
}
