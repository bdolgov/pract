#include "stab_common.h"
#include "elf_common.h"
#include "vector.h"

#include <stdio.h>

void backtrace(const char* execFileName)
{
	ElfFile *f = elf_open(execFileName);
	stabinfo_t *info = stab_load(f);

	void **ebp0, **ebp, **ebp_off = 0;
	stab_line_t *r;
	__asm__ ("movl %%ebp, %0" : "=r"(ebp0));
	printf("Stack backtrace:\n");
	for (int pass = 0; pass < 2; ++pass)
	{
		ebp = ebp0;
		do
		{
			char *eip = *(ebp + 1);
			r = stab_find(info, eip - 1);
			if (pass == 0)
			{
				ebp_off = ebp;
			}
			else
			{
				if (r->fn->begin <= eip && eip < r->fn->end)
				{
					printf("ebp 0x%08x eip 0x%08x | %s:%d: %s\n",
						(ebp_off - ebp) * sizeof(void*), (unsigned int)eip, r->fn->src->name, r->line, r->fn->name);
				}
				else
				{
					printf("ebp 0x%08x eip 0x%08x | %s:%d: %s\n",
						(ebp_off - ebp) * sizeof(void*), (unsigned int)eip, "<unknown>", 0, "<unknown>");
				}
			}
			ebp = (void**)*ebp;
		} while (!r || strcmp(r->fn->name, "main"));
	}

	stab_delete(info);
	elf_close(f);
}
/*
int g()
{
	backtrace("./backtrace");
}

int f()
{
	g(); g();
}

int main()
{
	f();
}*/
