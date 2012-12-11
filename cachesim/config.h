#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

struct config_entry_s
{
	char *name;
	char *value;
};
typedef struct config_entry_s config_entry_t;

typedef config_entry_t* config_t;

config_t config_parse(const char* filename);
void config_delete(config_t c);

const char* config_string(config_t c, const char* name);
bool config_int(config_t c, const char* name, int* value);

#endif
