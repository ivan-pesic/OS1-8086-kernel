/*
 * pcb.h
 *
 *  Created on: Aug 11, 2021
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

class PCB {
public:
	unsigned sp;
	unsigned ss;
	unsigned bp;
	unsigned* stack;
	unsigned finished;
	int time_slice;
};

#endif /* PCB_H_ */
