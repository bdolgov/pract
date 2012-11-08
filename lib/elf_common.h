#ifndef ELF_COMMON_H
#define ELF_COMMON_H
#include <elf.h>
struct bd_file_s
{
	int fd;
	unsigned eof:1;
};
typedef struct bd_file_s bd_file_t;

bd_file_t *bd_open(const char* path, const char* flags);
int bd_close(bd_file_t* f);
int bd_fmove(bd_file_t* f, int pos);
int bd_read(bd_file_t* f, void* where, int size);
int bd_read_e(bd_file_t* f, void* where, int size);
int bd_readat_e(bd_file_t* f, void* where, int from, int size);
typedef struct ElfFile
{
	bd_file_t *f;
	Elf32_Ehdr hdr;
	Elf32_Shdr *sections;
	char *section_names;
	Elf32_Phdr *segments;
} ElfFile;

ElfFile *elf_open(const char *path);
ElfFile *elf_close(ElfFile *pelf);
const Elf32_Shdr *elf_find_section(const ElfFile *pelf, const char *section_name);
void *elf_load_section(const ElfFile *pelf, const Elf32_Shdr *ps);
void *elf_load_segment(const ElfFile *pelf, const Elf32_Phdr *ph);
Elf32_Nhdr *elf_note_begin(const Elf32_Phdr *pnote, void *pdata);
Elf32_Nhdr *elf_note_end(const Elf32_Phdr *pnote, void *pdata);
Elf32_Nhdr *elf_note_next(Elf32_Nhdr *pnote);
const char *elf_note_name(Elf32_Nhdr *pnote);
const void *elf_note_desc(Elf32_Nhdr *pnote);
#endif
