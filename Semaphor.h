/*
 * Semaphor.h
 *
 *  Created on: Aug 13, 2021
 *      Author: OS1
 */

#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_

typedef unsigned int Time;
typedef int ID;
class KernelSem;

class Semaphore {
public:
	// for testing:
	//void printList();
	Semaphore (int init=1);
	virtual ~Semaphore ();

	virtual int wait (Time maxTimeToWait);
	virtual void signal();

	static void addOwner();
	void removeOwner(ID id);

	int val () const;  // Returns the current value of the semaphore
private:
	KernelSem* myImpl;
};

#endif /* SEMAPHOR_H_ */
