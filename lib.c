#include "lib.h"
#include <malloc.h>
#include <string.h>

Vector* new_vec()
{
	Vector *v = malloc(sizeof(v));
	v->capacity = 16;
	v->len = 0;
	v->data = malloc(sizeof(void*) * v->capacity);
	return v;
}

void vec_push(Vector *v, void *elem)
{
	if (v->len + 1 >= v->capacity) {
		v->capacity *= 2;
		v->data = realloc(v->data, sizeof(void*) * v->capacity);
	}
	v->data[v->len++] = elem;
}