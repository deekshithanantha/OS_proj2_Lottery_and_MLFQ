/* mlfq_scheduling.c - mlfq_scheduling */

#include <xinu.h>
#include <stdlib.h>

qid16 mlfq_list[UPRIORITY_QUEUES];


/*------------------------------------------------------------------------
 *  MLFQ  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */

pid32	mlfq_scheduler(void)
{	

	
}


void mlfq_list_insert(pid32 pid) {

	intmask	mask;	
	uint32 level;
	// a process must be pushed into this only if it is 
	// a user process and in ready state
	mask = disable();
	
	if( isbadpid(pid) || pid == NULLPROC
	    || proctab[pid].userprocess==FALSE 
		|| proctab[pid].prstate != PR_READY)
		{
			restore(mask);
			return;
		}

	level = proctab[pid].mlfq_level;

	if(level >= UPRIORITY_QUEUES ) {
		restore(mask);
		return;
	}
	
	enqueue(pid,mlfq_list[level]);
	restore(mask);

	}
