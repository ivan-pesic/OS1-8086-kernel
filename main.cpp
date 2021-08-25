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
	}
	~user_thread() { waitToComplete(); }
protected:
	void run() {
		userMain(this->argc, this->argv);
	}
private:
	int argc;
	char** argv;
};

int main(int argc, char** argv){

	System::inic();

	user_thread* user = new user_thread(argc, argv);
	user->start();
	delete user;

	System::restore();

	return 0;
}
