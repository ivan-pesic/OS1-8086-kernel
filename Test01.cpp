#include "Thread.h"
#include "Semaphor.h"
#include <STDLIB.H>
#include <IOSTREAM.H>
#include "system.h"
int syncPrintf(const char *format, ...);
void dumbSleep(int delay);

Semaphore sem;

class Producer : public Thread {
    public:
        Producer() : Thread(1, 10) {}
        virtual void run();
        ~Producer() {
            waitToComplete();
        }
};

void Producer::run() {
    while (1) {
        syncPrintf("Prodooc\n");
        sem.signal();
        dumbSleep(rand() % 1000);
    }
}

class Consumer : public Thread {
    public:
        Consumer() : Thread(1, 10) {}
        virtual void run();
        ~Consumer() {
            waitToComplete();
        }
};

void Consumer::run() {
    while (1) {
        int waitResult = sem.wait(0);
        if (waitResult) {
            syncPrintf("Consoomed %d\n", getId());
        } else {
            syncPrintf("CONSOOM FAILED %d\n", getId());
        }
        dumbSleep(rand() % 1000);
    }
}

void tick() {}

int userMain(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
    Producer p;
    Consumer c[10];
  //  lock
    p.start();
    for (unsigned i = 0; i < 10; ++i) {
        c[i].start();
    }
   // unlock
    return 0;
}
