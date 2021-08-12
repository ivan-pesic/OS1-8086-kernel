//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>
#include "pcb.h"
#include "SCHEDULE.H"
#include "list.h"

// Zabranjuje prekide
#define disable_interrupts asm { pushf; cli; }
// Dozvoljava prekide
#define enable_interrupts asm popf;
// potpis interrupt rutine
typedef void interrupt (*p_interrupt)(...);
// stara prekidna rutina
p_interrupt old_isr;

List list;
PCB *p[3];
volatile PCB* running;

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int time = 2;
volatile int context_switch_on_demand = 0;

// prekidna rutina
void interrupt timer(...){
	if (!context_switch_on_demand) {
		time--;
		(*old_isr)();
	}
	if (time == 0 || context_switch_on_demand) {
		context_switch_on_demand = 0;
		asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		running->sp = tsp;
		running->ss = tss;
		running->bp = tbp;

		if(!running->finished)
			Scheduler::put((PCB*)running);

		running = Scheduler::get();

		tsp = running->sp;
		tss = running->ss;
		tbp = running->bp;

		time = running->time_slice;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp, tbp
		}
	}
}

void dispatch(){ // sinhrona promena konteksta
	disable_interrupts
	context_switch_on_demand = 1;
	timer();
	enable_interrupts
}

// postavlja novu prekidnu rutinu
void inic(){
	disable_interrupts
	old_isr = getvect(8);
	setvect(8, timer);
	enable_interrupts
}

// vraca staru prekidnu rutinu
void restore(){
	disable_interrupts
	setvect(8, old_isr);
	enable_interrupts
}

void exitThread(){
	running->finished = 1;
	dispatch();
}

void a(){
	for (int i =0; i < 30; ++i) {
		disable_interrupts
		cout<<"u a() i = "<<i<<endl;
		enable_interrupts
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	exitThread();
}

void b(){
	for (int i =0; i < 30; ++i) {
		disable_interrupts
		cout<<"u b() i = "<<i<<endl;
		enable_interrupts
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	exitThread();
}


void createProcess(PCB *newPCB, void (*body)()){
	unsigned* st1 = new unsigned[1024];

	st1[1023] =0x200;//setovan I fleg u
	// pocetnom PSW-u za nit
	st1[1022] = FP_SEG(body);
	st1[1021] = FP_OFF(body);

	newPCB->sp = FP_OFF(st1+1012); //svi sacuvani registri
	//pri ulasku u interrupt
	//rutinu
	newPCB->ss = FP_SEG(st1+1012);
	newPCB->finished = 0;
	Scheduler::put(newPCB);
}



void doSomething(){



	disable_interrupts
	PCB* p1 = new PCB();
	//p[1] = new PCB();
	createProcess(p1,a);
	cout<<"napravio a"<<endl;
	p1->time_slice = 40;
	list.push_back(p1);

	PCB* p2 = new PCB();
	createProcess(p2,b);
	cout<<"napravio b"<<endl;
	p2->time_slice = 20;
	list.push_back(p2);

	PCB* p0 = new PCB();
	p0->time_slice = 2; // bravo Dejane
	p0->finished = 0;
	list.push_back(p0);

	running = p0;
	enable_interrupts


	for (int i = 0; i < 30; ++i) {
		disable_interrupts
		cout<<"main "<<i<<endl;
		enable_interrupts
		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	while(!(p[1]->finished && p[2]->finished))
		dispatch();
	cout<<"Happy End"<<endl;
}

int main(){

	inic();

	doSomething();

	restore();

	for(list.to_front(); list.has_current(); list.to_next()) {
		PCB* pcb = (PCB*)(list.get_current_data());
		delete pcb;
		list.pop_front();
	}
	cout << List::number_of_nodes;
	return 0;
}
