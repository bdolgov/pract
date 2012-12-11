#include <stdlib.h>
#include "util.h"
#include "vector.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

void do_print_config(config_t config)
{
	for (int i = 0; i < vector_size(config); ++i)
	{
		printf("%s = \"%s\"\n", config[i].name, config[i].value);
	}
}

int main(int ac, char** av)
{
	char *config_name = NULL;
	bool print_config = false;

	for (int i = 1; i < ac; ++i)
	{
		if (!strcmp(av[i], "--print-config"))
		{
			print_config = true;
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
	}
	else
	{
	}
}
