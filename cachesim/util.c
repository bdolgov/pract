#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void report_error(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}
