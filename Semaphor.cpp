/*
 * Semaphor.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: OS1
 */

#include "Semaphor.h"
#include "system.h"
#include "kersem.h"

Semaphore::Semaphore(int init) {
	lock
	myImpl = new KernelSem(init);
	unlock
}

Semaphore::~Semaphore() {
	lock
	if(myImpl != 0) {
		delete myImpl;
	}
	myImpl = 0;
	unlock
}

int Semaphore::wait(Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}

void Semaphore::signal() {
	myImpl->signal();
}

int Semaphore::val() const {
	return myImpl->val();
}
/*
void Semaphore::printList() {
	myImpl->printList();
}
 */

void Semaphore::turnOnPriorities() {
	myImpl->turn_on_priorities();
}
