//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>
#include "pcb.h"
#include "SCHEDULE.H"
#include "list.h"
#include "system.h"
#include "Thread.h"
#include "Thread.h"

void tick() {
	/*
	disable_interrupts
	cout << "\nSystem::time = " << System::time << ", current thread: " << ;
	enable_interrupts
	*/
}

class TestA : public Thread {
public:
	TestA (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice) : Thread(stackSize, timeSlice) {}
	virtual void run();
	virtual ~TestA() { waitToComplete(); }
};

class TestB : public Thread {
public:
	TestB (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice) : Thread(stackSize, timeSlice) {}
	virtual void run();
	virtual ~TestB() {waitToComplete();}
};

void TestA::run(){
	for (int i =0; i < 30; ++i) {
		disable_interrupts
		cout<<"u a() i = "<<i<<endl;
		enable_interrupts
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	//System::exitThread();
}

void TestB::run(){
	for (int i =0; i < 30; ++i) {
		disable_interrupts
		cout<<"u b() i = "<<i<<endl;
		enable_interrupts
		for (int k = 0; k<30000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	//System::exitThread();
}

void doSomething(){
	lock

	TestA* p1 = new TestA(1, 40);

	disable_interrupts
	cout<<"Napravio a"<<endl;
	enable_interrupts

	TestB* p2 = new TestB(1, 4);

	disable_interrupts
	cout<<"Napravio b"<<endl;
	enable_interrupts

	unlock

	p1->start();
	p2->start();
	for (int i = 0; i < 30; ++i) {
		disable_interrupts
		cout<<"u tds i = "<<i<<endl;
		enable_interrupts

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	p1->waitToComplete();
	p2->waitToComplete();
	lock
	delete p1;
	delete p2;
	unlock
	disable_interrupts
	cout<<"Srecan kraj!"<<endl;
	enable_interrupts

}
int main(){

	System::inic();

	doSomething();

	System::restore();

	return 0;
}
