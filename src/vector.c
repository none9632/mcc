#include <malloc.h>

#include "vector.h"
#include "error.h"

Vector *new_vector()
{
	Vector *vector = malloc(sizeof(Vector));

	if (vector == NULL)
		func_error();

	vector->length = 0;
	vector->capacity = 16;
	vector->data = malloc(sizeof(void *) * vector->capacity);

	if (vector->data == NULL)
		func_error();

	return vector;
}

void vec_push(Vector *vector, void *elem)
{
	if (vector->length + 1 >= vector->capacity)
	{
		vector->capacity *= 2;
		vector->data = realloc(vector->data, sizeof(void *) * vector->capacity);

		if (vector->data == NULL)
			func_error();
	}
	vector->data[vector->length++] = elem;
}
