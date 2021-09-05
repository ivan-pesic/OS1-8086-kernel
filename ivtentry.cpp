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
	lock
	if(!list_of_events.empty()) {
		list_of_events.to_front();
		while(list_of_events.has_current()) {
			list_elem* elem = (list_elem*)(list_of_events.get_current_data());
			elem->event->signal();
			list_of_events.to_next();
		}
	}
	unlock
}

void IVTEntry::call_old_routine() {
	lock
	(*old_interrupt_routine)();
	unlock
}

void IVTEntry::insert_in_list(KernelEv* kernel_event, int priority) {
	lock
	if(list_of_events.empty()) {
		list_of_events.push_back(new list_elem(kernel_event, priority));
		unlock
		return;
	}
	list_of_events.to_front();
	while(list_of_events.has_current()) {
		if(priority < ((list_elem*)(list_of_events.get_current_data()))->priority) {
			list_of_events.insert_before_current(new list_elem(kernel_event, priority));
			break;
		}
		list_of_events.to_next();
	}
	list_of_events.push_back(new list_elem(kernel_event, priority));
	unlock
}
void IVTEntry::remove_from_list(KernelEv* kernel_event) {
	lock
	list_of_events.to_front();
	while(list_of_events.has_current()) {
		if(kernel_event == ((list_elem*)(list_of_events.get_current_data()))->event) {
			list_of_events.remove_element((list_of_events.get_current_data()));
			break;
		}
		list_of_events.to_next();
	}
	unlock
}
