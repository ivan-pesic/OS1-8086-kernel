/*
 * Barrier.cpp
 *
 *  Created on: Sep 7, 2021
 *      Author: OS1
 */

#include "Barrier.h"
#include "Thread.h"
#include "pcb.h"
#include "system.h"
Barrier::Barrier(int open) {
	lock
	this->open_flag = open;
	this->blocked = 0;
	this->owner = (PCB*)System::running;
	unlock
}

void Barrier::open() {
	lock
	open_flag = 1;
	if(blocked) {
		this->owner->unblock();
		blocked = 0;
	}
	unlock
}

void Barrier::close() {
	lock
	open_flag = 0;
	unlock
}

void Barrier::pass() {
	lock
	if(System::running != owner) {
		unlock
		return;
	}
	if(open_flag) {
		unlock;
		return;
	}
	owner->block();
	blocked = 1;
	unlock
	dispatch();
}
