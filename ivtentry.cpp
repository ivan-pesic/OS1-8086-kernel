/*
 * ivtentry.cpp
 *
 *  Created on: Aug 16, 2021
 *      Author: OS1
 */

#include "ivtentry.h"
#include "system.h"
#include "kernelev.h"
#include "DOS.H"

IVTEntry::IVTEntry(IVTNo ivt_number, p_interrupt new_interrupt_routine) {
	lock
	this->ivt_number = ivt_number;
	this->new_interrupt_routine = new_interrupt_routine;
	this->kernel_event = 0;
	this->flag = 0;
	System::entries[ivt_number] = this;

	disable_interrupts
	old_interrupt_routine = getvect(ivt_number);
	setvect(ivt_number, new_interrupt_routine);
	enable_interrupts

	unlock
}

IVTEntry::~IVTEntry() {
	/*
	 * For some strange reason, I have to call old routine in this
	 * destructor in order to provide well functioning of the system
	 * when escape is pressed more than once.
	 *
	 * Flag is used to register that event has been set to IVTEntry
	 * instance. This is not necessary for proper functioning of the system,
	 * but at least it's not writing some strange French 'e symbol when
	 * public test starts without parameters.
	 *
	 * Comment if.
	 */
	if(flag)
		(*old_interrupt_routine)();
	disable_interrupts
	setvect(ivt_number, old_interrupt_routine);
	kernel_event = 0;
	System::entries[ivt_number] = 0;
	enable_interrupts
}

void IVTEntry::signal() {
	if(kernel_event)
		kernel_event->signal();
}

void IVTEntry::call_old_routine() {
	lock
	(*old_interrupt_routine)();
	unlock
}
