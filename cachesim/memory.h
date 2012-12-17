#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

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

typedef struct cell_s
{
	unsigned valid:1;
	unsigned char data;
} cell_t;


typedef struct memory_s
{
	statinfo_t *stat;
	void (*free)(struct memory_s* mem);
	void (*read)(struct memory_s* mem, unsigned int addr, int size, cell_t* data);
	void (*write)(struct memory_s* mem, unsigned int addr, int size, cell_t* data);
	void (*reveal)(struct memory_s* mem, unsigned int add, int size, const char* data);
	void (*flush)(struct memory_s* mem);
	struct memory_s *underlying;
} memory_t;

typedef struct real_memory_s
{
	statinfo_t *stat;
	void (*free)(struct real_memory_s* mem);
	void (*read)(struct real_memory_s* mem, unsigned int addr, int size, cell_t* data);
	void (*write)(struct real_memory_s* mem, unsigned int addr, int size, cell_t* data);
	void (*reveal)(struct real_memory_s* mem, unsigned int add, int size, const char* data);
	void (*flush)(struct real_memory_s* mem);
	struct memory_s *underlying;
	cell_t *data;
	char *used;
	int size, read_time, write_time, width;
} real_memory_t;

real_memory_t *create_real_memory(int size, int read_time, int write_time, int width);
void real_memory_dump(real_memory_t* mem, FILE* f);

enum { CACHE_VALID = 1, CACHE_DIRTY = 2 };

typedef struct
{
	unsigned valid:1;
	unsigned dirty:1;
	unsigned int begin;
	cell_t *data;
} cache_block_t;

typedef struct cache_s
{
    statinfo_t *stat;
    void (*free)(struct cache_s* mem);
    void (*read)(struct cache_s* mem, unsigned int addr, int size, cell_t* data);
    void (*write)(struct cache_s* mem, unsigned int addr, int size, cell_t* data);
	void (*reveal)(struct cache_s* mem, unsigned int addr, int size, const char* data);
    void (*flush)(struct cache_s* mem);
    struct memory_s *underlying;
	cache_block_t* blocks;
	int size, block;
	int read_time, write_time;
	random_t *rand;
	cache_block_t* (*find)(struct cache_s*, unsigned int addr);
} cache_t;

enum { ASSOC_DIRECT = 0, ASSOC_FULL = 1, WRITE_BACK = 0, WRITE_THROUGH = 1 };
memory_t *create_cache(int size, int block, int assoc, int write_st, int read_time, int write_time, memory_t* underlying, random_t* rand);

#endif
