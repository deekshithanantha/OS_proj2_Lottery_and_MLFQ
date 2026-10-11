/* mlfq_scheduling.c - mlfq_scheduling */

#include <xinu.h>
#include <stdlib.h>

qid16 mlfq_list[UPRIORITY_QUEUES];

void mlfq_list_booster(void);
/*------------------------------------------------------------------------
 *  MLFQ  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */

pid32	mlfq_scheduler(void)
{	
	uint32 i;

	for(i = 0; i < UPRIORITY_QUEUES; i++) {
		qid16 q = mlfq_list[i];
		if(queuetab[queuehead(q)].qnext != queuetail(q)) {
			return getfirst(q);
		} 
	}
	return EMPTY;

	
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


void mlfq_list_booster() {

	intmask	mask;	
	uint32 level, i;
	pid32 pid;

	// a process must be pushed into this only if it is 
	// a user process and in ready state
	mask = disable();

	boost_time_monitor = 0;
	// For process in ready list
	// move them to first line

	for ( level = 1; level < UPRIORITY_QUEUES; level++)
	{
		while(nonempty(mlfq_list[level])) {
			
			pid = dequeue(mlfq_list[level]);
			pid = enqueue(pid,mlfq_list[0]);

		}
	}
	// for all the process reset time allotment and level

	for (i = 0; i < NPROC; i++) {

		if(proctab[i].userprocess && proctab[i].prstate != PR_FREE) {
			/* Count every boost, including jobs already at the top level. */
			proctab[i].upgrades++;
			proctab[i].mlfq_level = 0;
			proctab[i].allotment_used = 0;


		}
	}
	restore(mask);

	}
