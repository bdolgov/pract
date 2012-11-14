#ifndef PROCESS_H
#define PROCESS_H
#include <sys/types.h>

typedef struct
{
	int f;
	int fd;
} ioredirect_t;

enum { STOPPED, RUNNING, EXITED };

typedef struct 
{
	pid_t pid;
	char **args;
	ioredirect_t *redirs;
	int state;
} process_t;

extern int *opened_fds;
extern process_t **running_processes;

int process_start(process_t *p);
void process_init();
void process_deinit();
process_t *process_new();
void process_delete(process_t *p);
void process_set_args(process_t *p, int n, ...);
void process_wait();
#endif
