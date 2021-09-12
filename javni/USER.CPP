//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    user.cpp
//   Date:    Maj 2021
//===========================================================================//
#include <iostream.h>
#include <stdlib.h>
#include <assert.h>

#include "keyevent.h"
#include "bounded.h"
#include "user.h"
#include "intLock.h"
#include <event.h>
#include "semaphor.h"

//---------------------------------------------------------------------------//
//  Otkomentarisati ukoliko se testira fork
//---------------------------------------------------------------------------//
#define FORK
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//  Ovo se menja u testu
//---------------------------------------------------------------------------//

Time TIME_SLICE = 2;   // 0 ili defaultTimeSlice

int N = 3;            // 1 <= N <= 19

//---------------------------------------------------------------------------//

volatile int theEnd=0;

class Producer : public Thread {
public:
	Producer (BoundedBuffer* bb, char y, Time time_slice);
	virtual ~Producer() {waitToComplete(); }
	Thread* clone() const { return new Producer(myBuffer, x, time_slice_clone); }
protected:
	virtual void run ();
	char produce() {return x;}; // Produce an item

private:
	Time time_slice_clone;
	BoundedBuffer* myBuffer;
	char x;
	Semaphore sleep;
};



//---------------------------------------------------------------------------//
class Consumer : public Thread {
public:
	Consumer (BoundedBuffer* bb) : Thread(defaultStackSize, 0), myBuffer(bb), sleep(0) {}
	virtual ~Consumer() {waitToComplete(); }
	Thread* clone() const { return new Consumer(myBuffer); }
protected:
	virtual void run ();
	void consume(char p); // Consume an item

private:
	BoundedBuffer* myBuffer;
	Semaphore sleep;
};




//---------------------------------------------------------------------------//
// Korisnicki program mora obavezno da definise ovu f-ju
//---------------------------------------------------------------------------//
void tick(){
}

//---------------------------------------------------------------------------//


Producer::Producer (BoundedBuffer* bb, char y, Time time_slice) 
: Thread(defaultStackSize, time_slice),myBuffer(bb), x(y), sleep(0), time_slice_clone(time_slice) {}

void Producer::run () {
	while(!theEnd) {
		char d = produce();
		myBuffer->append(d);
		assert(1 != sleep.wait(10));
	}
}

//---------------------------------------------------------------------------//


void Consumer::consume(char p) {
	intLock
	cout<<p<<" ";
	intUnlock
} // Consume an item

void Consumer::run () {

	int i = 0;
	while(!theEnd) {
		char d = myBuffer->take();
		consume(d);
		if (i++ == 40) {
			assert(1 != sleep.wait(5));
			i = 0;
		}else for(int j=0;j<200;j++);
	}

	intLock
	cout<<endl<<"ESC pressed - empty the buffer!"<<endl;
	intUnlock

	while (myBuffer->fullCount()){
		char d = myBuffer->take();
		consume(d);
		dispatch();
	}


	intLock
	cout<<endl<<"Happy End"<<endl;
	intUnlock
}



class Th : public Thread {
public:
	Th(Semaphore* s, int i) : Thread() {sem = s; id = i;}
	~Th() { waitToComplete(); }
protected:
	void run() {
		for(int i = 0; i < 5; i++) {
			sem->wait(0);
			cout << "Critical section " << id << " " << sem->val() << endl;
			sem->signal();
		}
		sem->signal();
		sem->signal();
		sem->signal();
		sem->signal();
		sem->signal();
		sem->signal();
		sem->signal();
	}
private:
	Semaphore* sem;
	int id;
};

int userMain (int argc, char* argv[])
{
	Semaphore* sem = new Semaphore(1);
	Th* threads[3];
	for(int i = 0; i < 3; i++) {
		threads[i] = new Th(sem, i);
		threads[i]->start();
	}

	for(int j = 0; j < 3; j++) {
		delete threads[j];
	}
	cout << sem->val() << endl;
}
//---------------------------------------------------------------------------//
