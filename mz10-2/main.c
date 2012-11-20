#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>

struct tm* read_time(int fd)
{
	time_t t;
	read(fd, &t, sizeof(time_t));
	return localtime(&t);
}

int main(void)
{
	time_t t = time(NULL);
	int pipes[2]; pipe(pipes);
	pid_t child1 = fork();
	if (child1 == 0)
	{
		close(pipes[1]);
		struct tm* tm;
		pid_t child2 = fork();
		if (child2 == 0)
		{
			pid_t child3 = fork();
			if (child3 == 0)
			{
				/* in child3 */
				tm = read_time(pipes[0]);
				printf("S:%02d\n", tm->tm_sec);
			}
			else
			{
				/* in child2 */
				while (wait(NULL) != child3);
				tm = read_time(pipes[0]);
				printf("M:%02d\n", tm->tm_min);
			}
		}
		else
		{
			/* in child1 */
			while (wait(NULL) != child2);
			tm = read_time(pipes[0]);
			printf("H:%02d\n", tm->tm_hour);
		}
		close(pipes[0]);
	}
	else
	{
		/* in parent */
		close(pipes[0]);
		for (int i = 0; i < 3; ++i)
		{
			write(pipes[1], &t, sizeof(time_t));
		}
		while (wait(NULL) != child1);
		close(pipes[1]);
	}
	return 0;
}
