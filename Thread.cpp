#include "Thread.h"
#include "system.h"
#include "pcb.h"
#include "ASSERT.H"
#include <DOS.H>

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
	assert(System::lock_counter == 0);
	disable_interrupts
	System::context_switch_on_demand = 1;
	System::timer();
	enable_interrupts
}

#ifdef FORK_IMPL

#include <STRING.H>

volatile PCB* parent_PCB, *child_PCB;
volatile Thread* child_thread;
volatile unsigned curr_bp, curr_ss, child_offset, parent_offset, new_child_offset, new_parent_offset, relative_offset;

void interrupt copy_and_adjust_stack() {
	memcpy(child_PCB->stack, parent_PCB->stack, parent_PCB->stack_size);
	asm {
		mov curr_bp, bp
		mov curr_ss, ss
	}
	child_PCB->ss = FP_SEG(child_PCB->stack);
	parent_offset = FP_OFF(parent_PCB->stack);
	child_offset = FP_OFF(child_PCB->stack);

	relative_offset = parent_offset - child_offset;
	child_PCB->bp = child_PCB->sp = curr_bp - relative_offset;

	parent_offset = curr_bp;
	child_offset = child_PCB->bp;

	while(1) {

		new_parent_offset = *(unsigned*)(MK_FP(curr_ss, parent_offset));
		new_child_offset = *(unsigned*)(MK_FP(child_PCB->ss, child_offset));

		if(!new_parent_offset){
			new_child_offset = 0;
			break;
		}

		new_child_offset = new_parent_offset - relative_offset;

		parent_offset = new_parent_offset;
		child_offset = new_child_offset;
	}

	child_PCB->my_thread->start();
}

ID Thread::fork() {
	lock
	parent_PCB = System::running;
	if(parent_PCB->children_list == 0 && parent_PCB->parent_sem == 0) {
		parent_PCB->children_list = new List();
		parent_PCB->parent_sem = new Semaphore(0);
	}

	child_thread = parent_PCB->my_thread->clone();
	if(!child_thread || !child_thread->myPCB->stack) {
		if(child_thread)
			delete child_thread;
		unlock
		return -1;
	}
	child_PCB = child_thread->myPCB;
	child_PCB->parent = (PCB*)parent_PCB;
	parent_PCB->children_list->push_back((PCB*)child_PCB);

	copy_and_adjust_stack();

	if(System::running == parent_PCB) {
		unlock
		return child_thread->getId();
	}
	else
		return 0;

}

void Thread::exit() {
	System::running->exit();
}

void Thread::waitForForkChildren() {

	System::running->wait_for_fork_children();
}

Thread* Thread::clone() const {
	return 0;
}
#endif
