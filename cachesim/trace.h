/* -*- mode:c; coding: utf-8 -*- */

/*!
  \file trace.h
  \brief Работа с трассой
 */

#ifndef TRACE_H_INCLUDED
#define TRACE_H_INCLUDED

#include "common.h"

#include <stdio.h>

/*!
  Структура описывает шаг трассы
  \brief Описание шага трассы
 */
typedef struct TraceStep
{
    char op; //!< 'R' - чтение, 'W' - запись
    char mem; //!< 'D' - память данных, 'I' - память инструкций
    memaddr_t addr; //!< адрес в памяти
    int size; //!< размер операции (1, 2, 4, 8)
    MemoryCell value[8]; //!< данные для чтения/записи
} TraceStep;

/*!
  Структура хранит состояние чтения трассы. Полное описание структуры
  находится в trace.c
 */
struct Trace;
typedef struct Trace Trace;

/*!
  Функция открывает файл трассы.
  \param path Путь к файлу, NULL означает чтение из stdin
  \param log_f Файл, в который выводить ошибки
  \return Указатель на дескриптор трассы, или NULL при ошибке
 */
Trace *trace_open(const char *path, FILE *log_f);

/*!
  Функция освобождает ресурсы трассы.
  \param t Указатель на дескриптор трассы
 */
Trace *trace_close(Trace *t);

/*!
  Функция считывает очередной шаг трассы из указанного дескриптора трассы.
  Параметры шага трассы записываются в структуру, получить доступ к которой можно с помощью trace_get
  \param t Указатель на дескриптор трассы
  \return В случае успешного чтения возвращается 1,
  в случае достижения конца файла возвращается 0,
  в случае ошибки при чтении возвращается -1. 
 */
int trace_next(Trace *t);

/*!
  Функция возвращает текущий шаг трассы (поле step дескриптора трассы)
  \param t Указатель на дескриптор трассы
  \return Указатель на структуру шага трассы
 */
TraceStep *trace_get(Trace *t);

#endif

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
