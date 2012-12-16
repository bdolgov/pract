#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

void report_error(const char* fmt, ...);
void report_error2(int rc, const char* fmt, ...);

typedef struct random_s
{
	int seed;
	int (*next)(struct random_s* rnd, int a, int b);
	void (*free)(struct random_s* rnd);
} random_t;

random_t *random_new_rand(int seed);

enum { TRACE_READ = 0, TRACE_WRITE = 1, TRACE_INSTR = 0, TRACE_DATA = 1 };

typedef struct
{
	unsigned op:1;
	unsigned type:1;
	unsigned size:4;
	unsigned int addr;
	long long value;
} traceop_t;

traceop_t *read_trace(FILE* f);

#endif
