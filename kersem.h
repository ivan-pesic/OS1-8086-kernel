/*
 * kersem.h
 *
 *  Created on: Aug 13, 2021
 *      Author: OS1
 */

#ifndef KERSEM_H_
#define KERSEM_H_

class KernelSem {
	KernelSem(int initial_value);
	~KernelSem();

	static void tick();
	void update_list();

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
		}
	};

	void inser_waiting(waiting_data* wd);
	void increment();
	int value;
	List blocked;
	List waiting;
};

#endif /* KERSEM_H_ */
