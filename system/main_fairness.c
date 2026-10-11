/* Copy this file to system/main.c to run the fairness test. */
#include <xinu.h>
#include <stdio.h>

#define FAIRNESS_TRIALS 5

void timed_execution(uint32 runtime)
{
	volatile struct procent *prptr = &proctab[currpid];

	while (prptr->runtime < runtime)
		;
}

int main() {
	pid32 prA, prB;
	uint32 runtimes[] = {10, 20, 50, 100, 250, 500, 1000, 2000, 4000};
	uint32 i, trial;

	kprintf("\nFAIRNESS_BEGIN\n");

	for (i = 0; i < sizeof(runtimes) / sizeof(runtimes[0]); i++) {
		kprintf("\n=== TESTCASE %d:: fairness - runtime %d ms =================\n",
		        (int) i + 1, runtimes[i]);

		for (trial = 1; trial <= FAIRNESS_TRIALS; trial++) {
			kprintf("\nTrial %d\n", trial);

			prA = create_user_process(timed_execution, 1024,
			                          "process A", 1, runtimes[i]);
			prB = create_user_process(timed_execution, 1024,
			                          "process B", 1, runtimes[i]);
			if (prA == SYSERR || prB == SYSERR) {
				kprintf("FAIRNESS_ERROR: process creation failed\n");
				return SYSERR;
			}

			set_tickets(prA, 50);
			set_tickets(prB, 50);

			resume(prA);
			resume(prB);

			receive();
			receive();

			sleepms(50); // wait for user processes to terminate

			kprintf("process %d:: runtime=%d, turnaround time=%d, ctx=%d\n",
			        prA, proctab[prA].runtime, proctab[prA].turnaroundtime,
			        proctab[prA].num_ctxsw);
			kprintf("process %d:: runtime=%d, turnaround time=%d, ctx=%d\n",
			        prB, proctab[prB].runtime, proctab[prB].turnaroundtime,
			        proctab[prB].num_ctxsw);

			/* The plotting script calculates F from these turnaround times. */
			kprintf("FAIRNESS_DATA,%u,%u,%u,%u\n", runtimes[i], trial,
			        proctab[prA].turnaroundtime, proctab[prB].turnaroundtime);
		}

		kprintf("================================================================\n\n");
	}

	kprintf("FAIRNESS_END\n");
	return OK;
}
