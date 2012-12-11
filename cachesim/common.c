/* -*- mode:c; coding: utf-8 -*- */

#include "common.h"

#include <stdio.h>
#include <stdlib.h>

char *
make_param_name(char *buf, int size, const char *prefix, const char *name)
{
    if (!prefix) prefix = "";
    snprintf(buf, size, "%s%s", prefix, name);
    return buf;
}

void
error_undefined(const char *func, const char *param)
{
    fprintf(stderr, "%s: configuration parameter '%s' is undefined\n", func, param);
    exit(1);
}

void
error_invalid(const char *func, const char *param)
{
    fprintf(stderr, "%s: configuration parameter '%s' value is invalid\n", func, param);
    exit(1);
}

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
