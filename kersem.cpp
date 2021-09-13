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
	current_operation = NOP;
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
	//cout << "SEM LIST SIZE: " << System::all_semaphores.size_of_list() << endl;
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
		if(temp_wd->time_to_wait > 0) {
			//cout << "TTW: " << temp_wd->time_to_wait << endl;
			temp_wd->time_to_wait--;
		}


		while(!waiting.empty() && ((waiting_data*)(waiting.get_current_data()))->time_to_wait == 0) {
			temp_wd = (waiting_data*)(waiting.pop_front());
			temp_wd->semaphore->increment();
			temp_wd->pcb->unblocked_by_time = 1;
			temp_wd->pcb->unblock();
			delete temp_wd;
			waiting.to_front();
		}
	}
	/*waiting.to_front();
	cout << endl;
	while(waiting.has_current()) {
		waiting_data* tmp = (waiting_data*)waiting.get_current_data();
		cout << tmp->time_to_wait << " ";
		waiting.to_next();
	}*/
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
			//cout << "max_time_to_wait: " << max_time_to_wait << endl;
			sem_lock
			insert_waiting(new waiting_data(to_block, this, max_time_to_wait));
			//waiting.print_list();
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

	/*waiting.to_front();
	cout << endl;
	while(waiting.has_current()) {
		waiting_data* tmp = (waiting_data*)waiting.get_current_data();
		cout << tmp->time_to_wait << " ";
		waiting.to_next();
	}*/

	sem_unlock
	unlock
}

void KernelSem::increment() {
	lock
	value++;
	unlock
}


void KernelSem::function1(char c) {
	lock
	switch(c) {
	case 'r': {
		switch(current_operation) {
		case NOP: {
			current_operation = RD;
			cout << "Thread ID: " << Thread::getRunningId() << " passed f1 wRD cNOP" << endl;
			holders.push_back((PCB*)System::running);
			break;
		}
		case RD: {
			cout << "Thread ID: " << Thread::getRunningId() << " passed f1 wRD cRD" << endl;
			holders.push_back((PCB*)System::running);
			break;
		}
		case WR: {
			cout << "Thread ID: " << Thread::getRunningId() << " blocked f1 wRD cWR" << endl;
			requests.push_back(new holder_struct((PCB*)System::running, RD));
			System::running->block();
			unlock
			dispatch();
			return;
		}
		}
		break;
	}
	case 'w': {
		switch(current_operation) {
		case NOP: {
			current_operation = WR;
			cout << "Thread ID: " << Thread::getRunningId() << " passed f1 wWR cNOP" << endl;
			holders.push_back((PCB*)System::running);
			break;
		}
		case RD: {
			requests.push_back(new holder_struct((PCB*)System::running, WR));
			cout << "Thread ID: " << Thread::getRunningId() << " blocked f1 wWR cRD" << endl;
			System::running->block();
			unlock
			dispatch();
			return;
		}
		case WR: {
			requests.push_back(new holder_struct((PCB*)System::running, WR));
			cout << "Thread ID: " << Thread::getRunningId() << " blocked f1 wWR cWR" << endl;
			System::running->block();
			unlock
			dispatch();
			return;
		}
		}
		break;
	}
	}
	unlock
}
#include "intLock.h"

void KernelSem::function2() {
	lock
	intLock
	//cout << "f2 called by: " << System::running->pcb_id << endl;
	intUnlock
	if(!holders.remove_element((PCB*)System::running))
		cout << "ERRORCINA" << endl;
	if(holders.empty()) {
		intLock
		//cout << "holders empty" << endl;
		intUnlock
		if(requests.empty()) {
			current_operation = NOP;
		}
		else {
			holder_struct* tmp = (holder_struct*)requests.pop_front();
			if(tmp->op == WR) {
				current_operation = WR;
				holders.push_back(tmp->holder);
				tmp->holder->unblock();
				cout << "Thread ID: " << tmp->holder->pcb_id << " passed f2 WR" << endl;
				delete tmp;
			}
			else {
				current_operation = RD;
				holders.push_back(tmp->holder);
				tmp->holder->unblock();
				cout << "Thread ID: " << tmp->holder->pcb_id << " passed f2 RD" << endl;
				requests.to_front();
				while(requests.has_current()) {
					tmp = (holder_struct*)requests.get_current_data();
					if(tmp->op == RD) {
						holders.push_back(tmp->holder);
						tmp->holder->unblock();
						cout << "Thread ID: " << tmp->holder->pcb_id << " passed f2 RD" << endl;
						requests.to_next();
						requests.remove_element(tmp);
						delete tmp;
					}
				}
			}
		}

	}
	else {
		intLock
		//cout << holders.size_of_list() << endl;
		intUnlock
	}
	unlock
}
