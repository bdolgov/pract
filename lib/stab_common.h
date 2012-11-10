#ifndef STABS_COMMON_H
#define STABS_COMMON_H

typedef struct stab_src_s
{
	char *name;
} stab_src_t;

typedef struct stab_fn_s
{
	char *name;
	char *begin;
	char *end;
	int src;
} stab_fn_t;

typedef struct stab_line_s
{
	char *begin;
	int line;
	int fn;
} stab_line_t;

typedef struct stabinfo_s
{
	stab_src_t *srcs;
	stab_fn_t *fns;
	stab_line_t *lines;
	char *str;
} stabinfo_t;

struct ElfFile;

stabinfo_t *stab_load(struct ElfFile* f);
void stab_delete(stabinfo_t* s);
stab_line_t *stab_find(stabinfo_t* s, char* addr);
#endif
