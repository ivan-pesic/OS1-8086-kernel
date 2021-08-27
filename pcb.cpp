/*
 * pcb.cpp
 *
 *  Created on: Aug 11, 2021
 *      Author: OS1
 */

#include "pcb.h"
#include "system.h"
#include "list.h"
#include "dos.h"
#include "SCHEDULE.H"
#include "Semaphor.h"

ID PCB::global_id = 0;
int PCB::live_PCBs = 0;

PCB::PCB() {
	lock

#ifdef FORK_IMPL
	//this->number_of_children = 0;
	this->children_list = 0;
	this->parent = 0;
	this->parent_sem = 0;
#endif

	pcb_id = ++global_id;
	time_slice = defaultTimeSlice;
	unblocked_by_time = 0;
	lock_cnt = System::lock_counter;
	state = PCB::READY;
	stack = 0;
	stack_size = 0;
	my_thread = 0;
	System::all_PCBs.push_back(this);
	// for testing
	PCB::live_PCBs++;
	//disable_interrupts
	//cout << "PCB constructor: ID: " << pcb_id << ", time_slice: " << this->time_slice << ", stack_size: " << this->stack_size << ", lock_cnt: " << lock_cnt << endl;
	//enable_interrupts
	unlock
}

PCB::PCB(Thread* my_thread, StackSize stack_size, Time time_slice) {
	lock

#ifdef FORK_IMPL
	//this->number_of_children = 0;
	this->children_list = 0;
	this->parent = 0;
	this->parent_sem = 0;
#endif

	this->my_thread = my_thread;
	this->time_slice = time_slice;
	pcb_id = ++global_id;
	unblocked_by_time = 0;
	if(time_slice == 0)
		this->time_slice = -1;
	lock_cnt = 0;

	if(stack_size < min_stack_size) stack_size = min_stack_size;
	if(stack_size > max_stack_size) stack_size = max_stack_size;

	unsigned real_stack_size = stack_size / sizeof(unsigned);
	stack = new unsigned[real_stack_size];

	if(!stack) {
		--global_id;
		unlock
		return;
	}

	// setovanje I flega u pocetnom PSW-u za nit
	stack[real_stack_size - 1] = 0x200;
	// postavljanje adrese funkcije koju ce nit da izvrsava
	stack[real_stack_size - 2] = FP_SEG(PCB::wrapper);
	stack[real_stack_size - 3] = FP_OFF(PCB::wrapper);
	//svi sacuvani registri pri ulasku u interrupt rutinu
	sp = FP_OFF(stack + real_stack_size - 12);
	ss = FP_SEG(stack + real_stack_size - 12);
	bp = sp;

#ifdef FORK_IMPL
	stack[real_stack_size - 12] = 0;
#endif

	this->stack_size = stack_size;
	state = PCB::NEW;

	if(!System::all_PCBs.push_back(this)) {
		--global_id;
		delete[] stack;
		stack = 0;
		unlock
		return;
	}

	// for testing
	PCB::live_PCBs++;
	//disable_interrupts
	//cout << "PCB constructor: ID: " << pcb_id << ", time_slice: " << this->time_slice << ", stack_size: " << this->stack_size << endl;
	//enable_interrupts
	unlock
}

PCB::~PCB() {
	lock
	System::all_PCBs.remove_element(this);
	if(stack)
		delete[] stack;
#ifdef FORK_IMPL
	if(children_list)
		delete children_list;
	if(parent_sem)
		delete parent_sem;

	children_list = 0;
	parent_sem = 0;
#endif
	stack = 0;
	PCB::live_PCBs--;
	unlock
}

void PCB::start() {
	lock
	if(state == PCB::NEW) {
		state = PCB::READY;
		Scheduler::put(this);
	}
	unlock
}

void PCB::wait_to_complete() {
	lock
	if(this == System::running || this->state == PCB::FINISHED || this->state == PCB::NEW || this->state == PCB::IDLE) {
		unlock
		return;
	}
	((PCB*)(System::running))->block();
	blocked_PCBs.push_back((PCB*)System::running);
	unlock
	dispatch();
}

ID PCB::get_id() {
	return pcb_id;
}

ID PCB::get_running_id() {
	return System::running->pcb_id;
}

Thread* PCB::get_thread_by_id(ID id) {
	lock
	for(System::all_PCBs.to_front(); System::all_PCBs.has_current(); System::all_PCBs.to_next()) {
		PCB* tmp = (PCB*)(System::all_PCBs.get_current_data());
		if(tmp->pcb_id == id) {
			unlock
			return tmp->my_thread;
		}
	}
	unlock
	return 0;
}

void PCB::wrapper() {
	System::running->my_thread->run();
#ifndef FORK_IMPL
	lock
	PCB* tmp = (PCB*)(System::running->blocked_PCBs.pop_front());
	while(tmp) {
		tmp->unblock();
		tmp = (PCB*)(System::running->blocked_PCBs.pop_front());
	}
	System::running->state = PCB::FINISHED;
	unlock
	dispatch();
#else
	System::running->exit();
#endif
}

void PCB::block() {
	lock
	state = PCB::BLOCKED;
	unlock
}

void PCB::unblock() {
	lock
	state = PCB::READY;
	Scheduler::put(this);
	unlock
}

PCB* PCB::get_idle_PCB() {
	return System::idle_thread->myPCB;
}

// fork methods
#ifdef FORK_IMPL

void PCB::exit() {
	lock
	PCB* tmp = (PCB*)(blocked_PCBs.pop_front());
	while(tmp) {
		tmp->unblock();
		tmp = (PCB*)(blocked_PCBs.pop_front());
	}

	state = PCB::FINISHED;

	if(children_list) {
		tmp = (PCB*)(children_list->pop_front());
		while(tmp) {
			tmp->parent = 0;
			tmp = (PCB*)(children_list->pop_front());
		}
	}
	if(parent) {
		parent->children_list->remove_element(this);
		if(parent->children_list->empty())
			parent->parent_sem->signal();
	}
	unlock
	dispatch();
}
void PCB::wait_for_fork_children() {
	if(children_list && !children_list->empty()) {
		parent_sem->wait(0);
	}
}
#endif
