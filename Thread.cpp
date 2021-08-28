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
	disable_interrupts
	//assert(System::lock_counter == 0);
	System::context_switch_on_demand = 1;
	System::timer();
	enable_interrupts
}

#ifdef FORK_IMPL

#include <STRING.H>

volatile PCB* parent_PCB, *child_PCB;
volatile Thread* child_thread;
volatile unsigned parent_bp, parent_ss, parent_sp, child_offset, parent_offset, relative_offset;
volatile unsigned* child_curr_bp, *parent_curr_bp;

void interrupt copy_and_adjust_stack() {
	memcpy(child_PCB->stack, parent_PCB->stack, parent_PCB->stack_size);

	asm {
		mov parent_bp, bp
		mov parent_ss, ss
		mov parent_sp, sp
	}

	relative_offset = FP_OFF(parent_PCB->stack) - FP_OFF(child_PCB->stack);

	child_PCB->ss = FP_SEG(child_PCB->stack);
	child_PCB->bp = parent_bp - relative_offset;
	child_PCB->sp = parent_sp - relative_offset;

	parent_offset = parent_bp;
	child_offset = child_PCB->bp;

	parent_curr_bp = (unsigned*)(MK_FP(parent_ss, parent_offset));
	child_curr_bp = (unsigned*)(MK_FP(child_PCB->ss, child_offset));

	for(; (*parent_curr_bp) != 0; parent_offset = *parent_curr_bp, child_offset = *child_curr_bp) {
		parent_curr_bp = (unsigned*)(MK_FP(parent_ss, parent_offset));
		child_curr_bp = (unsigned*)(MK_FP(child_PCB->ss, child_offset));
		*child_curr_bp = *parent_curr_bp - relative_offset;
	}
	*child_curr_bp = 0;

	child_PCB->my_thread->start();
}

ID Thread::fork() {
	lock
	parent_PCB = System::running;
	if(parent_PCB->children_list == 0 && parent_PCB->parent_sem == 0) {
		parent_PCB->children_list = new List();
		parent_PCB->parent_sem = new Semaphore(0);
		if(!parent_PCB->children_list || !parent_PCB->parent_sem) {
			unlock
			return -1;
		}
	}

	child_thread = parent_PCB->my_thread->clone();
	if(!child_thread || !child_thread->myPCB || (child_thread && child_thread->myPCB && !child_thread->myPCB->stack)) {
		if(child_thread && child_thread->myPCB)
			delete child_thread;
		unlock
		return -1;
	}
	child_PCB = child_thread->myPCB;
	child_PCB->parent = (PCB*)parent_PCB;
	if(!parent_PCB->children_list->push_back((PCB*)child_PCB)) {
		delete child_thread;
		unlock
		return -1;
	}

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
