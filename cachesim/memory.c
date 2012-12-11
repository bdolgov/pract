/* -*- mode:c; coding: utf-8 -*- */

#include "memory.h"

#include <stdlib.h>

/*!
  Ограничения значений параметров
 */
enum
{
    MAX_MEM_SIZE = 1 * GiB, //!< Максимальный поддерживаемый размер ОЗУ модели
    MAX_READ_TIME = 100000, //!< Максимальное время чтения из ОЗУ в тактах
    MAX_WRITE_TIME = MAX_READ_TIME, //!< Максимальное время записи в озу в тактах
    MAX_WIDTH = 1024 //!< Максимальный размер блока памяти
};

struct Memory;
typedef struct Memory Memory;

/*!
  Структура описывает модель ОЗУ
  \brief Дескриптор модели ОЗУ
 */
struct Memory
{
    AbstractMemory b; //!< Базовые поля
    MemoryCell *mem; //!< Массив ячеек памяти
    int memory_size; //!< Размер ОЗУ (считанный из конфиг. файла)
    int memory_read_time; //!< Время чтения из ОЗУ
    int memory_write_time; //!< Время записи в ОЗУ
    int memory_width; //!< Полоса пропускания ОЗУ
};

/*!
  Освободить ресурсы
  \param a Указатель на структуру описания модели ОЗУ (в виде указателя на базовую структуру)
  \return Указатель NULL
 */
static AbstractMemory *
memory_free(AbstractMemory *a)
{
    // FIXME: реализовать
    return NULL;
}

/*!
  Прочитать ячейки из ОЗУ
  \param a Указатель на структуру описания модели ОЗУ (в виде указателя на базовую структуру)
  \param addr Адрес в ОЗУ
  \param size Количество считываемых ячеек
  \param dst Указатель, куда копировать ячейки из ОЗУ
 */
static void
memory_read(AbstractMemory *a, memaddr_t addr, int size, MemoryCell *dst)
{
    Memory *m = (Memory*) a;

    // учитываем время, требуемое на выполнение операции чтения
    statistics_add_counter(m->b.info, (size + m->memory_width - 1) / m->memory_width * m->memory_read_time);
    // выполняем копирование данных
    for (; size; ++addr, --size, ++dst) {
        *dst = m->mem[addr];
    }
}

/*!
  Записать ячейки в ОЗУ
  \param a Указатель на структуру описания модели ОЗУ (в виде указателя на базовую структуру)
  \param addr Адрес в ОЗУ
  \param size Количество записываемых ячеек
  \param src Указатель, откуда копировать ячейки в ОЗУ
 */
static void
memory_write(AbstractMemory *a, memaddr_t addr, int size, const MemoryCell *src)
{
    // FIXME: реализовать
}

/*!
  "Раскрыть" содержимое указанных ячеек ОЗУ. Эта функция необходима, так как начальное состояние
  памяти неизвестно, и значения ячеек памяти могут определяться как при записи, так и при чтении,
  так как 'R'-запись в трассе содержит считываемое значение. Эта функция работает как запись в память
  за исключением того, что не учитывается время доступа.
  \param a Указатель на структуру описания модели ОЗУ (в виде указателя на базовую структуру)
  \param addr Адрес в ОЗУ
  \param size Количество считываемых ячеек
  \param src Указатель, откуда копировать ячейки в ОЗУ
 */
static void
memory_reveal(AbstractMemory *a, memaddr_t addr, int size, const MemoryCell *src)
{
    Memory *m = (Memory*) a;

    for (; size; ++addr, --size, ++src) {
        m->mem[addr] = *src;
    }
}

static void
memory_flush(AbstractMemory *m)
{
}

static AbstractMemoryOps memory_ops =
{
    memory_free,
    memory_read,
    memory_write,
    memory_reveal,
    memory_flush,
};

/*!
  Создать модель ОЗУ
  \param cfg Указатель на структуру, хранящую конфигурационные параметры
  \param var_prefix Префикс имен параметров
  \param info Указатель на структуру, хранящую статистику моделирования
  \return Указатель на структуру описания модели ОЗУ (в виде указателя на базовую структуру)
 */
AbstractMemory *
memory_create(ConfigFile *cfg, const char *var_prefix, StatisticsInfo *info)
{
    char buf[1024];
    Memory *m = calloc(1, sizeof(*m));
    int r;

    // заполняем базовые поля
    m->b.ops = &memory_ops;
    m->b.info = info;

    // считываем и проверяем параметр memory_size
    r = config_file_get_int(cfg, make_param_name(buf, sizeof(buf), var_prefix, "memory_size"), &m->memory_size);
    if (!r) {
        error_undefined("memory_create", buf);
    } else if (r < 0 || m->memory_size <= 0 || m->memory_size > MAX_MEM_SIZE || m->memory_size % KiB != 0) {
        error_invalid("memory_create", buf);
    }

    // FIXME: реализовать чтение параметров memory_read_time, memory_write_time, memory_width

    // создаем массив ячеек
    m->mem = (MemoryCell*) calloc(m->memory_size, sizeof(m->mem[0]));

    // возвращяем указатель, преобразованный к указателю на базовую структуру
    return (AbstractMemory*) m;
}

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
