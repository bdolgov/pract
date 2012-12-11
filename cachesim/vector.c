#include "vector.h"

#include <stdlib.h>
#include <string.h>

typedef struct
{
	int elem_size;
	int size;
	int alloc;
	char pad[4];
} vector_info_t;

char* do_vector_new(int size)
{
	vector_info_t *v = malloc(sizeof(vector_info_t) + size);
	v->elem_size = size;
	v->size = 0;
	v->alloc = 1;
	return (char*)v + sizeof(vector_info_t);
}

void do_vector_delete(char** v)
{
	free(*v - sizeof(vector_info_t));
	*v = NULL;
}

int do_vector_size(char** v)
{
	vector_info_t* i = (vector_info_t*)(*v - sizeof(vector_info_t));
	return i->size;
}

void *do_vector_pb(char** v, void* item)
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

void do_vector_pop(char** v)
{
	vector_info_t* i = (vector_info_t*)(*v - sizeof(vector_info_t));
	i->size--;
}

void do_vector_remove(char** v, int idx)
{
	vector_info_t* i = (vector_info_t*)(*v - sizeof(vector_info_t));
	if (--i->size != idx) memcpy(*v + idx * i->elem_size, *v + i->size  * i->elem_size, i->elem_size); 
}
