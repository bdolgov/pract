#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	unsigned hit_counter_needed:1;
	unsigned write_back_counter_needed:1;
	int clock_counter;
	int read_counter;
	int write_counter;
	int hit_counter;
	int write_back_counter;
} statinfo_t;

void statinfo_dump(statinfo_t* info, FILE* f);

typedef struct memory_s
{
	statinfo_t stat;
	void (*free)(struct memory_s* mem);
	void (*read)(struct memory_s* mem, unsigned int addr, int size, const char* data);
	void (*write)(struct memory_s* mem, unsigned int addr, int size, const char* data);
	void (*reveal)(struct memory_s* mem, unsigned int addr, int size, const char* data);
	void (*flush)(struct memory_s* mem);
	struct memory_s *underlying;
} memory_t;

typedef struct real_memory_s
{
	statinfo_t stat;
	void (*free)(struct real_memory_s* mem);
	void (*read)(struct real_memory_s* mem, unsigned int addr, int size, const char* data);
	void (*write)(struct real_memory_s* mem, unsigned int addr, int size, const char* data);
	void (*reveal)(struct real_memory_s* mem, unsigned int addr, int size, const char* data);
	void (*flush)(struct real_memory_s* mem);
	struct memory_s *underlying;
	char *data;
	char *used;
	int size, read_time, write_time, width;
} real_memory_t;

real_memory_t *create_real_memory(int size, int read_time, int write_time, int width);
void real_memory_dump(real_memory_t* mem, FILE* f);

#endif
