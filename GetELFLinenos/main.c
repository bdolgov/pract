#include "elf_common.h"
#include "stab_common.h"
#include <stdio.h>
int main(int argc, char** argv)
{
	ElfFile* f = elf_open(argv[1]);
	stabinfo_t *s = stab_load(f);
	if (!s)
	{
		printf("No debug info");
		return 0;
	}

	char* addr;
	while (scanf("%x", (unsigned int*)&addr) == 1)
	{
		stab_line_t *r = stab_find(s, (char*)addr);
		if (r->fn->begin <= addr && addr < r->fn->end)
		{
			printf("0x%08x:%s:%s:%x:%d\n",
				(unsigned int)addr, r->fn->src->name, r->fn->name, addr - r->fn->begin, r->line);
		}
		else
		{
			printf("0x%08x::::\n", (unsigned int)addr);
		}
	}

	stab_delete(s);
	elf_close(f);
}
