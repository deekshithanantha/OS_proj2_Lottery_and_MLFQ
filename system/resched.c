/* resched.c - resched, resched_cntl */

#include <xinu.h>
#define DEBUG_CTXSW

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	pid32 old_pid = currpid;	/* Needed even when logging is disabled */

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		/* Requeue before choosing a system process or lottery winner. */

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
		lottery_list_insert(currpid);
	}

	/* Select a non-null system first; otherwise try the lottery. */

	currpid = dequeue_system_process(readylist);

	if(currpid == EMPTY) {

		currpid = lottery_scheduling();
		if (currpid == EMPTY) {
			currpid = NULLPROC;
		}
		getitem(currpid); /* Also removes the lottery node, if present. */
		queuetab[currpid].qprev = EMPTY;
		queuetab[currpid].qnext = EMPTY;
	}

	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	if (old_pid == currpid) {
		return; /* No actual context switch. */
	}
	proctab[currpid].num_ctxsw++;
	#ifdef DEBUG_CTXSW
		if(old_pid!=currpid) {
			kprintf("ctxsw::%d-%d\n",old_pid,currpid);
		}
	#endif
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
