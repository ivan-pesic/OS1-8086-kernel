#include "Thread.h"
#include "system.h"
#include "pcb.h"

Thread::Thread(StackSize stackSize, Time timeSlice) {
	lock
	myPCB = new PCB(this, stackSize, timeSlice);
	unlock
}

Thread::~Thread() {
	//waitToComplete();
	lock
	//System::all_PCBs.remove_element(myPCB);
	if(myPCB != 0)
		delete myPCB;
	myPCB = 0;
	unlock
}

void Thread::start() {
	if(myPCB == 0)
		return;
	myPCB->start();
}

void Thread::waitToComplete() {
	if(myPCB == 0)
		return;
	myPCB->wait_to_complete();
}

ID Thread::getId() {
	if(myPCB == 0)
		return -1;
	return myPCB->get_id();
}

ID Thread::getRunningId() {
	return PCB::get_running_id();
}

Thread* Thread::getThreadById(ID id) {
	return PCB::get_thread_by_id(id);
}

// sinhrona promena konteksta
void dispatch(){
	disable_interrupts
	System::context_switch_on_demand = 1;
	System::timer();
	enable_interrupts
}
