#include "util.h"
#include "vector.h"
#include "config.h"
#include "memory.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void do_print_config(config_t config)
{
	for (int i = 0; i < vector_size(config); ++i)
	{
		printf("%s = \"%s\"\n", config[i].name, config[i].value);
	}
}

real_memory_t* configure_real_memory(config_t config)
{
	int size, read_time, write_time, width;
	if (!config_int(config, "memory_size", &size) ||
		!config_int(config, "memory_read_time", &read_time) ||
		!config_int(config, "memory_write_time", &write_time) ||
		!config_int(config, "memory_width", &width))
	{
		return NULL;
	}

	return create_real_memory(size, read_time, write_time, width);
}

memory_t* configure_memory(config_t config, memory_t *underlying)
{
	int size, block, assoc, replacement_st, write_st;
	int read_time, write_time;
	int seed;
	char *_replacement_st, *_write_st, *_assoc;
	if (!config_int(config, "cache_size", &size) ||
		!config_int(config, "cache_block", &block) ||
		!config_int(config, "cache_read_time", &read_time) ||
		!config_int(config, "cache_write_time", &write_time) ||
		!config_int(config, "seed", &seed) || 
		!(_replacement_st = config_string(config, "replacement_strategy")) ||
		!(_write_st = config_string(config, "write_strategy")) ||
		!(_assoc = config_string(config, "associativity")))
	{
		return NULL;
	}

	return create_cache(size, block, strcmp(_assoc, "full") ? ASSOC_DIRECT : ASSOC_FULL,
		strcmp(_write_st, "write-back") ? WRITE_THROUGH : WRITE_BACK,
		read_time, write_time, underlying, random_new_rand(seed));

}

char *convert(long long val, int size)
{
	char *bytes = (char*)&val;
	static char ret[8];
	for (int i = 0; i < size; ++i)
	{
		ret[i] = bytes[size - 1 - i];
	}
	return ret;
}

int main(int ac, char** av)
{
	char *config_name = NULL;
	bool print_config = false, dump_memory = false;
	bool statistics = false, disable_cache = false;

	for (int i = 1; i < ac; ++i)
	{
		if (!strcmp(av[i], "--print-config"))
		{
			print_config = true;
		}
		else if (!strcmp(av[i], "--dump-memory"))
		{
			dump_memory = true;
		}
		else if (!strcmp(av[i], "--statistics"))
		{
			statistics = true;
		}
		else if (!strcmp(av[i], "--disable-cache"))
		{
			disable_cache = true;
		}
		else if (av[i][0] == '-' && av[i][1] == '-')
		{
			report_error("Invalid arguments");
		}
		else if (!config_name)
		{
			config_name = av[i];
		}
		else
		{
			report_error("Invalid arguments");
		}
	}
	if (!config_name) report_error("Invalid arguments");

	config_t config = config_parse(config_name);

	if (print_config)
	{
		do_print_config(config);
		goto cleanup;
	}

	real_memory_t *real_memory = configure_real_memory(config);
	memory_t *memory = disable_cache ? (memory_t*)real_memory : configure_memory(config, real_memory);

	if (!real_memory)
	{
		config_delete(config);
		return 1;
	}
	if (!memory)
	{
		real_memory->free(real_memory);
		config_delete(config);
		return 1;
	}
	
	traceop_t *op;
	while ((op = read_trace(stdin)))
	{
		if (op->op == TRACE_READ)
		{
			memory->read(memory, op->addr, op->size, NULL);
		}
		else 
		{
			memory->write(memory, op->addr, op->size, NULL);
		}
		memory->reveal(memory, op->addr, op->size, convert(op->value, op->size));
	}
	memory->flush(memory);
	if (dump_memory)
	{
		real_memory_dump(real_memory, stdout);
	}
	if (statistics)
	{
		statinfo_dump(memory->stat, stdout);
	}
	memory->free(memory);
cleanup:
	config_delete(config);
}
