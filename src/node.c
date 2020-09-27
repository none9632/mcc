#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "table.h"
#include "error.h"
#include "node.h"

static char *prefix = NULL;

extern Vector *string_list;

Node *new_node(u_int8_t kind, int value, Symbol *symbol, void *p_union, Node *rhs)
{
	Node *node = malloc(sizeof(Node));

	if (node == NULL)
		func_error();

	node->kind = kind;
	node->value = value;
	node->symbol = symbol;
	node->u.node_list = p_union;
	node->rhs = rhs;

	return node;
}

static void print_kind(Node *node)
{
	switch (node->kind)
	{
		case K_PROGRAM:      printf("<program>\n");                break;
		case K_FUNC:         printf("%s()\n", node->symbol->name); break;
		case K_CALL_FUNC:    printf("%s()\n", node->symbol->name); break;
		case K_INIT_PARAMS:  printf("<init-params>\n");            break;
		case K_PARAMS:       printf("<params>\n");                 break;
		case K_STATEMENTS:   printf("<statements>\n");             break;
		case K_IF_ELSE:      printf("<if-else>\n");                break;
		case K_IF:           printf("<if>\n");                     break;
		case K_ELSE:         printf("<else>\n");                   break;
		case K_PRINTF:       printf("<printf>\n");                 break;
		case K_SCANF:        printf("<scanf>\n");                  break;
		case K_WHILE:        printf("<while>\n");                  break;
		case K_DO_WHILE:     printf("<do-while>\n");               break;
		case K_FOR:          printf("<for>\n");                    break;
		case K_RETURN:       printf("<return>\n");                 break;
		case K_INIT_VARS:    printf("<init-vars>\n");              break;
		case K_GVAR:         printf("%s\n", node->symbol->name);   break;
		case K_VAR:          printf("%s\n", node->symbol->name);   break;
		case K_EXPR:         printf("<expr>\n");                   break;
		case K_ADDA:         printf("+=\n");                       break;
		case K_SUBA:         printf("-=\n");                       break;
		case K_MULTA:        printf("*=\n");                       break;
		case K_DIVA:         printf("/=\n");                       break;
		case K_MODA:         printf("%%=\n");                      break;
		case K_BIT_ANDA:     printf("&=\n");                       break;
		case K_BIT_XORA:     printf("^=\n");                       break;
		case K_BIT_ORA:      printf("|=\n");                       break;
		case K_LEFT_SHIFTA:  printf("<<=\n");                      break;
		case K_RIGHT_SHIFTA: printf(">>=\n");                      break;
		case K_ASSIGN:       printf("=\n");                        break;
		case K_OR:           printf("||\n");                       break;
		case K_AND:          printf("&&\n");                       break;
		case K_BIT_OR:       printf("|\n");                        break;
		case K_BIT_XOR:      printf("^\n");                        break;
		case K_BIT_AND:      printf("&\n");                        break;
		case K_EQUAL:        printf("==\n");                       break;
		case K_NOT_EQUAL:    printf("!=\n");                       break;
		case K_MOREEQ:       printf(">=\n");                       break;
		case K_LESSEQ:       printf("<=\n");                       break;
		case K_MORE:         printf(">\n");                        break;
		case K_LESS:         printf("<\n");                        break;
		case K_LEFT_SHIFT:   printf("<<\n");                       break;
		case K_RIGHT_SHIFT:  printf(">>\n");                       break;
		case K_ADD:          printf("+\n");                        break;
		case K_SUB:          printf("-\n");                        break;
		case K_MULT:         printf("*\n");                        break;
		case K_DIV:          printf("/\n");                        break;
		case K_MOD:          printf("%%\n");                       break;
		case K_NEG:          printf("-\n");                        break;
		case K_PRE_INC:      printf("<pre_inc>\n");                break;
		case K_POST_INC:     printf("<post_inc>\n");               break;
		case K_PRE_DEC:      printf("<pre_dec>\n");                break;
		case K_POST_DEC:     printf("<post_dec>\n");               break;
		case K_NUM:          printf("%i\n", node->value);          break;
		case K_NONE:         printf("<none>\n");                   break;
		case K_STRING:
			printf("\"%s\"\n", (char *)string_list->data[node->value]);
			break;
	}
}

/*
 * The '│' symbol cannot be saved to the pointer so I save '|' and output '│'.
 */
static void print_prefix(uint prefix_len)
{
	for (uint i = 0; i < prefix_len; i++)
	{
		if (prefix[i] == '|')
			printf("│");
		else
			printf("%c", prefix[i]);
	}
}

/*
 * Adds '|   ' or '    ' to the prefix.
 */
static void new_prefix(uint prefix_len, int8_t is_left)
{
	prefix = realloc(prefix, prefix_len + 4);

	if (prefix == NULL)
		func_error();

	for (int i = prefix_len; i < prefix_len + 4; ++i)
		prefix[i] = ' ';

	if (is_left)
		prefix[prefix_len] = '|';
}

static int8_t is_node_list(u_int8_t kind)
{
	return (kind == K_STATEMENTS  ||
			kind == K_INIT_PARAMS ||
			kind == K_PARAMS      ||
			kind == K_INIT_VARS   ||
	        kind == K_FOR         ||
	        kind == K_PRINTF      ||
	        kind == K_SCANF       );
}

static void print_node(Node *node, uint prefix_len, int8_t is_left)
{
	if (node != NULL)
	{
		print_prefix(prefix_len);
		printf("%s", (is_left ? "├── " : "└── "));
		print_kind(node);

		new_prefix(prefix_len, is_left);
		prefix_len += 4;

		if (is_node_list(node->kind))
		{
			uint i = 0;
			for (; i < node->u.node_list->length - 1; ++i)
				print_node(node->u.node_list->data[i], prefix_len, 1);
			print_node(node->u.node_list->data[i], prefix_len, 0);
		}
		else
		{
			print_node(node->u.lhs, prefix_len, 1);
			print_node(node->rhs, prefix_len, 0);
		}
	}
}

void start_print_node(Node *node)
{
	print_kind(node);

	uint i = 0;
	for (; i < node->u.node_list->length - 1; ++i)
		print_node(node->u.node_list->data[i], 0, 1);
	print_node(node->u.node_list->data[i], 0, 0);
}
