#include "Thread.h"
int syncPrintf(const char *format, ...);
void dumbSleep(int delay);
/*
	Test: Niti maxStack velicine
 */

const int n = 10;

void tick(){}

class TestThread : public Thread
{
public:

	TestThread(): Thread(65535,2) {};
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{

	int buffer=2;

	for(int i=0;i<32000;i++)
	{
		buffer = 4096/2048;
		for (int j = 0; j < 1024; j++)
		{
			buffer = buffer*2;
			if(buffer%2)
				buffer = 2;
		}
	}

}


int userMain(int argc, char** argv)
{
	syncPrintf("Test starts: %d threads.\n",n);
	int i;
	TestThread threads[n];
	Thread *th = 0;
	for(i=0;i<n;i++)
	{
		threads[i].start();
	}
	for(i=0;i<n;i++)
	{
		threads[i].waitToComplete();
		th = Thread::getThreadById(i+3);
		if (!th) syncPrintf("MEGA ERROR");
		else syncPrintf("%d. Done!\n",th->getId());
	}
	syncPrintf("Test ends.\n");
	return 0;
}
