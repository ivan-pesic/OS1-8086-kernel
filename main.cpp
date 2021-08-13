//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>
#include "pcb.h"
#include "SCHEDULE.H"
#include "list.h"
#include "system.h"
#include "Thread.h"
#include "Thread.h"

int userMain(int argc, char** argv);

int main(int argc, char** argv){

	System::inic();

	userMain(argc, argv);

	System::restore();

	return 0;
}
