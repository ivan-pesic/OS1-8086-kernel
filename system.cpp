/*
 * system.cpp
 *
 *  Created on: Aug 12, 2021
 *      Author: OS1
 */

#include "system.h"
#include "dos.h"
#include "pcb.h"
#include "list.h"
#include "SCHEDULE.H"
#include "idle.h"

volatile PCB* System::running = 0;
volatile int System::context_switch_on_demand = 0;
volatile int System::lock_counter = 0;
volatile int System::lock_flag = 0;

PCB* System::main_PCB = 0;
PCB* System::idle_PCB = 0;
Thread* System::idle_thread = 0;

p_interrupt System::old_isr = 0;
volatile int System::time = defaultTimeSlice;

unsigned tsp = 0;
unsigned tss = 0;
unsigned tbp = 0;

// definition of lists of all PCBs and all Semaphores
List System::all_PCBs;
List System::all_semaphores;

// prekidna rutina
void interrupt System::timer(...){
	if (!System::context_switch_on_demand) {
		(*System::old_isr)();
		tick();
		// TODO: implement
		//Kernel_Sem::tick();
	}

	if(!System::context_switch_on_demand && System::time > 0) {
		System::time--;
	}

	if ((System::time == 0 && !in_locked_section) || System::context_switch_on_demand) {
		System::context_switch_on_demand = 0;
		System::lock_flag = 0;

		asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		System::running->sp = tsp;
		System::running->ss = tss;
		System::running->bp = tbp;
		System::running->lock_cnt = System::lock_counter;

		if(System::running->state == PCB::READY)
			Scheduler::put((PCB*)System::running);

		System::running = Scheduler::get();
		if(System::running == 0) {
			// TODO: add IDLE thread and PCB
		}
		tsp = System::running->sp;
		tss = System::running->ss;
		tbp = System::running->bp;
		System::lock_counter = System::running->lock_cnt;
		System::time = running->time_slice;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp, tbp
		}
	}
	else if(System::time == 0 && in_locked_section) {
		System::lock_flag = 1;
	}
}

/*void System::dispatch(){ // sinhrona promena konteksta
	disable_interrupts
	System::context_switch_on_demand = 1;
	System::timer();
	enable_interrupts
}*/

// postavlja novu prekidnu rutinu
void System::inic(){
	System::main_PCB = new PCB();
	System::idle_thread = new Idle();
	System::idle_PCB = PCB::get_idle_PCB();
	idle_PCB->state = PCB::IDLE;
	System::running = System::main_PCB;
	disable_interrupts
	System::old_isr = getvect(8);
	setvect(8, System::timer);
	enable_interrupts
}

// vraca staru prekidnu rutinu
void System::restore(){
	disable_interrupts
	setvect(8, System::old_isr);
	enable_interrupts

	cout << "\nNumber of nodes remaining: " << List::number_of_nodes;
	cout << "\nLock counter: " << System::lock_counter;
	cout << "\nLive PCBs: " << PCB::live_PCBs;

	// deleting remaining PCBs
	while(!System::all_PCBs.empty()) {
		PCB* pcb = (PCB*)(System::all_PCBs.pop_front());
		if(pcb)
			delete pcb;
	}

	// check print
	cout << "\nNumber of nodes remaining: " << List::number_of_nodes;
	cout << "\nLock counter: " << System::lock_counter;
	cout << "\nLive PCBs: " << PCB::live_PCBs;
}
