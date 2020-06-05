#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <malloc.h>

typedef struct vector
{
	void **data;
	size_t length;
	size_t capacity;
}
Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);

#endif