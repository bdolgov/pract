#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#define _XOPEN_SOURCE
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

int limit;

void child(int id, int* pipes)
{
	key_t semkey = ftok("/etc/passwd", 'B');
	FILE *in = fdopen(pipes[0], "r"), *out = fdopen(pipes[1], "w");

	int sem = semget(semkey, 2, IPC_CREAT | IPC_EXCL | 0777);
	if (sem != -1)
	{
//		unsigned short vals[] = {0, 0};
//		semctl(sem, 2, SETALL, vals);
	}
	else
	{
		sem = semget(semkey, 2, 0);
	}
	struct sembuf sop;
	sop.sem_flg = 0;
	if (id == 1)
	{
		fprintf(out, "1\n");
		fflush(out);
		sop.sem_num = 0;
		sop.sem_op = 1;
		semop(sem, &sop, 1);
	}

	for(;;)
	{
		sop.sem_num = id;
		sop.sem_op = -1;
		if (semop(sem, &sop, 1) == -1) return;
		int k;
		if (fscanf(in, "%d", &k) == 1)
		{
			if (k == limit)
			{
				fclose(in);
				fclose(out);
				semctl(sem, 2, IPC_RMID);
				return;
			}
			else
			{
				sop.sem_num = 1 - id;
				sop.sem_op = 1;
				semop(sem, &sop, 1);
				printf("%d %d\n", id + 1, k);
				fflush(stdout);
				fprintf(out, "%d\n", k + 1);
				fflush(out);
			}
		}
		else
		{
			break;
		}
	}
	fclose(in);
	fclose(out);
}
int main(int ac, char** av)
{
	limit = atoi(av[1]);
	int pipes[2]; pipe(pipes);
	if (fork() == 0)
	{
		child(0, pipes);
		return 0;
	}
	if (fork() == 0)
	{
		child(1, pipes);
		return 0;
	}
	close(pipes[1]);
	close(pipes[0]);
	wait(NULL);
	wait(NULL);
	printf("Done\n");
	return 0;
}
