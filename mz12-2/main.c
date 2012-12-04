#include <unistd.h>
#include <wait.h>

int main(int ac, char** av)
{
	int pipes[2];
	for (int i = ac - 1; i != 0; --i)
	{
		pipe(pipes);
		if (fork() == 0)
		{
			if (i != 1)
			{
				dup2(pipes[0], 0);
			}
			close(pipes[0]);
			close(pipes[1]);
			execlp(av[i], av[i], NULL);
		}
		else
		{
			dup2(pipes[1], 1);
			close(pipes[0]);
			close(pipes[1]);
		}
	}
	
	for (int i = 1; i < ac; ++i) wait(NULL);
	
	return 0;
}
