/* lottery_scheduling.c - lottery_scheduling */

#include <xinu.h>
#include <stdlib.h>

struct lottery_node *lottery_head = NULL;
uint32 total_lottery_tixs = 0;


void	lottery_list_insert(pid32 pid);
bool8	lottery_list_contains(pid32 pid);
void 	lottery_list_remove(pid32 pid);


/*------------------------------------------------------------------------
 *  lottery_scheduling  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
pid32	lottery_scheduling(void)
{	

	struct lottery_node *curr_lottery_node = lottery_head;
	
	if (curr_lottery_node == NULL || total_lottery_tixs == 0) {
		return EMPTY;
	}

	if(curr_lottery_node->next == NULL) {
		return curr_lottery_node->pid;
	}

	uint32 counter = 0;
	uint32 winner = rand() % total_lottery_tixs;

	while(curr_lottery_node) {
		counter = counter + curr_lottery_node->tickets;
		if (counter > winner) break; // found the winner
		curr_lottery_node = curr_lottery_node->next;
	}
	return curr_lottery_node != NULL ? curr_lottery_node->pid : EMPTY;
}


/* 
// counter: used to track if we’ve found the winner yet

int counter = 0;

// winner: call some random number generator to
// get a value >= 0 and <= (totaltickets- 1)

int winner = getrandom(0, totaltickets);

// current: use this to walk through the list of jobs

node_t *current = head;
while (current) {
	counter = counter + current->tickets;
	if (counter > winner)
		break; // found the winner
	current = current->next;
}
// ’current’ is the winner: schedule it..

*/


void	lottery_list_insert(pid32 pid) {

	uint32 tickets;	
	struct lottery_node *node;
	struct lottery_node *curr;
	struct lottery_node *prev;
	intmask	mask;	

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
	tickets = proctab[pid].lottery_tickets;

	if (tickets == 0) {
		lottery_list_remove(pid);
		restore(mask);
		return;
	}

	/* Allocate before deleting an existing entry. */
	node = (struct lottery_node *) getmem(sizeof(struct lottery_node));
	if (node == (struct lottery_node *)SYSERR) {
		panic("Cannot allocate lottery node");
		restore(mask);
		return;
	}
	if (lottery_list_contains(pid)) {
		lottery_list_remove(pid);
	}

	if (tickets > 0xFFFFFFFFU - total_lottery_tixs) {
		panic("Lottery ticket total overflow");
		restore(mask);
		return;
	}
	node->pid = pid;
	node->tickets = tickets;

	prev = NULL;
	curr = lottery_head;



	while(curr!=NULL && 
		(curr->tickets > tickets || 
		(curr->tickets == tickets && curr->pid <pid))) {
			prev = curr;
			curr = curr->next;
		}

		node->next = curr;

		    if (prev == NULL) {
        		lottery_head = node;
    		} else {
        		prev->next = node;
    		}

    	total_lottery_tixs += tickets;
    	proctab[pid].lottery_tickets = tickets;

    	restore(mask);

	}


bool8	lottery_list_contains(pid32 pid) {
	intmask mask = disable();
	struct lottery_node *curr = lottery_head;

	while(curr!=NULL){
		if(curr->pid == pid) {
			restore(mask);
			return TRUE;
		}
		curr = curr->next;
	}
	restore(mask);	
	return FALSE;
	
}


void lottery_list_remove(pid32 pid) {
	intmask mask = disable();
	struct lottery_node *curr = lottery_head;
	struct lottery_node *prev = NULL;

	while(curr!=NULL && curr->pid!=pid) {
		prev = curr;
		curr = curr->next;
	}

	if (curr != NULL) {
		if (prev == NULL) { //first element
			lottery_head = curr->next;
		} else {
			prev->next = curr->next;
		}

		total_lottery_tixs -= curr->tickets;
		freelottery(curr);
	}
	restore(mask);

}
