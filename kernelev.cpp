/*
 * kernelev.cpp
 *
 *  Created on: Aug 16, 2021
 *      Author: OS1
 */

#include "kernelev.h"
#include "system.h"
#include "pcb.h"
#include "ivtentry.h"

KernelEv::KernelEv(IVTNo ivt_number) {
	lock
	this->ivt_number = ivt_number;
	this->creator_pcb = (PCB*)(System::running);
	this->value = 0;
	System::entries[ivt_number]->kernel_event = this;
	unlock
}

KernelEv::~KernelEv() {
	signal();
	disable_interrupts
	if(creator_pcb->state == PCB::BLOCKED)
		creator_pcb->unblock();
	System::entries[ivt_number]->kernel_event = 0;

	enable_interrupts
}

void KernelEv::wait() {
	lock
	if(System::running != creator_pcb) {
		unlock
		return;
	}
	if(value == 0) {
		creator_pcb->block();
		blocked = creator_pcb;
		unlock
		dispatch();

	}
	else if(value == 1) {
		value = 0;
		unlock
	}

}

void KernelEv::signal() {
	lock
	if(blocked == 0) {
		value = 1;
	}
	else {
		blocked = 0;
		creator_pcb->unblock();
	}
	unlock
}
