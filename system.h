/*
 * system.h
 *
 *  Created on: Aug 12, 2021
 *      Author: OS1
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "IOSTREAM.H"

class PCB;
class List;
class Thread;

// deklaracija dispatch-a
void dispatch();
// deklaracija tick-a
void tick();

// Zabranjuje prekide
#define disable_interrupts asm { pushf; cli; }
// Dozvoljava prekide
#define enable_interrupts asm popf;

// Zabranjuje prekide bez promene konteksta
#define lock ++System::lock_counter;
// Dozvoljava prekide
#define unlock\
	if(--System::lock_counter < 0) {cout << "ERROR";}\
	if(System::lock_counter == 0 && System::lock_flag) { dispatch(); }\

#define in_locked_section (System::lock_counter > 0)

// potpis interrupt rutine
typedef void interrupt (*p_interrupt)(...);

class System {
public:
	// structures
	static List all_PCBs;
	static List all_semaphores;
	// necessary variables
	static PCB* main_PCB;
	static PCB* idle_PCB;
	static Thread* idle_thread;
	// for timer
	static p_interrupt old_isr;
	static volatile PCB* running;
	static volatile int time;
	static volatile int context_switch_on_demand;
	// for locking
	static volatile int lock_counter;
	static volatile int lock_flag;
	// functions
	static void interrupt timer(...);
	static void inic();
	static void restore();
	//static void dispatch();
};

#endif /* SYSTEM_H_ */
