#include "elf_common.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>


ElfFile *elf_open(const char* path)
{
	ElfFile* ret = calloc(sizeof(ElfFile), 1);
	ret->f = bd_open(path, "r");
	if (!ret->f) goto clean;

	if (!bd_read_e(ret->f, &ret->hdr, sizeof(Elf32_Ehdr))) goto clean;
	if (ret->hdr.e_ident[0] != '\x7f' ||
		ret->hdr.e_ident[1] != 'E' ||
		ret->hdr.e_ident[2] != 'L' ||
		ret->hdr.e_ident[3] != 'F') goto clean;
	if (ret->hdr.e_ident[4] != ELFCLASS32 ||
		ret->hdr.e_ident[5] != ELFDATA2LSB ||
		ret->hdr.e_ident[6] != EV_CURRENT ||
		ret->hdr.e_ident[7] != ELFOSABI_NONE ||
		ret->hdr.e_ident[8] != 0) goto clean;
	
	ret->sections = malloc(sizeof(Elf32_Shdr) * ret->hdr.e_shnum);
	if (bd_fmove(ret->f, ret->hdr.e_shoff) == -1 ||
		!bd_read_e(ret->f, ret->sections, sizeof(Elf32_Shdr) * ret->hdr.e_shnum)) goto clean;

	if (ret->hdr.e_shstrndx)
	{
		ret->section_names = elf_load_section(ret, ret->sections + ret->hdr.e_shstrndx);
		if (!ret->section_names) goto clean;
	}
	else
	{
		ret->section_names = malloc(0);
	}

	ret->segments = malloc(sizeof(Elf32_Phdr) * ret->hdr.e_phnum);
	if (bd_fmove(ret->f, ret->hdr.e_phoff) == -1 ||
		!bd_read_e(ret->f, ret->segments, sizeof(Elf32_Phdr) * ret->hdr.e_phnum)) goto clean;

	return ret;

clean:
	if (ret->f) bd_close(ret->f);
	if (ret->sections) free(ret->sections);
	if (ret->section_names) free(ret->section_names);
	if (ret->segments) free(ret->segments);
	free(ret);
	return NULL;
}

ElfFile *elf_close(ElfFile *pelf)
{
	bd_close(pelf->f);
	free(pelf->sections);
	free(pelf->section_names);
	free(pelf->segments);
	free(pelf);
	return NULL;
}

void* elf_load_section(const ElfFile *pelf, const Elf32_Shdr *ps)
{
	if (!ps || !pelf) return NULL;
	void *ret = malloc(ps->sh_size);
	if (!ret) return NULL;

	if (bd_fmove(pelf->f, ps->sh_offset) == -1 ||
		!bd_read_e(pelf->f, ret, ps->sh_size)) {
		free(ret);
		return NULL;
	}
	return ret;
}

const Elf32_Shdr *elf_find_section(const ElfFile *pelf, const char *section_name)
{
	if (!pelf || !section_name) return NULL;
	for (int i = 0; i < pelf->hdr.e_shnum; ++i) {
		if (!strcmp(pelf->section_names + pelf->sections[i].sh_name, section_name))
			return pelf->sections + i;
	}
	return NULL;
}

void *elf_load_segment(const ElfFile *pelf, const Elf32_Phdr *ph)
{
	if (!pelf || !ph) return NULL;
	void *ret = malloc(ph->p_filesz);
	if (!ret) return NULL;

	if (bd_fmove(pelf->f, ph->p_offset) == -1 ||
		!bd_read_e(pelf->f, ret, ph->p_filesz)) {
		free(ret);
		return NULL;
	}
	return ret;
}

Elf32_Nhdr *elf_note_begin(const Elf32_Phdr *pnote, void *pdata)
{
	return pdata;
}

Elf32_Nhdr *elf_note_end(const Elf32_Phdr *pnote, void *pdata)
{
	return (Elf32_Nhdr*)((char*)pdata + pnote->p_filesz);
}

static int align4(int x)
{
	switch (x & 3) {
		case 0:
			return x;
		case 1:
			return x + 3;
		case 2:
			return x + 2;
		case 3:
			return x + 1;
		default:
			return -1;
	}
}

Elf32_Nhdr *elf_note_next(Elf32_Nhdr *pnote)
{
	return (Elf32_Nhdr*)((char*)pnote + sizeof(Elf32_Nhdr) +
		align4(pnote->n_namesz) +
		align4(pnote->n_descsz));
}

const char *elf_note_name(Elf32_Nhdr *pnote)
{
	return (char*)pnote + sizeof(Elf32_Nhdr);
}

const void *elf_note_desc(Elf32_Nhdr *pnote)
{
	return (char*)pnote + sizeof(Elf32_Nhdr) +
		align4(pnote->n_namesz);
}

bd_file_t *bd_open(const char* path, const char* flags)
{
	bool r = false, w = false, c = false, a = false;
	int open_flags = 0;
	for (const char* i = flags; *i; ++i) {
		if (*i == 'r') r = true;
		else if (*i == 'w') w = true;
		else if (*i == 'c') c = true;
		else if (*i == 'a') a = true;
	}
	
	if (r && w) open_flags = O_RDWR;
	else if (w) open_flags = O_WRONLY;
	else open_flags = O_RDONLY;

	if (c) open_flags |= O_CREAT;
	if (a) open_flags |= O_APPEND;

	int fd = open(path, open_flags);
	if (fd == -1) return NULL;

	bd_file_t *ret = calloc(1, sizeof(bd_file_t));
	ret->fd = fd;
	return ret;
}

int bd_close(bd_file_t* f)
{
	int rc = close(f->fd);
	free(f);
	return rc;
}

int bd_fmove(bd_file_t* f, int pos)
{
	int rc = lseek(f->fd, pos, SEEK_SET);
	if (rc != -1) f->eof = 0;
	return rc;
}

int bd_read(bd_file_t* f, void* where, int size)
{
	int rd = 0;
	while (rd != size)
	{
		int rc = read(f->fd, (char*)where + rd, size - rd);
		if (rc == 0) {
			f->eof = 1;
			return rd;
		}
		else if (rc == -1 && errno != EINTR) {
			return -1;
		}
		else {
			rd += rc;
		}
	}
	return rd;
}

int bd_read_e(bd_file_t* f, void* where, int size)
{
	return bd_read(f, where, size) == size;
}

int bd_readat_e(bd_file_t* f, void* where, int from, int size)
{
	return bd_fmove(f, from) != -1 && bd_read_e(f, where, size);
}
