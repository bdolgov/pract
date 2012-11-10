#include "stab_common.h"
#include "elf_common.h"
#include "vector.h"

#include <stab.h>

typedef struct Stab
{
	uint32_t n_strx;
	uint8_t n_type;
	uint8_t n_other;
	uint16_t n_desc;
	uintptr_t n_value;
} Stab;

stabinfo_t *stab_load(struct ElfFile* f)
{
	const Elf32_Shdr *stabsect = elf_find_section(f, ".stab");
	if (!stabsect) return NULL;
	stabinfo_t *s = malloc(sizeof(stabinfo_t));
	s->str = elf_load_section(f, elf_find_section(f, ".stabstr"));
	Stab *stab = elf_load_section(f, stabsect);
	int n = stabsect->sh_size / sizeof(Stab);
	
	int src = -1, fn = -1;
	s->srcs = vector_new(stab_src_t);
	s->fns = vector_new(stab_fn_t);
	s->lines = vector_new(stab_line_t);
	
	int next_offset = 0;

	char *stabstr = s->str;

	for (int i = 0; i < n; ++i)
	{
		if (stab[i].n_type == N_SO && *(stabstr + stab[i].n_strx))
		{
			stab_src_t new_src;
			new_src.name = stabstr + stab[i].n_strx;
			src = vector_push_back_i(s->srcs, new_src);
		}
		else if (stab[i].n_type == N_SO)
		{
			if (fn != -1) s->fns[fn].end = (char*)stab[i].n_value;
			fn = -1;
			src = -1;
		}
		else if (stab[i].n_type == N_FUN)
		{
			if (fn != -1) s->fns[fn].end = (char*)stab[i].n_value;
			stab_fn_t new_fn;
			new_fn.begin = (char*)stab[i].n_value;
			new_fn.src = src;
			new_fn.name = stabstr + stab[i].n_strx;
			char *delim = strchr(new_fn.name, ':');
			if (delim) *delim = 0;
			fn = vector_push_back_i(s->fns, new_fn);
		}
		else if (stab[i].n_type == N_SLINE)
		{
			stab_line_t new_line;
			new_line.fn = fn;
			new_line.line = stab[i].n_desc;
			new_line.begin = stab[i].n_value + s->fns[fn].begin;
			vector_push_back(s->lines, new_line);
		}
		else if (stab[i].n_type == 0)
		{
			stabstr = s->str + next_offset;
			next_offset = stab[i].n_value;
		}
	}

	free(stab);

	return s;
}

void stab_delete(stabinfo_t* s)
{
	vector_delete(s->srcs);
	vector_delete(s->fns);
	vector_delete(s->lines);
	free(s->str);
	free(s);
}

stab_line_t *stab_find(stabinfo_t* s, char* addr)
{
	int l = 0, r = vector_size(s->lines) - 1;
	while (l != r)
	{
		int c = (l + r) / 2;
		if (s->lines[c].begin >= addr) r = c;
		else l = c + 1;
	}
	return s->lines + (s->lines[l].begin == addr || !l ? l : l - 1);
}
