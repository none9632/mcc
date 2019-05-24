#pragma once

typedef struct vector {
	void **data;
	int len;
	int capacity;
}Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);