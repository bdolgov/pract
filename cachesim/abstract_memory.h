/* -*- mode:c; coding: utf-8 -*- */

/*!
  \file abstract_memory.h
  \brief Базовый интерфейс к модели памяти
 */

#ifndef ABSTRACT_MEMORY_H_INCLUDED
#define ABSTRACT_MEMORY_H_INCLUDED

#include "common.h"
#include "statistics.h"

struct AbstractMemory;
typedef struct AbstractMemory AbstractMemory;

/*! Операции с памятью
  \brief Базовые операции с памятью
 */
typedef struct AbstractMemoryOps
{
/*!
  Освободить ресурсы
  \param m Указатель на структуру описания модели памяти
  \return Указатель NULL
 */
    AbstractMemory *(*free)(AbstractMemory *m);
/*!
  Прочитать ячейки из памяти
  \param m Указатель на структуру описания модели памяти
  \param addr Адрес в памяти
  \param size Количество считываемых ячеек
  \param dst Указатель, куда копировать ячейки из памяти
 */
    void (*read)(AbstractMemory *m, memaddr_t addr, int size, MemoryCell *dst);
/*!
  Записать ячейки в память
  \param m Указатель на структуру описания модели памяти
  \param addr Адрес в памяти
  \param size Количество записываемых ячеек
  \param src Указатель, откуда копировать ячейки в память
 */
    void (*write)(AbstractMemory *m, memaddr_t addr, int size, const MemoryCell *src);
/*!
  "Раскрыть" содержимое указанных ячеек памяти. Эта функция необходима, так как начальное состояние
  памяти неизвестно, и значения ячеек памяти могут определяться как при записи, так и при чтении,
  так как 'R'-запись в трассе содержит считываемое значение. Эта функция работает как запись в память
  за исключением того, что не учитывается время доступа.
  \param a Указатель на структуру описания модели памяти
  \param addr Адрес в памяти
  \param size Количество считываемых ячеек
  \param src Указатель, откуда копировать ячейки в память
 */
    void (*reveal)(AbstractMemory *m, memaddr_t addr, int size, const MemoryCell *src);
    /*!
      Фиксировать состояние памяти. Например, кеш с отложенной записью
      можно сбросить в кеш все грязные блоки. Для других типов памяти
      данная операция может не делать ничего.
      \param m Указатель на структуру описания модели памяти
     */
    void (*flush)(AbstractMemory *m);
} AbstractMemoryOps;

/*! Базовая структура модели памяти
  \brief Дескриптор базовой модели памяти
 */
struct AbstractMemory
{
    AbstractMemoryOps *ops; //!< Указатели на операции
    StatisticsInfo *info; //!< Указатель на структуру, хранящую статистику выполнения
};

#endif

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
