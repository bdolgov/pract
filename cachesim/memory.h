/* -*- mode:c; coding: utf-8 -*- */

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

/*!
  \file memory.h
  \brief Модель ОЗУ
 */

#include "abstract_memory.h"
#include "parse_config.h"

/*!
  Создать модель ОЗУ
  \param cfg Указатель на структуру, хранящую конфигурационные параметры
  \param var_prefix Префикс имен параметров
  \param info Указатель на структуру, хранящую статистику моделирования
  \return Указатель на структуру описания модели ОЗУ (в виде указателя на базовую структуру)
 */
AbstractMemory *memory_create(ConfigFile *cfg, const char *var_prefix, StatisticsInfo *info);

#endif

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
