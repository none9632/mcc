#ifndef LIB_H
#define LIB_H

struct vector {
	void  **data;
	int     len;
	int     capacity;
};

typedef struct vector Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);

#endif