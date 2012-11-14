#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/wait.h>

#include "vector.h"
#include "process.h"

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
