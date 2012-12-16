#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "vector.h"
#include "util.h"

enum { MAX_LINE_LEN = 1000 };


static void strtrim(char** begin, char** end)
{
	while (**begin == ' ' && *begin != *end) ++*begin;
	while (isspace(*(*end - 1)) && *begin != *end) --*end;
}

static int cmp_entries(const void* p1, const void* p2)
{
	return strcmp(((const config_entry_t*)p1)->name, ((const config_entry_t*)p2)->name);
}

config_t config_parse(const char* filename)
{
#define cleanup() do { fclose(in); config_delete(ret); } while (0)

	FILE *in = fopen(filename, "r");
	if (!in) report_error("Failed to open %s for reading", filename);

	config_t ret = vector_new(config_entry_t);
	
	static char buf[MAX_LINE_LEN + 1];
	int cur_line = 1;
	while (fgets(buf, sizeof(buf), in))
	{
		char *gbegin = buf;
		char *gend = buf + strlen(buf);
		if (*(gend - 1) == '\n') --gend;
		strtrim(&gbegin, &gend);
		if (gbegin == gend || *gbegin == '#') { ++cur_line; continue; }
		char *nbegin = buf;
		char *nend = strchr(buf, '=');
		char *vbegin = nend + 1;
		char *vend = strchr(buf, '#');
		if (!vend) vend = gend;

		if (!nend || vbegin >= vend || nbegin >= nend)
		{
			cleanup(); report_error("Syntax error in line %d of %s", cur_line, filename);
		}
		strtrim(&nbegin, &nend);
		strtrim(&vbegin, &vend);
		
		if (nbegin == nend || vbegin == vend || (!isalpha(*nbegin) && *nbegin != '_'))
		{
			cleanup(); report_error("Syntax error in line %d of %s", cur_line, filename);
		}
		for (char *j = nbegin; j != nend; ++j)
		{
			if (!isalpha(*j) && *j != '-' && *j != '_' && !isdigit(*j))
			{
				cleanup(); report_error("Syntax error in line %d of %s", cur_line, filename);
			}
		}
		config_entry_t entry;
		entry.name = malloc((nend - nbegin) + (vend - vbegin) + 2);
		memcpy(entry.name, nbegin, nend - nbegin);
		entry.name[nend - nbegin] = 0;
		entry.value = entry.name + (nend - nbegin) + 1;
		memcpy(entry.value, vbegin, vend - vbegin);
		entry.value[vend - vbegin] = 0;

		vector_push_back(ret, entry);

		++cur_line;
	}

	qsort(ret, vector_size(ret), sizeof(*ret), cmp_entries);
	for (int i = 1; i < vector_size(ret); ++i)
	{
		if (!strcmp(ret[i].name, ret[i - 1].name))
		{
			report_error2(-1, "Duplicate parameter %s in %s", ret[i].name, filename);
			cleanup();
			exit(1);
		}
	}

	fclose(in);
	return ret;
#undef cleanup
}

const char* config_string(config_t c, const char* name)
{
	config_entry_t x = { (char*)name, NULL };
	config_entry_t *r = bsearch(&x, c, vector_size(c), sizeof(*c), cmp_entries);
	if (!r)
	{
		fprintf(stderr, "Configuration parameter %s is undefined\n", name);
		return NULL;
	}
	return r->value;
}

bool config_int(config_t c, const char* name, int* ret)
{
	const char *str = config_string(c, name);
	if (!str) return false;

	char *eptr = NULL;
	errno = 0;
	*ret = strtol(str, &eptr, 10);
	if (*eptr || errno)
	{
		fprintf(stderr, "Configuration parameter %s value is invalid\n", name);
		return false;
	}

	return true;
}

void config_delete(config_t c)
{
	for (int i = 0; i < vector_size(c); ++i)
	{
		free(c[i].name);
	}
	vector_delete(c);
}
