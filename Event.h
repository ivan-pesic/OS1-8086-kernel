/*
 * Event.h
 *
 *  Created on: Aug 16, 2021
 *      Author: OS1
 */

#ifndef EVENT_H_
#define EVENT_H_

typedef unsigned char IVTNo;
class KernelEv;

#include "ivtentry.h"

#define PREPAREENTRY(ivt_number, call_old)\
		extern IVTEntry ivt_entry##ivt_number;\
		void interrupt interrupt_event##ivt_number(...) {\
			ivt_entry##ivt_number.signal();\
			if (call_old) ivt_entry##ivt_number.call_old_routine();\
		}\
		IVTEntry ivt_entry##ivt_number(ivt_number, interrupt_event##ivt_number);


class Event {
public:
	Event (IVTNo ivtNo);
	~Event ();

	void wait  ();
protected:
	friend class KernelEv;
	void signal(); // can call KernelEv

private:
	KernelEv* myImpl;
};

#endif /* EVENT_H_ */
