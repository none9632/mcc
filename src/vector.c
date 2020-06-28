#include "../include/vector.h"

Vector *new_vector()
{
	Vector *vector = malloc(sizeof(Vector));

	if (vector == NULL)
		error(0, 0, "memory allocation error in new_vector()");

	vector->length   = 0;
	vector->capacity = 16;
	vector->data     = malloc(sizeof(void *) * vector->capacity);

	if (vector->data == NULL)
		error(0, 0, "memory allocation error in new_vector()");

	return vector;
}

void vec_push(Vector *vector, void *elem)
{
	if (vector->length + 1 >= vector->capacity)
	{
		vector->capacity *= 2;
		vector->data      = realloc(vector->data, sizeof(void *) * vector->capacity);

		if (vector->data == NULL)
			error(0, 0, "memory allocation error in vec_push()");
	}
	vector->data[vector->length++] = elem;
}
