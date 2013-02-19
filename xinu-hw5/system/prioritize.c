/**
 * @file prioritize.c
 * @provides 
 * Modified by
 * Kaleb Breault
 * and
 * Jason Laqua
 */
 
#include <proc.h>
#include <queue.h>

void prioritize( short pid, queue q )
{
	struct qentry current = queuetab[queuehead(q)];
	while (current.key> queuetab[pid].key)
	{
		current = queuetab[current.next];
	}
	queuetab[pid].next = queuetab[current.prev].next;
	queuetab[current.prev].next=pid;
	queuetab[pid].prev=current.prev;
	current.prev= pid;
}

