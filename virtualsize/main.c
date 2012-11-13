#include <stdio.h>
#include <string.h>

int main(int ac, char** av)
{
	unsigned int sum = 0;
	unsigned int a1, a2;
	char buf[4096];
	while (fgets(buf, sizeof(buf), stdin))
	{
		sscanf(buf, "%x-%x", &a1, &a2);
		sum += a2 - a1;
		if (buf[strlen(buf) - 1] != '\n') while (getc(stdin) != '\n');
	}
	printf("%u\n", sum);
}
