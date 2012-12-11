/* -*- mode:c; coding: utf-8 -*- */

/*!
  \file cache.h
  \brief Общий интерфейс к модели кеша
 */

#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED

#include "abstract_memory.h"
#include "parse_config.h"
#include "random.h"

/*!
  Создать кеш
  \param cfg Указатель на структуру, хранящую конфигурационные параметры
  \param var_prefix Префикс имен параметров
  \param info Указатель на структуру, хранящую статистику моделирования
  \param mem Указатель на дескриптор нижележащей памяти (в виде указателя на базовую структуру)
  \param rnd Указатель на дескриптор генератора случайных чисел
  \return Указатель на структуру описания модели кеша (в виде указателя на базовую структуру)
 */
AbstractMemory *cache_create(ConfigFile *cfg, const char *var_prefix, StatisticsInfo *info, AbstractMemory *mem, Random *rnd);

#endif

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
