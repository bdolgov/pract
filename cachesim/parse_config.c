/* -*- mode:c; coding: utf-8 -*- */

#include "parse_config.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

enum
{
    LINE_BUF_SIZE = 1024,
    MAX_LINE_LENGTH = 1000
};

/*!
  Структура хранит один конфигурационный параметр
  \brief Описание одного конфигурационного параметра
 */
typedef struct ConfigParam
{
    char *name; //!< Имя параметра
    char *value; //!< Значение параметра
} ConfigParam;

/*!
  Структура описывает считанные в память конфигурационные параметры.
  \brief Дескриптор конфигурационного файла
 */
struct ConfigFile
{
    int size; //!< Число использованных элементов в векторе
    int reserved; //!< Число элементов, под которые зарезервирована память
    ConfigParam *params; //!< Массив параметров
};

ConfigFile *
config_file_parse(const char *path, FILE *log_f)
{
    ConfigFile *cfg = (ConfigFile*) calloc(1, sizeof(*cfg));

    // FIXME: реализовать разбор конфигурационного файла и сортировку массива params

    return cfg;
}

ConfigFile *
config_file_free(ConfigFile *cfg)
{
    // FIXME: реализовать
    return NULL;
}

const char *
config_file_get(const ConfigFile *cfg, const char *name)
{
    // FIXME: реализовать бинарный поиск
    return NULL;
}

int
config_file_get_int(const ConfigFile *cfg, const char *name, int *p_value)
{
    const char *str = config_file_get(cfg, name);
    if (!str) {
        return 0;
    }
    char *eptr = NULL;
    errno = 0;
    int value = strtol(str, &eptr, 10);
    if (*eptr || errno) {
        return -1;
    }
    *p_value = value;
    return 1;
}

/*
 * Local variables:
 *  c-basic-offset: 4
 * End:
 */
