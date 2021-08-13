/*
 * kersem.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: OS1
 */

#include "kersem.h"
#include "system.h"

KernelSem::KernelSem(int initial_value) {
	lock
	value = initial_value;
	System::all_semaphores.push_back(this);
	unlock
}

KernelSem::~KernelSem() {
	lock
	System::all_semaphores.remove_element(this);
	while(!waiting.empty()) {
		waiting_data* wd = (waiting_data*)(waiting.pop_front());
		delete wd;
	}
	unlock
}

int KernelSem::wait(Time max_time_to_wait) {
	lock
	int return_value = 1;
	if(--value < 0) {
		PCB* to_block = (PCB*)(System::running);
		to_block->block();
		if(max_time_to_wait == 0) {
			blocked.push_back(to_block);
		}
		else {
			waiting.push_back(new waiting_data(to_block, this, max_time_to_wait));
		}
		unlock
		dispatch();
		lock
		if(System::running->unblocked_by_time) {
			return_value = 0;
			System::running->unblocked_by_time = 0;
		}
	}
	unlock
	return return_value;
}

void KernelSem::signal() {
	lock
	if(value++ < 0) {
		PCB* potentially_ublocked = 0;
		waiting_data* potential_wd = (waiting_data*)(waiting.pop_back());
		if(potential_wd) {
			potentially_ublocked = potential_wd->pcb;
			delete potential_wd;
		}
		else {
			potentially_ublocked = (PCB*)(blocked.pop_front());
		}
		if(potentially_ublocked)
			potentially_ublocked->unblock();
	}
	unlock
}

int KernelSem::val() const {
	return value;
}

