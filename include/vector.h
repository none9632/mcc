#ifndef VECTOR_H
#define VECTOR_H

typedef struct vector
{
	void **data;
	size_t length;
	size_t capacity;
}
Vector;

Vector *new_vector ();
void    vec_push   (Vector *vector, void *elem);
void    vec_free   (Vector *vector);

#endif