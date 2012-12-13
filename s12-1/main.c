#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { N = 10, M = 5 };

typedef struct 
{
	long author;
	char txt[81];
} msg_t;

static int cmp_msg(const void *p1, const void* p2)
{
	const msg_t *a = p1, *b = p2;
	return strcmp(b->txt, a->txt);
}

void child(int i)
{
	key_t key = ftok("/usr/bin/shell", '1');
	int msg = msgget(key, 0);
	int alloc = 1, size = 0;
	msg_t *messages = malloc(sizeof(msg_t));
	
	while (msgrcv(msg, messages + size, sizeof(msg_t), i, IPC_NOWAIT) != -1)
	{
		if (size == alloc)
		{
			alloc *= 2;
			messages = realloc(messages, alloc * sizeof(msg_t));
		}
	}
	qsort(messages, size, sizeof(msg_t), cmp_msg);
	for (int j = 0; j < size; ++j)
	{
		msgsnd(msg, messages + j, sizeof(long) + strlen(messages[j].txt) + 1, 0);
	}
	free(messages);
}

int main(int ac, char **av)
{
	for (int i = 0; i < M; ++i)
	{
		if (!fork())
		{
			child(2 * i + 1);
			child(2 * i + 2);
			return 0;
		}
	}
	for (int i = 0; i < M; ++i)
	{
		wait(NULL);
	}
	return 0;
}
