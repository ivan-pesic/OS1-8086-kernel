#include "Thread.h"
#include "Semaphor.h"
#include <STDLIB.H>
#include <IOSTREAM.H>
#include "system.h"
#include "Event.h"
int syncPrintf(const char *format, ...);
void dumbSleep(int delay);

class ForkThread : public Thread {
    public:
        ForkThread() : Thread(1, 1) {}
        virtual void run();
        virtual Thread* clone() const {
            return new ForkThread();
        }
        ~ForkThread() {
            waitToComplete();
        }
        static volatile int failedFork;
};

volatile int ForkThread::failedFork = 0;

void ForkThread::run() {
    while (!failedFork) {
        ID forked = fork();
        if (forked < 0) {
            syncPrintf("Failed to fork in thread %d!\n", getRunningId());
            failedFork = 1;
            break;
        } else if (forked == 0) {
            syncPrintf("We are in child %d\n", getRunningId());
        } else {
            syncPrintf("Cloned thread: %d\n", forked);
            dumbSleep(10000);
        }
    }
    waitForForkChildren();
}

void tick() {}

int userMain(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
    ForkThread t;
    t.start();
    return 0;
}
