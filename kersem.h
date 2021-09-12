/*
 * kersem.h
 *
 *  Created on: Aug 13, 2021
 *      Author: OS1
 */

#ifndef KERSEM_H_
#define KERSEM_H_

#include "list.h"

typedef unsigned int Time;
class PCB;
class Semaphore;


class KernelSem {
public:

	static int waiting_data_counter;
	static int live_semaphores;

	KernelSem(int initial_value);
	~KernelSem();

	static void tick();
	void update_list();

	//mod
	void function1(char c);
	void function2();
	enum Operation { NOP, RD, WR };
	Operation current_operation;
	struct holder_struct {
		PCB* holder;
		Operation op;
		holder_struct(PCB* h, Operation o) {
			holder = h;
			op = o;
		}
	};
	List holders;
	List requests;

	int wait(Time max_time_to_wait);
	void signal();

	int val() const;
private:
	struct waiting_data {
		PCB* pcb;
		KernelSem* semaphore;
		Time time_to_wait;
		waiting_data(PCB* p, KernelSem* ks, Time t) {
			pcb = p; semaphore = ks; time_to_wait = t;
			waiting_data_counter++;
		}
		~waiting_data() {
			waiting_data_counter--;
		}
	};

	void insert_waiting(waiting_data* wd);
	void increment();
	int value;
	List blocked;
	List waiting;
};

#endif /* KERSEM_H_ */
