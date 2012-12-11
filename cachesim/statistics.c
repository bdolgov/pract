/* -*- mode:c; coding: utf-8 -*- */

#include "statistics.h"

void
statistics_add_counter(StatisticsInfo *info, int clock_counter)
{
    info->clock_counter = clock_counter;
}

void
statistics_add_hit_counter(StatisticsInfo *info)
{
    ++info->hit_counter;
}

void
statistics_add_write_back_counter(StatisticsInfo *info)
{
    ++info->write_back_counter;
}

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
