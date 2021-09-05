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
#include "list.h"

class IVTEntry {
	IVTNo ivt_number;
	KernelEv* kernel_event;
	p_interrupt new_interrupt_routine;
	int flag;
	struct list_elem {
		KernelEv* event;
		int priority;
		list_elem(KernelEv* ev, int pr) {
			event = ev;
			priority = pr;
		}
	};
	List list_of_events;
	friend class KernelEv;
public:
	IVTEntry(IVTNo ivt_number, p_interrupt new_interrupt_routine);
	~IVTEntry();

	p_interrupt old_interrupt_routine;
	void insert_in_list(KernelEv* kernel_event, int priority);
	void remove_from_list(KernelEv* kernel_event);
	void signal();
	void call_old_routine();
};

#endif /* IVTENTRY_H_ */
