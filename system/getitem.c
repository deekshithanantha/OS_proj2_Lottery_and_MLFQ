/* getitem.c - getfirst, getlast, getitem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getfirst  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32	head;

	if (isempty(q)) {
		return EMPTY;
	}

	head = queuehead(q);
	return getitem(queuetab[head].qnext);
}
/*------------------------------------------------------------------------
 *  getfirst_system_process  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst_system_process(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
    pid32 pid;
	
    for (pid = firstid(q); pid != queuetail(q); pid = queuetab[pid].qnext) {

        if (pid != NULLPROC && !proctab[pid].userprocess) {
        
			return getitem(pid);
        }
    }
    return EMPTY;
}

/*------------------------------------------------------------------------
 *  getlast  -  Remove a process from end of queue
 *------------------------------------------------------------------------
 */
pid32	getlast(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32 tail;

	if (isempty(q)) {
		return EMPTY;
	}

	tail = queuetail(q);
	return getitem(queuetab[tail].qprev);
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
	)
{
	pid32	prev, next;

    if (proctab[pid].prstate == PR_READY && proctab[pid].userprocess) {
        lottery_list_remove(pid);
    }

	next = queuetab[pid].qnext;	/* Following node in list	*/
	prev = queuetab[pid].qprev;	/* Previous node in list	*/
	queuetab[prev].qnext = next;
	queuetab[next].qprev = prev;
	return pid;
}
