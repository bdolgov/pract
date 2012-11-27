#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int ac, char** av)
{
	if (ac != 7) return 1;
	char *pr1 = av[1], *pr2 = av[2], *pr3 = av[3];
	char *a1 = av[4], *a2 = av[5], *a3 = av[6];

	int pipes[2]; pipe(pipes);

	if (fork() == 0)
	{
		dup2(pipes[1], 1);
		close(pipes[0]);
		close(pipes[1]);
		if (fork() == 0)
		{
			execl(pr1, pr1, NULL);
			return 127;
		}
		else
		{
			wait(NULL);
			if (fork() == 0)
			{
				int fd = open(a1, O_RDONLY);
				dup2(fd, 0);
				close(fd);
				execl(pr2, pr2, NULL);
				return 127;
			}
			else
			{
				wait(NULL);
			}
		}
	}
	else
	{
		if (fork() == 0)
		{
			dup2(pipes[0], 0);
			close(pipes[0]);
			close(pipes[1]);
			int fd = creat(a3, 0777);
			dup2(fd, 1);
			close(fd);
			execl(pr3, pr3, a2, NULL);
			return 127;
		}
		else
		{
			close(pipes[0]);
			close(pipes[1]);
			wait(NULL);
		}
		wait(NULL);
	}
	return 0;
}
