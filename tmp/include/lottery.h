/* lottery.h - definitions for the lottery scheduling queue */

#ifndef LOTTERY_H
#define LOTTERY_H

struct lottery_node {
	pid32 pid;
	uint32 tickets;
	struct lottery_node *next;
};

extern struct lottery_node *lottery_head;
extern uint32 total_lottery_tixs;

#endif