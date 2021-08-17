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
class IVTEntry;

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
#define unlock if(--System::lock_counter < 0) {cout << "ERROR";}
//	if(System::lock_counter == 0 && System::lock_flag) { dispatch(); }\

// Zakljucava globalnu listu semafora
#define sem_lock ++System::sem_lock_counter;
// Otkljucava globalnu listu semafora
#define sem_unlock if(--System::sem_lock_counter < 0) { cout << "ERROR";}

// Provera da li se nalazim u zakljucanoj sekciji
#define in_locked_section (System::lock_counter > 0)
// Provera da li sam zakljucao globalnu listu semafora
#define sem_locked (System::sem_lock_counter > 0)

// potpis interrupt rutine
typedef void interrupt (*p_interrupt)(...);

class System {
public:
	// structures
	static List all_PCBs;
	static List all_semaphores;
	static IVTEntry* entries[256];
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
	static volatile int sem_lock_counter;
	static volatile int sem_artificial_ticks;
	// functions
	static void interrupt timer(...);
	static void inic();
	static void restore();
	//static void dispatch();
};

#endif /* SYSTEM_H_ */
