#include "Thread.h"
#include "Semaphor.h"
#include <STDLIB.H>
#include <IOSTREAM.H>
#include "system.h"
int syncPrintf(const char *format, ...);
void dumbSleep(int delay);

/**
 * USER7.CPP
 *
 * Creates the maximum amount of threads possible within the system memory
 * and frees them, hoping nothing will go wrong.
 */
#define nullptr 0
class OveruseThread : public Thread {
    public:
        OveruseThread() : Thread(1, 20) {}
        virtual void run() {
            syncPrintf("This should not happen.\n");
        }
        ~OveruseThread() {
            waitToComplete();
        }
};

void tick() {}

Thread* threads[500];

int userMain(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
    unsigned i = 0;
    for (; i < 500; ++i) {
        syncPrintf("Creating %d\n", i);
        lock
        threads[i] = new OveruseThread();
        unlock
        if (threads[i] == nullptr || threads[i]->getId() == -1) {
            syncPrintf("Failed at index %d\n", i);
            if (threads[i] != nullptr) {
                lock
                delete threads[i];
                unlock
            }
            break;
        }
    }
    for (unsigned j = 0; j < i; ++j) {
        lock
        delete threads[j];
        unlock
    }
    syncPrintf("Done\n");
    return 0;
}
