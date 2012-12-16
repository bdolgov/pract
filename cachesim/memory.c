#include "memory.h"

#define SET_BIT(C, B) (C[(B) / 8] |= 1 << ((B) % 8))
#define GET_BIT(C, B) (C[(B) / 8] >> ((B) % 8) & 1)
static void real_memory_free(real_memory_t* mem)
{
	free(mem->data);
	free(mem->used);
	free(mem);
}

static void real_memory_reveal(real_memory_t* mem, unsigned int addr, int size, const char* data)
{
	for (int i = 0; i < size; ++i)
	{
		SET_BIT(mem->used, addr + i);
		mem->data[addr + i] = data[size - 1 - i];
	}
}

static void real_memory_read(real_memory_t* mem, unsigned int addr, int size, const char* data)
{
	real_memory_reveal(mem, addr, size, data);
	++mem->stat.read_counter;
	mem->stat.clock_counter += mem->read_time * ((size + mem->width - 1) / mem->width);
}

static void real_memory_write(real_memory_t* mem, unsigned int addr, int size, const char* data)
{
	real_memory_reveal(mem, addr, size, data);
	++mem->stat.write_counter;
	mem->stat.clock_counter += mem->write_time * ((size + mem->width - 1) / mem->width);
}

static void real_memory_flush(real_memory_t* mem)
{
}

real_memory_t *create_real_memory(int size, int read_time, int write_time, int width)
{
	real_memory_t *ret = calloc(sizeof(real_memory_t), 1);
	ret->size = size;
	ret->read_time = read_time;
	ret->write_time = write_time;
	ret->width = width;
	ret->free = real_memory_free;
	ret->read = real_memory_read;
	ret->write = real_memory_write;
	ret->reveal = real_memory_reveal;
	ret->flush = real_memory_flush;
	ret->underlying = NULL;
	ret->data = malloc(size);
	ret->used = calloc(size / 8, 1);
	return ret;
}

void real_memory_dump(real_memory_t* mem, FILE* f)
{
	for (int i = 0; i < mem->size; i += 16)
	{
		fprintf(f, "%08X", i);
		for (int j = 0; j < 16; ++j)
		{
			if (GET_BIT(mem->used, i + j)) fprintf(f, " %02X", (unsigned char)mem->data[i + j]);
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
