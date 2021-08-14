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
#include "kersem.h"

int syncPrintf(const char *format, ...);

volatile PCB* System::running = 0;
volatile int System::context_switch_on_demand = 0;
volatile int System::lock_counter = 0;
volatile int System::lock_flag = 0;
volatile int System::sem_lock_counter = 0;
volatile int System::sem_artificial_ticks = 0;

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
		if(!sem_locked) {
			while(System::sem_artificial_ticks >= 0) {
				tick();
				KernelSem::tick();
				System::sem_artificial_ticks--;
			}
			System::sem_artificial_ticks = 0;
		}
		else {
			System::sem_artificial_ticks++;
		}
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
			syncPrintf("\nU IDLE!");
			System::running = System::idle_PCB;
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
	lock

	disable_interrupts
	System::old_isr = getvect(8);
	setvect(8, System::timer);
	enable_interrupts

	System::main_PCB = new PCB();
	System::idle_thread = new Idle();
	System::idle_PCB = PCB::get_idle_PCB();
	idle_PCB->state = PCB::IDLE;
	System::running = System::main_PCB;

	unlock
}

// vraca staru prekidnu rutinu
void System::restore(){

	disable_interrupts
	setvect(8, System::old_isr);
	enable_interrupts

	lock

	disable_interrupts
	syncPrintf("\nNumber of nodes remaining: %d", List::number_of_nodes);
	syncPrintf("\nLock counter: %d", System::lock_counter);
	syncPrintf("\nLive PCBs: %d", PCB::live_PCBs);
	syncPrintf("\nLive Semaphores: %d", KernelSem::live_semaphores);
	syncPrintf("\nwaiting_data_counter: %d", KernelSem::waiting_data_counter);
	enable_interrupts

	// deleting remaining semaphores
	sem_lock
	while(!System::all_semaphores.empty()) {
		KernelSem* sem = (KernelSem*)(System::all_semaphores.pop_front());
		if(sem) {
			delete sem;
		}

	}
	sem_unlock

	// deleting remaining PCBs
	while(!System::all_PCBs.empty()) {
		PCB* pcb = (PCB*)(System::all_PCBs.pop_front());
		if(pcb)
			delete pcb;
	}

	disable_interrupts
	// check print
	syncPrintf("\nNumber of nodes remaining: %d", List::number_of_nodes);
	syncPrintf("\nLock counter: %d", System::lock_counter);
	syncPrintf("\nLive PCBs: %d", PCB::live_PCBs);
	syncPrintf("\nLive Semaphores: %d", KernelSem::live_semaphores);
	syncPrintf("\nwaiting_data_counter: %d", KernelSem::waiting_data_counter);
	enable_interrupts

	unlock
}
