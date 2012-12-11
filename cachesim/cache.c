/* -*- mode:c; coding: utf-8 -*- */

#include "cache.h"
#include "direct_cache.h"
#include "full_cache.h"

#include <string.h>

AbstractMemory *
cache_create(ConfigFile *cfg, const char *var_prefix, StatisticsInfo *info, AbstractMemory *mem, Random *rnd)
{
    const char *a = config_file_get(cfg, "associativity");
    if (!a) {
        error_undefined("cache_create", "associativity");
    } else if (!strcmp(a, "full")) {
        return full_cache_create(cfg, var_prefix, info, mem, rnd);
    } else if (!strcmp(a, "direct")) {
        return direct_cache_create(cfg, var_prefix, info, mem, rnd);
    } else {
        error_invalid("cache_create", "associativity");
    }
    return NULL;
}

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
