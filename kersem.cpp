/*
 * kersem.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: OS1
 */

#include "kersem.h"
#include "system.h"
#include "list.h"
#include "pcb.h"

int KernelSem::waiting_data_counter = 0;
int KernelSem::live_semaphores = 0;

KernelSem::KernelSem(int initial_value) {
	lock
	value = initial_value;
	sem_lock
	System::all_semaphores.push_back(this);
	sem_unlock
	KernelSem::live_semaphores++;
	unlock
}

KernelSem::~KernelSem() {
	lock
	sem_lock
	System::all_semaphores.remove_element(this);
	//cout << waiting.size_of_list();
	while(!waiting.empty()) {
		waiting_data* wd = (waiting_data*)(waiting.pop_front());
		delete wd;
	}
	//cout << waiting.size_of_list();
	KernelSem::live_semaphores--;
	sem_unlock
	unlock
}

void KernelSem::tick() {
	lock
	System::all_semaphores.to_front();
	while (System::all_semaphores.has_current()) {
		KernelSem* sem = (KernelSem*)(System::all_semaphores.get_current_data());
		if(sem)
			sem->update_list();
		System::all_semaphores.to_next();
	}
	unlock
}

void KernelSem::update_list() {
	lock
	waiting_data* temp_wd;
	if (!waiting.empty()) {
		waiting.to_front();
		temp_wd = (waiting_data*)(waiting.get_current_data());
		if(temp_wd->time_to_wait > 0)
			temp_wd->time_to_wait--;

		while(!waiting.empty() && ((waiting_data*)(waiting.get_current_data()))->time_to_wait == 0) {
			temp_wd = (waiting_data*)(waiting.pop_front());
			temp_wd->semaphore->increment();
			temp_wd->pcb->unblocked_by_time = 1;
			temp_wd->pcb->unblock();
			delete temp_wd;
			waiting.to_front();
		}
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
			//blocked.print_list();
		}
		else {
			sem_lock
			insert_waiting(new waiting_data(to_block, this, max_time_to_wait));
			sem_unlock
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
		if(!waiting.empty()) {
			sem_lock
			//waiting.print_list();
			waiting_data* potential_wd = (waiting_data*)(waiting.pop_back());
			sem_unlock

			if(potential_wd) {
				potentially_ublocked = potential_wd->pcb;
				potentially_ublocked->unblock();
				delete potential_wd;
			}
		}

		else if(!blocked.empty()) {
			potentially_ublocked = (PCB*)(blocked.pop_front());
			potentially_ublocked->unblock();
		}
	}
	unlock
}

int KernelSem::val() const {
	return value;
}

void KernelSem::insert_waiting(waiting_data* wd) {
	lock
	sem_lock

	if(waiting.empty()) {
		waiting.push_back(wd);
		sem_unlock
		unlock
		return;
	}

	Time time = wd->time_to_wait;
	waiting.to_front();
	waiting_data* temp_wd;
	Time temp_time;

	while(waiting.has_current()) {
		temp_wd = (waiting_data*)(waiting.get_current_data());
		temp_time = temp_wd->time_to_wait;
		if(time < temp_time)
			break;
		time -= temp_time;
		waiting.to_next();
	}

	while(waiting.has_current()) {
		temp_wd = (waiting_data*)(waiting.get_current_data());
		temp_time = temp_wd->time_to_wait;
		if(temp_time) {
			temp_time -= time;
			temp_wd->time_to_wait = temp_time;
			break;
		}
		waiting.to_next();
	}
	wd->time_to_wait = time;

	if(waiting.has_current())
		waiting.insert_before_current(wd);
	else
		waiting.push_back(wd);

	sem_unlock
	unlock
}

void KernelSem::increment() {
	lock
	value++;
	unlock
}
