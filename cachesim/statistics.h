/* -*- mode:c; coding: utf-8 -*- */

/*!
  \file statistics.h
  \brief Описание собираемой статистики
*/

#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED

#include "parse_config.h"

#include <stdio.h>

/*! Подсчет статистики */

struct StatisticsInfo;
typedef struct StatisticsInfo StatisticsInfo;

/*! Структура для накопления информации о работе кеша
 \brief Собираемая при симуляции статистическая информация
 */
struct StatisticsInfo
{
    int hit_counter_needed; //!< Требуется ли подсчитывать число попаданий
    int write_back_needed; //!< Требуется ли подсчитывать записи блоков кеша в память (при отложенной записи)
    int clock_counter; //!< Общее число тактов требуемых для выполнения трассы
    int read_counter; //!< Число операций чтения в трассе
    int write_counter; //!< Число операций записи в трассе
    int hit_counter; //!< Число попаданий в кеш при чтении
    int write_back_counter; //!< Число записей блоков кеша в память при отложенной записи
};

/*!
  Создание дескриптора статистической информации.
  \param cfg Указатель на конфигурационные параметры
  \return Указатель на новый дескриптор статистической информации
 */
StatisticsInfo *statistics_create(ConfigFile *cfg);
/*!
  Освободить ресурсы
  \param info Указатель на дескриптор статистической информации
  \return NULL
 */
StatisticsInfo *statistics_free(StatisticsInfo *info);
/*!
  Добавить число к счетчику тактов
  \param info Указатель на дескриптор статистической информации
  \param clock_counter Добавляемое количество тактов
 */
void statistics_add_counter(StatisticsInfo *info, int clock_counter);
/*!
  Увеличить счетчик чтений на 1
  \param info Указатель на дескриптор статистической информации
 */
void statistics_add_read(StatisticsInfo *info);
/*!
  Увеличить счетчик записей на 1
  \param info Указатель на дескриптор статистической информации
 */
void statistics_add_write(StatisticsInfo *info);
/*!
  Увеличить счетчик попаданий на 1
  \param info Указатель на дескриптор статистической информации
 */
void statistics_add_hit_counter(StatisticsInfo *info);
/*!
  Увеличить счетчик отложенных записей на 1
  \param info Указатель на дескриптор статистической информации
 */
void statistics_add_write_back_counter(StatisticsInfo *info);
/*!
  Распечатать статистическую информацию
  \param info Указатель на дескриптор статистической информации
  \param f Поток для вывода информации
 */
void statistics_print(StatisticsInfo *info, FILE *f);

#endif

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
