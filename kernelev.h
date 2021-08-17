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
	IVTNo ivt_number;
	PCB* creator_pcb;
	PCB* blocked;
public:
	KernelEv(IVTNo ivt_number);
	~KernelEv();
	void wait();
	void signal();
};

#endif /* KERNELEV_H_ */
