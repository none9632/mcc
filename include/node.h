#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

// kind list
enum
{
	K_PROGRAM,    // 0
	K_STATEMENTS, // 1
	K_PRINT,      // 2
	K_INPUT,      // 3
	K_WHILE,      // 4
	K_DO_WHILE,   // 5
	K_IF,         // 6
	K_IF_ELSE,    // 7
	K_ELSE,       // 8
	K_ADD,        // 9
	K_SUB,        // 10
	K_MULT,       // 11
	K_DIV,        // 12
	K_MOD,        // 13
	K_NEG,        // 14
	K_POSITIVE,   // 15
	K_MORE,       // 16
	K_LESS,       // 17
	K_MOREEQ,     // 18
	K_LESSEQ,     // 19
	K_EQUAL,      // 20
	K_NOT_EQUAL,  // 21
	K_AND,        // 22
	K_OR,         // 23
	K_VAR,        // 24
	K_NUM,        // 25
	K_NONE,       // 26
	K_ASSIGN,     // 27
	K_PAREN_EXPR  // 28
};

typedef struct node
{
	int kind;
	int value;
	struct node *lhs; // left-hand side
	struct node *rhs; // right-hand side

	Vector *node_list;
}
Node;

void  start_print_node (Node *n);
Node *new_node         ();

#endif