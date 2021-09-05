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

KernelEv::KernelEv(IVTNo ivt_number, int priority) {
	lock
	this->ivt_number = ivt_number;
	this->creator_pcb = (PCB*)(System::running);
	this->value = 0;
	this->blocked = 0;
	disable_interrupts
	System::entries[ivt_number]->insert_in_list(this, priority);
	System::entries[ivt_number]->flag = 1;
	enable_interrupts
	unlock
}

KernelEv::~KernelEv() {
	signal();
	disable_interrupts
	System::entries[ivt_number]->remove_from_list(this);
	System::entries[ivt_number]->kernel_event = 0;
	enable_interrupts
	/*
	signal();
	disable_interrupts
	setvect(ivt_number, System::entries[ivt_number]->old_interrupt_routine);
	System::entries[ivt_number]->kernel_event = 0;
	 */
}

/*
 * Idea for implementing wait and signal found in this student's guide
 * written 6 years ago:
 * https://drive.google.com/file/d/1FfSk-shJZ2_ZyA9GGEC8Pp3KEYH94zlT/view
 */

void KernelEv::wait() {
	lock
	if(System::running != creator_pcb) {
		unlock
		return;
	}
	if(value == 0) {
		creator_pcb->block();
		blocked = 1;
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
