/* print_ready_list.c - print_ready_list */
#include <xinu.h>
/*------------------------------------------------------------------------
- print_ready_list  -
 *------------------------------------------------------------------------
 */
syscall	print_ready_list()
{
	qid16	curr;	
	curr = firstid(readylist);		
	while(curr != queuetail(readylist)) {
		kprintf("PID:%d, Priority:%d\n",curr,queuetab[curr].qkey);
		curr = queuetab[curr].qnext;
	}
	return OK;
}