#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int pipes[2];
pid_t friend_pid;
FILE *pin, *pout;
int n;
int my_id;

int signals_arrived = 0;

void call_die(int unused)
{
	die();
}

void die()
{
	fclose(pin);
	fclose(pout);
	exit(0);
}

void hndl(int unused)
{
	fprintf(stderr, "%d hndl\n", my_id);
	signal(SIGUSR1, hndl);
	int cur;
	if (fscanf(pin, "%d", &cur) != 1) die(); //Никогда не случится
	if (friend_pid == -1)
	{
		friend_pid = cur;
		cur = 1;
	}
	if (cur == n)
	{
		fprintf(stderr, "%d exiting\n", my_id);
		kill(friend_pid, SIGTERM);
		die();
	}
	printf("%d %d\n", my_id, cur);
	fflush(stdout);
	fprintf(pout, "%d\n", cur + 1);
	fflush(pout);
	fprintf(stderr, "%d beforekill\n", my_id);
	kill(friend_pid, SIGUSR1);
	fprintf(stderr, "%d afterkill\n", my_id);
}

void child(int n)
{
	my_id = n;
	for (;;) sleep(1);
}

int main(int ac, char** av)
{
	if (ac != 2) return 1;
	n = atoi(av[1]);
	pipe(pipes);
	pin = fdopen(pipes[0], "r");
	pout = fdopen(pipes[1], "w");
	signal(SIGUSR1, hndl);
	signal(SIGTERM, call_die);
	pid_t p1 = fork();
	if (p1 == 0)
	{
		friend_pid = -1;
		child(1);
		return 0;
	}
	pid_t p2 = fork();
	if (p2 == 0)
	{
		friend_pid = p1;
		child(2);
		return 0;
	}
	/* Вместо "1" первому процессу приходит PID второго процесса.
	 * Доставлять первому процессу единицу через pipe невозможно,
	 * так как порядок доставки сигналов не гарантируется,
	 * а обработка сигнала может быть прервана новым сигналом. */
	fprintf(pout, "%d\n", (int)p2);
	fflush(pout);
	kill(p1, SIGUSR1);
	fclose(pin); fclose(pout);
	int finished = 0;
	while (finished != 2)
	{
		if (wait(NULL) > 0) ++finished;
	}
	printf("Done\n");
	return 0;
}
