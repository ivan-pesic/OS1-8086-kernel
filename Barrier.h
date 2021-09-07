/*
 * Barrier.h
 *
 *  Created on: Sep 7, 2021
 *      Author: OS1
 */

#ifndef BARRIER_H_
#define BARRIER_H_

class PCB;

class Barrier {
public:
	Barrier(int open = 1);
	void open();
	void close();
	void pass();
private:
	int open_flag;
	int blocked;
	PCB* owner;
};

#endif /* BARRIER_H_ */
