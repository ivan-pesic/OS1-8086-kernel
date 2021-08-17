/*
 * ivtentry.h
 *
 *  Created on: Aug 16, 2021
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_


typedef unsigned char IVTNo;
typedef void interrupt (*p_interrupt)(...);
class KernelEv;

class IVTEntry {
	IVTNo ivt_number;
	KernelEv* kernel_event;
	p_interrupt new_interrupt_routine;
	friend class KernelEv;
public:
	IVTEntry(IVTNo ivt_number, p_interrupt new_interrupt_routine);
	~IVTEntry();

	p_interrupt old_interrupt_routine;

	void signal();
	void call_old_routine();
};

#endif /* IVTENTRY_H_ */
