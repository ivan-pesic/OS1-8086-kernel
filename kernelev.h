/*
 * kernelev.h
 *
 *  Created on: Aug 16, 2021
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

class PCB;

typedef unsigned char IVTNo;

class KernelEv {
	int value;
	int blocked;
	IVTNo ivt_number;
	PCB* creator_pcb;
public:
	KernelEv(IVTNo ivt_number, int priority);
	~KernelEv();
	void wait();
	void signal();
};

#endif /* KERNELEV_H_ */
