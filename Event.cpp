/*
 * Event.cpp
 *
 *  Created on: Aug 16, 2021
 *      Author: OS1
 */

#include "Event.h"
#include "system.h"
#include "kernelev.h"
#include "pcb.h"
Event::Event(IVTNo ivtNo, int priority) {
	lock
	myImpl = new KernelEv(ivtNo, priority);
	unlock
}

Event::~Event() {
	lock
	if(myImpl) {
		delete myImpl;
		myImpl = 0;
	}
	unlock
}
#include "STDIO.H"
void Event::wait() {
	if(myImpl) {
		printf("\nBLOCKED - THREAD ID = %d", System::running->get_id());
 		myImpl->wait();
	}

}

void Event::signal() {
	if(myImpl)
		myImpl->signal();
}
