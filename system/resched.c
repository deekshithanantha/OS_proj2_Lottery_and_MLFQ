/* resched.c - resched, resched_cntl */

#include <xinu.h>
//#define DEBUG_CTXSW

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	pid32 old_pid = currpid;
	bool8 boost_due;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	boost_due = (boost_time_monitor >= PRIORITY_BOOST_PERIOD);
	
	// check the mlfq exit element. userProcess and not deleted
	// check allotment, 
    //   if  allotment =0, reduce level (check it not less than level)                                                        
    //   else deque from the list and enque it again                                                       
    //                                                          
    //                                                          
    //                     
	        
	if(ptold->userprocess && ptold->prstate != PR_FREE) {

		uint32 level = ptold->mlfq_level;
		uint32 allotment_allowed = (1U << level) * TIME_ALLOTMENT;
		

		if(ptold->allotment_used >= allotment_allowed) {

			if(level +1 < UPRIORITY_QUEUES ) {
				ptold->mlfq_level++;
				ptold->allotment_used = 0;
				ptold->downgrades++;
		}		
	}


		}

		
	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (!boost_due && !ptold->userprocess && currpid != NULLPROC &&
		    ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		if (ptold->userprocess) {
			mlfq_list_insert(currpid);
		} else {
			insert(currpid, readylist, ptold->prprio);
		}
	}

	/* Return the running process to its queue before merging levels.
	 * Blocked processes stay outside the ready queues.
	 */
	if (boost_due) {
		mlfq_list_booster();
	}

	/* Force context switch to highest priority ready process */
	
	if (nonempty(readylist) && firstid(readylist) != NULLPROC) {
	    currpid = dequeue(readylist);                           
	} else {                                                    
	    currpid = mlfq_scheduler();                                                
	                                                            
	    if (currpid == EMPTY) {                                  
	        currpid = dequeue(readylist); /* Select null */     
	    }                                                       
	}                                                                 

	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	if (old_pid == currpid) {
		return;		/* No actual context switch */
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
