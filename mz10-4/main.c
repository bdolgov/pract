#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/types.h>

void write_int(int fd, int n)
{
	char buf[sizeof("2147483647")];
	sprintf(buf, "%d", n);
	write(fd, buf, strlen(buf) + 1);
}

int read_int(int fd, int *eof)
{
	char buf[sizeof("2147483647")];
	char c = 0xff;
	char *i = buf;
	*eof = 0;
	while (read(fd, &c, 1) == 1)
	{
		*(i++) = c;
		if (!c) break;
	}
	if (c)
	{
		*eof = 1;
		return 0;
	}
	else
	{
		int ret;
		sscanf(buf, "%d", &ret);
		return ret;
	}
}

int pipes[2];
pid_t friend_pid;
int n;
int my_id;

void hndl(int unused)
{
	int eof;
	int cur = read_int(pipes[0], &eof);
	if (cur == n || eof) exit(0);
	printf("%d %d\n", my_id, cur);
	fflush(stdout);
	write_int(pipes[1], cur + 1);
	kill(friend_pid, SIGUSR1);
	signal(SIGUSR1, hndl);
}

void set_friend(int unused)
{
	int eof = 0;
	friend_pid = read_int(pipes[0], &eof);
}

void child(int n, int readfd, int writefd)
{
	my_id = n;
	for (;;) sleep(1);
}

int main(int ac, char** av)
{
	if (ac != 2) return 1;
	n = atoi(av[1]);
	pipe(pipes);
	pid_t p1 = fork();
	signal(SIGUSR1, hndl);
	signal(SIGUSR2, set_friend);
	if (p1 == 0)
	{
		fried_pid = -1;
		close(pipes[0][0]);
		close(pipes[1][1]);
		child(1, n, pipes[1][0], pipes[0][1]);
		return 0;
	}
	pid_t p2 = fork();
	if (p2 == 0)
	{
		close(pipes[1][0]);
		close(pipes[0][1]);
		friend_pid = -1;
		child(2, n, pipes[0][0], pipes[1][1]);
		return 0;
	}
	write_int(pipes[1], p1);
	kill(SIGUSR2, p1);
	write_int(pipes[1], 1);
	kill(SIGUSR1, p1);
	int finished = 0;
	close(pipes[0]);
	close(pipes[1]);
	while (finished != 2)
	{
		if (wait(NULL) > 0) ++finished;
	}
	printf("Done\n");
	return 0;
}
