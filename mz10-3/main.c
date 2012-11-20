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

void child(int n, int lim, int readfd, int writefd)
{
	int eof = 0;
	int cur = 0;
	for (;;)
	{
		cur = read_int(readfd, &eof);
		if (cur >= lim || eof) return;
		printf("%d %d\n", n, cur);
		fflush(stdout);
		++cur;
		write_int(writefd, cur);
	}
}

int main(int ac, char** av)
{
	if (ac != 2) return 1;
	int n = atoi(av[1]);
	int pipes[2][2];
	pipe(pipes[0]); /* 2r, 1w */
	pipe(pipes[1]); /* 1r, 2w */
	pid_t p1 = fork();
	if (p1 == 0)
	{
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
		child(2, n, pipes[0][0], pipes[1][1]);
		return 0;
	}
	
	write_int(pipes[1][1], 1);
	int finished = 0;
	close(pipes[0][0]);
	close(pipes[0][1]);
	close(pipes[1][0]);
	close(pipes[1][1]);
	while (finished != 2)
	{
		if (wait(NULL) > 0) ++finished;
	}
	printf("Done\n");
	return 0;
}
