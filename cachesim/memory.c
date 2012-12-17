#include "memory.h"
#include <stdlib.h>
#include <string.h>

static void real_memory_free(real_memory_t* mem)
{
	free(mem->stat);
	free(mem->data);
	free(mem);
}

static void real_memory_reveal(real_memory_t* mem, unsigned int addr, int size, const char* data)
{
	for (int i = 0; i < size; ++i)
	{
		mem->data[addr + i].valid = 1;
		mem->data[addr + i].data = data[i];
	}
}

static void real_memory_read(real_memory_t* mem, unsigned int addr, int size, cell_t* data)
{
	++mem->stat->read_counter;
	mem->stat->clock_counter += mem->read_time * ((size + mem->width - 1) / mem->width);
	if (0 && data)
	{
		memcpy(data, mem->data + addr, size);
	}
}

static void real_memory_write(real_memory_t* mem, unsigned int addr, int size, cell_t* data)
{
	++mem->stat->write_counter;
	mem->stat->clock_counter += mem->write_time * ((size + mem->width - 1) / mem->width);
	if (0 && data)
	{
		memcpy(mem->data + addr, data, size);
	}
}

static void real_memory_flush(real_memory_t* mem)
{
}

real_memory_t *create_real_memory(int size, int read_time, int write_time, int width)
{
	real_memory_t *ret = calloc(sizeof(real_memory_t), 1);
	ret->stat = calloc(sizeof(statinfo_t), 1);
	ret->size = size;
	ret->read_time = read_time;
	ret->write_time = write_time;
	ret->width = width;
	ret->free = real_memory_free;
	ret->read = real_memory_read;
	ret->write = real_memory_write;
	ret->flush = real_memory_flush;
	ret->underlying = NULL;
	ret->data = calloc(size, sizeof(cell_t));
	ret->reveal = real_memory_reveal;
	return ret;
}

void real_memory_dump(real_memory_t* mem, FILE* f)
{
	for (int i = 0; i < mem->size; i += 16)
	{
		fprintf(f, "%08X", i);
		for (int j = 0; j < 16; ++j)
		{
			if (mem->data[i + j].valid) fprintf(f, " %02X", (unsigned char)mem->data[i + j].data);
			else fprintf(f, " ??");
		}
		fprintf(f, "\n");
	}
}

void statinfo_dump(statinfo_t* info, FILE* f)
{
	fprintf(f, "clock count: %d\n", info->clock_counter);
	fprintf(f, "reads: %d\n", info->read_counter);
	fprintf(f, "writes: %d\n", info->write_counter);
	if (info->hit_counter_needed) fprintf(f, "read hits: %d\n", info->hit_counter);
	if (info->write_back_counter_needed) fprintf(f, "cache block writes: %d\n", info->write_back_counter);
}
void cache_flush_block(cache_t *mem, cache_block_t *block)
{
	if (block->dirty && block->valid)
	{
		block->dirty = 0;
		if (mem->underlying) mem->underlying->write(mem->underlying, block->begin, mem->block, block->data);
		++mem->stat->write_back_counter;
	}
}


static void cache_free(cache_t* mem)
{
	free(mem->rand);
	for (int i = 0; i < mem->size / mem->block; ++i)
	{
		free(mem->blocks[i].data);
	}
	free(mem->blocks);
	if (mem->underlying) mem->underlying->free(mem->underlying);
	else free(mem->stat);
	free(mem);
}

void cache_read(cache_t* mem, unsigned int addr, int size, cell_t* data)
{
	cache_block_t *pb = NULL;
	for (int i = 0; i < size; ++i) 
	{
		cache_block_t *block = mem->find(mem, addr + i);
		if (block != pb)
		{
			pb = block;
			mem->stat->clock_counter += mem->read_time;
			if (!block->valid || block->begin != (addr + i) / mem->block * mem->block)
			{
				cache_flush_block(mem, block);
				block->valid = 1;
				block->dirty = 0;
				block->begin = (addr + i) / mem->block * mem->block;
				mem->underlying->read(mem->underlying, (addr + i) / mem->block * mem->block, mem->block, block->data);
			}
			else
			{
				++mem->stat->hit_counter;
			}
			if (data)
			{
				// FIXME
			}
		}
	}
}

void cache_reveal(cache_t* mem, unsigned int addr, int size, const char* data)
{
	if (mem->underlying) mem->underlying->reveal(mem->underlying, addr, size, data);
}

void cache_write_writethrough(cache_t* mem, unsigned int addr, int size, cell_t* data)
{
	cache_block_t* pb = NULL;
	for (int i = 0; i < size; ++i) 
	{
		cache_block_t* block = mem->find(mem, addr + i);
		if (block != pb)
		{
			pb = block;
			if (block->valid && block->begin == (addr + i) / mem->block * mem->block)
			{
				mem->stat->clock_counter += mem->write_time;
				if (data)
				{
					// FIXME
				}
			}
		}
	} 
	mem->underlying->write(mem->underlying, addr, size, data);
}

void cache_write_writeback(cache_t* mem, unsigned int addr, int size, cell_t* data)
{
	cache_block_t *pb = NULL;
	for (int i = 0; i < size; ++i)
	{
		cache_block_t *block = mem->find(mem, addr + i);
		if (block != pb)
		{
			pb = block;
			if (!block->valid && block->begin != (addr + i) / mem->block * mem->block)
			{
				cache_flush_block(mem, block);
				block->valid = 1;
				block->dirty = 0;
				block->begin = (addr + i) / mem->block * mem->block;
				mem->underlying->read(mem->underlying, (addr + i) / mem->block * mem->block, mem->block, block->data);
				if (data)
				{
					// FIXME
				}
			}
			block->dirty = 1;
		}
	}
}

cache_block_t* cache_find_direct(cache_t* mem, unsigned int addr)
{
	return mem->blocks + (addr / mem->block) % (mem->size / mem->block);
}

cache_block_t* cache_find_full(cache_t* mem, unsigned int addr)
{
	for (int i = 0; i < mem->size / mem->block; ++i)
	{
		if (mem->blocks[i].begin == addr / mem->block * mem->block && mem->blocks[i].valid) return mem->blocks + i;
	}
	return mem->blocks + mem->rand->next(mem->rand, 0, mem->size / mem->block);
}

void cache_flush(cache_t* mem)
{
	for (int i = 0; i < mem->size / mem->block; ++i)
	{
		cache_flush_block(mem, mem->blocks + i);
	}
}

memory_t *create_cache(int size, int block, int assoc, int write_st, int read_time, int write_time, memory_t* underlying, random_t* rand)
{
	cache_t *mem = calloc(sizeof(cache_t), 1);
	mem->underlying = underlying;
	if (underlying) mem->stat = underlying->stat;
	else mem->stat = calloc(sizeof(statinfo_t), 1);
	mem->free = cache_free;
	mem->rand = rand;
	mem->size = size;
	mem->block = block;
	mem->flush = cache_flush;
	mem->blocks = calloc(sizeof(cache_block_t), mem->size / mem->block);
	mem->stat->hit_counter_needed = 1;
	for (int i = 0; i < mem->size / mem->block; ++i)
	{
		mem->blocks[i].data = calloc(sizeof(cell_t), mem->block);
	}
	if (assoc == ASSOC_DIRECT)
	{
		mem->find = cache_find_direct;
	}
	else
	{
		mem->find = cache_find_full;
	}

	if (write_st == WRITE_THROUGH)
	{
		mem->write = cache_write_writethrough;
	}
	else
	{
		mem->write = cache_write_writeback;
		mem->stat->write_back_counter_needed = 1;
	}

	mem->read = cache_read;
	mem->reveal = cache_reveal;
	mem->read_time = read_time;
	mem->write_time = write_time;
	return (memory_t*)mem;
}
