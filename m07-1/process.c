#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/wait.h>

#include "vector.h"

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

int *opened_fds;
process_t **running_processes;

int process_start(process_t *p)
{
	pid_t pid = fork();
	if (pid == -1) return -1;
	else if (!pid)
	{
		for (int i = 0; i < vector_size(p->redirs); ++i)
		{
			dup2(p->redirs[i].f, p->redirs[i].fd);
		}
		for (int i = 0; i < vector_size(opened_fds); ++i)
		{
			close(opened_fds[i]);
		}
		execv(p->args[0], p->args);
		exit(255);
	}
	p->pid = pid;
	vector_push_back(running_processes, p);
	return pid;
}

void process_init()
{
	opened_fds = vector_new(int);
	running_processes = vector_new(process_t*);
}

void process_deinit()
{
	vector_delete(opened_fds);
	vector_delete(running_processes);
}

process_t *process_new()
{
	process_t *p = calloc(sizeof(process_t), 1);
	p->state = STOPPED;
	p->redirs = vector_new(ioredirect_t);
	p->args = vector_new(char*);
	return p;
}

void process_delete(process_t *p)
{
	vector_delete(p->redirs);
	vector_delete(p->args);
	free(p);
}

void process_set_args(process_t *p, int n, ...)
{
	va_list ap;
	va_start(ap, n);
	for (int i = 0; i < n; ++i)
	{
		char **x = va_arg(ap, char**);
		vector_push_back(p->args, x);
	}
	va_end(ap);
	char **x = NULL;
	vector_push_back(p->args, x);
}

void process_wait()
{
	pid_t p = wait(NULL);
	if (p != -1)
	{
		for (int i = 0; i < vector_size(running_processes); ++i)
		{
			if (running_processes[i]->pid == p)
			{
				running_processes[i]->state = EXITED;
				vector_remove(running_processes, i);
				break;
			}
		}
	}
}

int main(int ac, char** av)
{
	process_init();

	int pipes[2]; 
	pipe(pipes);

	int a1 = open(av[4], O_RDONLY);
	int a3 = creat(av[6], 0644);
	vector_push_back(opened_fds, pipes[0]);
	vector_push_back(opened_fds, pipes[1]);
	vector_push_back(opened_fds, a1);
	vector_push_back(opened_fds, a3);

	process_t *pr1 = process_new(), *pr2 = process_new(), *pr3 = process_new();
	process_set_args(pr1, 1, av[1]);
	process_set_args(pr2, 1, av[2]);
	process_set_args(pr3, 2, av[3], av[5]);
	
	ioredirect_t r;
	r.f = pipes[1];
	r.fd = 1;
	vector_push_back(pr1->redirs, r);
	vector_push_back(pr2->redirs, r);
	r.f = a3;
	vector_push_back(pr3->redirs, r);
	r.f = pipes[0];
	r.fd = 0;
	vector_push_back(pr3->redirs, r);
	r.f = a1;
	vector_push_back(pr2->redirs, r);

	process_start(pr1);
	process_start(pr3);

	while (pr1->state != EXITED)
	{
		process_wait();
	}
	process_start(pr2);
	close(a1); close(a3);
	close(pipes[0]); close(pipes[1]);
	while (pr2->state != EXITED || pr3->state != EXITED)
	{
		process_wait();
	}
	process_delete(pr1);
	process_delete(pr2);
	process_delete(pr3);
	process_deinit();
	return 0;
}
