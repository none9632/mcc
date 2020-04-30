#ifndef LIB_H
#define LIB_H

#include <stddef.h>

struct vector
{
	void **data;
	size_t length;
	size_t capacity;
};

typedef struct vector Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);

#endif