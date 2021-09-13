/*
 * pcb.h
 *
 *  Created on: Aug 11, 2021
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#define min_stack_size 1024
#define max_stack_size 65535

#include "Thread.h"
#include "list.h"
#include "Semaphor.h"

class PCB {
	static ID global_id;
public:
	enum State { NEW, READY, BLOCKED, FINISHED, IDLE};

	PCB();
	PCB(Thread* my_thread, StackSize stack_size = defaultStackSize, Time time_slice = defaultTimeSlice);
	PCB(void (*f) (void*), void* param, StackSize stackSize, Time timeSlice, Thread* my_thread = 0);
	void start();
	void wait_to_complete();
	ID get_id();
	static ID get_running_id();
	static Thread* get_thread_by_id(ID id);

	static void wrapper();
	static void new_wrapper();

	void (*runFunc)(void*);
	void* param;
	void block();
	void unblock();
	static PCB* get_idle_PCB();

	~PCB();

	unsigned sp;
	unsigned ss;
	unsigned bp;
	unsigned* stack;
	unsigned finished;
	Time time_slice;
	StackSize stack_size;
	int unblocked_by_time;
	Thread* my_thread;
	State state;
	ID pcb_id;
	List blocked_PCBs;

	//for fork
#ifdef FORK_IMPL
	List* children_list;
	PCB* parent;
	Semaphore* parent_sem;
	void exit();
	void wait_for_fork_children();
#endif

	// for testing
	static int live_PCBs;
};

#endif /* PCB_H_ */
