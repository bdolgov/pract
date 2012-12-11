/* -*- mode:c; coding: utf-8 -*- */

#include "direct_cache.h"

#include <stdlib.h>
#include <string.h>

/*!
  Ограничения значений параметров
 */
enum
{
    MAX_CACHE_SIZE = 16 * MiB, //!< Максимальный поддерживаемый размер кеша
    MAX_READ_TIME = 100000, //!< Максимальное время чтения из кеша в тактах
    MAX_WRITE_TIME = MAX_READ_TIME, //!< Максимальное время записи в кеш в тактах
};

/*!
  Описание одного блока кеша прямого отображения
  \brief Блок кеша прямого отображения
 */
typedef struct DirectCacheBlock
{
    memaddr_t addr; //!< Адрес по которому в основной памяти (mem) находится этот блок, -1 - если этот блок свободен
    MemoryCell *mem; //!< Ячейки памяти, хранящиеся в блоке кеша
    int dirty; //!< Флаг того, что блок содержит данные, не сброшенные в память, только для write back кеша
} DirectCacheBlock;

struct DirectCache;
typedef struct DirectCache DirectCache;

/*!
  Дополнительные операции, настраивающие поведение кеша прямого отображения
  \brief Доп. операции кеша прямого отображения
 */
typedef struct DirectCacheOps
{
    /*!
      Функция должна выполнить синхронизацию между блоком кеша и памятью. Для кеша с прямой
      записью функция не делает ничего, для кеша с отложенной записью функция записывает
      грязный блок в память.
      \param c Указатель на дескриптор кеша
      \param b Указатель на финализируемый блок
     */
    void (*finalize)(DirectCache *c, DirectCacheBlock *b);
} DirectCacheOps;

/*!
  Кеш прямого отображения
  \brief Дескриптор кеша прямого отображения
 */
struct DirectCache
{
    AbstractMemory b; //!< Базовые поля
    DirectCacheOps direct_ops; //!< Дополнительные операции кеша прямого отображения
    DirectCacheBlock *blocks; //!< Блоки кеша
    AbstractMemory *mem; //!< Нижележащая память
    int cache_size; //!< Размер кеша (считывается из конф. файла)
    int block_size; //!< Размер одного блока кеша (считывается из конф. файла)
    int block_count; //!< Количество блоков кеша
    int cache_read_time; //!< Время выполнения чтения из кеша (считывается из конф. файла)
    int cache_write_time; //!< Время выполнения записи в кеш (считывается из конф. файла)
};

static AbstractMemory *
direct_cache_free(AbstractMemory *m)
{
    // FIXME: реализовать
    return NULL;
}

static DirectCacheBlock *
direct_cache_find(DirectCache *c, memaddr_t aligned_addr)
{
    int index = (aligned_addr / c->block_size) % c->block_count;
    if (c->blocks[index].addr != aligned_addr) return NULL;
    return &c->blocks[index];
}

static DirectCacheBlock *
direct_cache_place(DirectCache *c, memaddr_t aligned_addr)
{
    int index = (aligned_addr / c->block_size) % c->block_count;
    DirectCacheBlock *b = &c->blocks[index];
    if (b->addr != -1) {
        c->direct_ops.finalize(c, b);
        b->addr = -1;
        memset(b->mem, 0, c->block_size * sizeof(b->mem[0]));
    }
    return b;
}

static void
direct_cache_read(AbstractMemory *m, memaddr_t addr, int size, MemoryCell *dst)
{
    DirectCache *c = (DirectCache*) m;
    memaddr_t aligned_addr = addr & -c->block_size;
    // FIXME: реализовать до конца
}

/*!
  Функция записи в кеш в случае сквозной записи
*/
static void
direct_cache_wt_write(AbstractMemory *m, memaddr_t addr, int size, const MemoryCell *src)
{
    // FIXME: реализовать до конца
}

/*!
  Функция записи в кеш в случае отложенной записи
*/
static void
direct_cache_wb_write(AbstractMemory *m, memaddr_t addr, int size, const MemoryCell *src)
{
    DirectCache *c = (DirectCache*) m;
    memaddr_t aligned_addr = addr & -c->block_size;
    statistics_add_counter(c->b.info, c->cache_write_time);
    DirectCacheBlock *b = direct_cache_find(c, aligned_addr);
    if (!b) {
        b = direct_cache_place(c, aligned_addr);
        b->addr = aligned_addr;
        c->mem->ops->read(c->mem, aligned_addr, c->block_size, b->mem);
    }
    memcpy(b->mem + (addr - aligned_addr), src, size * sizeof(b->mem[0]));
    b->dirty = 1;
}

static void
direct_cache_reveal(AbstractMemory *m, memaddr_t addr, int size, const MemoryCell *src)
{
    DirectCache *c = (DirectCache*) m;
    memaddr_t aligned_addr = addr & -c->block_size;
    DirectCacheBlock *b = direct_cache_find(c, aligned_addr);
    if (b) {
        memcpy(b->mem + (addr - aligned_addr), src, size * sizeof(b->mem[0]));
    }
    c->mem->ops->reveal(c->mem, addr, size, src);
}

/*!
  Финализация блока кеша в случае сквозной записи
*/
static void
direct_cache_wt_finalize(DirectCache *c, DirectCacheBlock *b)
{
    // FIXME: реализовать до конца
}

/*!
  Финализация блока кеша в случае отложенной записи записи
*/
static void
direct_cache_wb_finalize(DirectCache *c, DirectCacheBlock *b)
{
    // FIXME: реализовать до конца
}

static void
direct_cache_flush(AbstractMemory *m)
{
    // FIXME: реализовать до конца
}

static AbstractMemoryOps direct_cache_wt_ops =
{
    direct_cache_free,
    direct_cache_read,
    direct_cache_wt_write,
    direct_cache_reveal,
    direct_cache_flush,
};

static AbstractMemoryOps direct_cache_wb_ops =
{
    direct_cache_free,
    direct_cache_read,
    direct_cache_wb_write,
    direct_cache_reveal,
    direct_cache_flush,
};

AbstractMemory *
direct_cache_create(ConfigFile *cfg, const char *var_prefix, StatisticsInfo *info, AbstractMemory *mem, Random *rnd)
{
    char buf[1024];
    DirectCache *c = (DirectCache*) calloc(1, sizeof(*c));
    c->b.info = info;
    const char *strategy = config_file_get(cfg, make_param_name(buf, sizeof(buf), var_prefix, "write_strategy"));
    if (!strategy) {
        error_undefined("direct_cache_create", buf);
    } else if (!strcmp(strategy, "write-through")) {
        c->b.ops = &direct_cache_wt_ops;
        c->direct_ops.finalize = direct_cache_wt_finalize;
    } else if (!strcmp(strategy, "write-back")) {
        c->b.ops = &direct_cache_wb_ops;
        c->direct_ops.finalize = direct_cache_wb_finalize;
    } else {
        error_invalid("direct_cache_create", buf);
    }
    c->mem = mem;

    // FIXME: реализовать до конца

    return (AbstractMemory*) c;
}

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
