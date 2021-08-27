#include <DOS.H>
#include <STDIO.H>
#include <STDARG.H>
#include "system.h"

/*
 *
 * These two functions were being used for testing purposes.
 * They were found on Google Drive of older students who
 * left their tests there.
 * Functions are **NOT** integral part of the kernel code.
 * Keep that in mind when scanning for similarities
 * between students' code.
 *
 */

int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	lock
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	unlock
		return res;
}

void dumbSleep(int delay) {
      for (int i = 0; i < 1000; ++i) {
          for (int j = 0; j < delay; ++j);
      }
  }
