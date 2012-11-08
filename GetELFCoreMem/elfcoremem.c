#include "elf_common.h"
#include <stdio.h>


int main(int argc, char** argv)
{
	if (argc != 2) return 1;
	ElfFile *f = elf_open(argv[1]);

	
	int addr;
	while (scanf("%x", &addr) == 1)
	{
		int found = 0;
    	for (int i = 0; i < f->hdr.e_phnum; ++i) {
			if (f->segments[i].p_type == PT_LOAD &&
				f->segments[i].p_vaddr <= addr &&
				addr < f->segments[i].p_vaddr +  f->segments[i].p_memsz)
			{
				if (f->segments[i].p_filesz == f->segments[i].p_memsz &&
					f->segments[i].p_filesz)
				{
					int ret;
					bd_readat_e(f->f, &ret, f->segments[i].p_offset + (addr - f->segments[i].p_vaddr), sizeof(int));
					printf("%08X\n", ret);
				}
				else
				{
					printf("UNKNOWNVAL\n");
				}
				found = 1;
				break;
			}
		}
		if (!found)
		{
			printf("INVALIDADR\n");
		}
	}
	elf_close(f);
	return 0;
}
