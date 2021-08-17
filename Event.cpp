/*
 * Event.cpp
 *
 *  Created on: Aug 16, 2021
 *      Author: OS1
 */

#include "Event.h"
#include "system.h"
#include "kernelev.h"

Event::Event(IVTNo ivtNo) {
	lock
	myImpl = new KernelEv(ivtNo);
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

void Event::wait() {
	if(myImpl)
		myImpl->wait();
}

void Event::signal() {
	if(myImpl)
		myImpl->signal();
}
