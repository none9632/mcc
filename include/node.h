#ifndef NODE_H
#define NODE_H

#include <stdio.h>

// kind list
enum
{
	K_ADD,
	K_SUB,
	K_MULT,
	K_DIV,
	K_NUM,
	K_NONE
};

typedef struct node
{
	int kind;
	int value;
	struct node *n1;
	struct node *n2;
}
Node;

void output_node(Node *n, int count_bar);

#endif