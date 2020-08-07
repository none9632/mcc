#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "table.h"
#include "error.h"

// list of node types
enum
{
	K_PROGRAM,
	K_FUNC,
	K_INIT_PARAMS,
	K_PARAMS,
	K_STATEMENTS,
	K_IF_ELSE,
	K_IF,
	K_ELSE,
	K_PRINT,
	K_INPUT,
	K_WHILE,
	K_DO_WHILE,
	K_FOR,
	K_RETURN,
	K_INIT_VARS,
	K_EXPR,
	K_OR,
	K_AND,
	K_EQUAL,
	K_NOT_EQUAL,
	K_MORE,
	K_LESS,
	K_MOREEQ,
	K_LESSEQ,
	K_ADD,
	K_SUB,
	K_MULT,
	K_DIV,
	K_MOD,
	K_NEG,
	K_VAR,
	K_NUM,
	K_ADDA,
	K_SUBA,
	K_MULTA,
	K_DIVA,
	K_MODA,
	K_ASSIGN,
	K_STRING,
	K_NONE
};

typedef struct node
{
	int kind;
	int value;
	Symbol *symbol;

	// Using union, node takes up less memory space.
	union
	{
		struct node *lhs; // left-hand side
		Vector *node_list;
	}
	u;

	struct node *rhs; // right-hand side
}
Node;

void  start_print_node (Node *node);
Node *new_node         (int kind);

#endif