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

int main()
{
       key_t key = ftok("/usr/bin/shell", '1');
        int msg = msgget(key, IPC_CREAT | 0777);
        msgctl(msg, IPC_RMID, NULL);
        msg = msgget(key, IPC_CREAT | 0777);
        for (int j = 0; j < 10; ++j)
        {
            for (int i = 1; i <= 10; ++i)
            {
                msg_t m;
                m.author = i;
                m.txt[0] = 'a' + j;
                m.txt[1] = 0;
                msgsnd(msg, &m, sizeof(long) + 2, 0);
            }
        }
	char c; scanf("%c", &c);
    {
        key_t key = ftok("/usr/bin/shell", '1');
        int msg = msgget(key, IPC_CREAT | 0777);
        msg_t m;
        while (msgrcv(msg, &m, sizeof(msg_t), 0, IPC_NOWAIT) != -1)
        {
            printf("%d %c\n", (int)m.author, m.txt[0]);
        }
    }
}
