#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

// kind list
enum
{
	K_PROGRAM,    // 0
	K_STATEMENT,  // 1
	K_PRINT,      // 2
	K_INPUT,      // 3
	K_WHILE,      // 4
	K_DO_WHILE,   // 5
	K_IF,         // 6
	K_IF_ELSE,    // 7
	K_ADD,        // 8
	K_SUB,        // 9
	K_MULT,       // 10
	K_DIV,        // 11
	K_MOD,        // 12
	K_NEG,        // 13
	K_POSITIVE,   // 14
	K_MORE,       // 15
	K_LESS,       // 16
	K_MOREEQ,     // 17
	K_LESSEQ,     // 18
	K_EQUAL,      // 19
	K_NOT_EQUAL,  // 20
	K_AND,        // 21
	K_OR,         // 22
	K_VAR,        // 23
	K_NUM,        // 24
	K_NONE        // 25
};

typedef struct node
{
	int kind;
	int value;
	struct node *n1;
	struct node *n2;

	Vector *node_list;
}
Node;

void  start_print_node (Node *n);
Node *new_node         ();

#endif