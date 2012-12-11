/* -*- mode:c; coding: utf-8 -*- */

/*!
  \file random.h
  \brief Общий интерфейс к генератору случайных чисел
*/

#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include "parse_config.h"

struct Random;
typedef struct Random Random;

/*!
  Структура указателей на операции со случайными числами .
  \brief Операции с генератором случайных чисел
 */
typedef struct RandomOps
{
    /*!
      Освободить ресурсы.
      \param rnd Указатель на дескриптор генератора случайных чисел.
      \return NULL
     */
    Random *(*free)(Random *rnd);
    /*!
      Сгенерировать следующее случайное число в последовательности
      равномерно распределенных целых случайных чисел в интервале [0, n)
      \param rnd Указатель на дескриптор генератора случайных чисел
      \param n Верхняя граница случайных чисел
      \return случайное число в интервале [0, n)
     */
    int (*next)(Random *rnd, int n);
} RandomOps;

/*!
  Создать дескриптор генератора случайных чисел.
  \param cfg Конфигурационные параметры
  \return Новый дескриптор случайных чисел
 */
Random *random_create(ConfigFile *cfg);

#endif

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
