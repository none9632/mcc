#ifndef NODE_H
#define NODE_H

#include <stdio.h>

// kind list
enum
{
};

typedef struct node
{
	int kind;
	int value;
}
Node;

void output_node(Node *n, int count_bar);

#endif