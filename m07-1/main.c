#include "vector.h"
#include "process.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

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
