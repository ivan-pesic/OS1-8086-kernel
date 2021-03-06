//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>
#include "pcb.h"
#include "SCHEDULE.H"
#include "list.h"
#include "system.h"
#include "Thread.h"

int userMain(int argc, char** argv);

class user_thread : public Thread {
public:
	user_thread(int argc, char** argv) : Thread(defaultStackSize, defaultTimeSlice) {
		this->argc = argc;
		this->argv = argv;
		this->ret = 0;
	}

	~user_thread() { waitToComplete(); }

	virtual Thread* clone() const {
		return new user_thread(argc, argv);
	}

	int get_ret() {
		return ret;
	}
protected:
	void run() {
		ret = userMain(this->argc, this->argv);
	}
private:
	int ret;
	int argc;
	char** argv;
};

int ret_val = 0;

int main(int argc, char** argv){

	System::inic();

	user_thread* user = new user_thread(argc, argv);
	user->start();
	user->waitToComplete();
	ret_val = user->get_ret();
	delete user;

	System::restore();

	return ret_val;
}
