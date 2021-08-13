/*
 * idle.h
 *
 *  Created on: Aug 13, 2021
 *      Author: OS1
 */

#ifndef IDLE_H_
#define IDLE_H_

#include "Thread.h"

class Idle: public Thread {
public:
	Idle() : Thread(1024, 1) {}
protected:
	virtual void run();
};

#endif /* IDLE_H_ */
