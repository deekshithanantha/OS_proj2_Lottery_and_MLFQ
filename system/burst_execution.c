/* burst_execution.c - burst_execution */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  burst_execution  -  bwhich simulates the execution of applications that 
 	alternate execution phases requiring the CPU (CPU bursts), 
	and execution phases not requiring it (CPU inactivity phases
 *------------------------------------------------------------------------
 */
void burst_execution(
	uint32 number_bursts, 
	uint32 burst_duration, 
	uint32 sleep_duration); 
{
	uint32 i;
	uint32 initial_time;

	struct procent *prptr = &proctab[currpid];


	for(i=0; i<number_bursts, i++) {
		initial_time = prptr->runtime;

		while (prptr->runtime-initial_time < burst_duration )
		{
			/* code */
		}

		sleepms(sleep_duration);
		
	}
}
