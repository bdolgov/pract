#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

#define vector_new(type) ((type*)do_vector_new(sizeof(type)))
#define vector_push_back(v, item) do_vector_pb((char**)&v, (void*)&item)
#define vector_push_back_i(v, item) (vector_push_back(v, item), vector_size(v) - 1)
#define vector_size(v) do_vector_size((char**)&v)
#define vector_delete(v) do_vector_delete((char**)&v)
#define vector_pop(v) do_vector_pop((char**)&v)
#define vector_remove(v, i) do_vector_remove((char**)&v, i)

char* do_vector_new(int size);
void do_vector_delete(char** v);
int do_vector_size(char** v);
void *do_vector_pb(char** v, void* item);
void do_vector_pop(char** v);
void do_vector_remove(char** v, int idx);

#endif
