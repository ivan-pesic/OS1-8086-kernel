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

	KernelSem(int undo, int initial_value);
	~KernelSem();

	static void tick();
	void update_list();

	int wait(Time max_time_to_wait);
	void signal();

	int val() const;
protected:
	friend class PCB;
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
	int undo;
	int value;
	List blocked;
	List waiting;
};

#endif /* KERSEM_H_ */
