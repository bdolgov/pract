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

void report_error2(int rc, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	if (rc != -1) exit(rc);
}

typedef struct
{
	int seed;
	int (*next)(random_t* rnd, int a, int b);
	void (*free)(random_t* rnd);
	int n;
} rand_random_t;

static int rand_next(random_t *_rnd, int a, int b)
{
	rand_random_t *rnd = (rand_random_t*)_rnd;
	static rand_random_t* cur = NULL;
	if (cur != rnd)
	{
		srand(rnd->seed);
		for (int i = 0; i < rnd->n; ++i) rand();
	}
	++rnd->n;
	return rand() / (RAND_MAX + 1.0) * (b - a) + a;
}

random_t *random_new_rand(int seed)
{
	rand_random_t *rnd = malloc(sizeof(rand_random_t));
	rnd->seed = seed;
	rnd->next = rand_next;
	rnd->free = (void(*)(random_t*))free;
	rnd->n = 0;
	return (random_t*)rnd;
}

traceop_t *read_trace(FILE* f)
{
	static traceop_t ret;
	static char buf[sizeof("RI 00000000 0 -9223372036854775808\n")];
	if (!fgets(buf, sizeof(buf), f)) return NULL;
	char st[3];
	int size;
	int rc = sscanf(buf, "%s %x %u %lld", st, &ret.addr, &size, &ret.value);
	ret.size = size;
	ret.op = st[0] == 'W';
	ret.type = st[1] == 'D';
	if (rc == 2)
	{
		ret.size = 1;
		ret.value = 0;
	}
	else if (rc != 4)
	{
		return NULL;
	}
	return &ret;
}
