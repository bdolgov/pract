#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int ac, char** av)
{
	if (ac != 4) return 1;
	int m = atoi(av[2]);
	long long n = atoll(av[3]);
	int pipes[2]; pipe(pipes);
	if (fork() == 0)
	{
		/* ch 1*/
		close(pipes[1]);
		int son_pid;
		FILE *pin = fdopen(pipes[0], "r");
		fscanf(pin, "%d", &son_pid);
		long long sum = 0; int t;
		while (fscanf(pin, "%d", &t) == 1)
		{
			sum += t;
			if (sum > n)
			{
				printf("-1\n");
				kill((pid_t)son_pid, SIGTERM);
				fclose(pin);
				return 0;
			}
		}
		printf("%lld\n", sum);
		fflush(stdout);
		fclose(pin);
		return 0;
	}
	if (fork() == 0)
	{
		/* ch 2 */
		close(pipes[0]);
		if (fork() == 0)
		{
			/* ch 3 */
			FILE *in = fopen(av[1], "r");
			FILE *pout = fdopen(pipes[1], "w");
			fprintf(pout, "%d\n", getpid());
			fflush(pout);
			int t;
			while (fscanf(in, "%d", &t) == 1)
			{
				t = ((long long)t * t) % m;
				fprintf(pout, "%d\n", t);
				fflush(pout);
			}
			fclose(in);
			fclose(pout);
			return 0;
		}
		return 0;
		wait(NULL);
	}
	close(pipes[0]);
	close(pipes[1]);
	wait(NULL);
	wait(NULL);
	printf("0\n");
	return 0;
}
