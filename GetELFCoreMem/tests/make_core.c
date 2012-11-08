#include <stdio.h>
int z = -1;
int main()
{
	int x = 1, y = 0xfee1dead;
	printf("%08x %08x\n", &x, x);
	printf("%08x %08x\n", &y, y);
	printf("%08x %08x\n", &z, z);
	*(int*)0=0;
}
