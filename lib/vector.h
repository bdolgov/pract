#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

#define vector_new(type) ((type*)do_vector_new(sizeof(type)))
#define vector_push_back(v, item) do_vector_pb((char**)&v, (void*)&item)
#define vector_push_back_i(v, item) (vector_push_back(v, item), vector_size(v) - 1)
#define vector_size(v) do_vector_size((char**)&v)
#define vector_delete(v) do_vector_delete((char**)&v)

typedef struct
{
	int elem_size;
	int size;
	int alloc;
	char pad[4];
} vector_info_t;

static char* do_vector_new(int size)
{
	vector_info_t *v = malloc(sizeof(vector_info_t) + size);
	v->elem_size = size;
	v->size = 0;
	v->alloc = 1;
	return (char*)v + sizeof(vector_info_t);
}

static void do_vector_delete(char** v)
{
	free(*v - sizeof(vector_info_t));
	*v = NULL;
}

static int do_vector_size(char** v)
{
	vector_info_t* i = (vector_info_t*)(*v - sizeof(vector_info_t));
	return i->size;
}

static void *do_vector_pb(char** v, void* item)
{
	vector_info_t* i = (vector_info_t*)(*v - sizeof(vector_info_t));
	if (i->size == i->alloc)
	{
		i->alloc *= 2;
		i = realloc(i, sizeof(vector_info_t) + i->elem_size * i->alloc);
		*v = (char*)i + sizeof(vector_info_t);
	}
	void *new_place = *v + i->elem_size * i->size++;
	memcpy(new_place, item, i->elem_size);
	return new_place;
}

#endif
