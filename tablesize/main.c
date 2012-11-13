#include <stdio.h>
#include <string.h>

enum {PAGESIZE = 4096 };

int main(int ac, char** av)
{
	unsigned int sum = 0;
	unsigned int a1, a2;
	char buf[4096];
	static char table1[1024];
	while (fgets(buf, sizeof(buf), stdin))
	{
		sscanf(buf, "%x-%x", &a1, &a2);
		for (unsigned int page = a1; page <= a2; page += PAGESIZE)
		{
			table1[page >> 22] = 1;
		}
		if (buf[strlen(buf) - 1] != '\n')
		{
			while (getc(stdin) != '\n');
		}
	}
	sum += PAGESIZE;
	for (int i = 0; i < sizeof(table1); ++i)
	{
		if (table1[i]) sum += PAGESIZE;
	}

	printf("%u\n", sum);
}
