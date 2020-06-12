#include "../include/node.h"

static char *prefix;

Node *new_node()
{
	Node *n = malloc(sizeof(Node));
	n->kind = K_NONE;
	n->value = 0;
	n->lhs = NULL;
	n->rhs = NULL;
	n->node_list = NULL;
	return n;
}

static void print_kind(int kind, int value)
{
	switch (kind)
	{
		case K_ADD:        printf("+\n");            break;
		case K_SUB:        printf("-\n");            break;
		case K_MULT:       printf("*\n");            break;
		case K_DIV:        printf("/\n");            break;
		case K_MOD:        printf("%%\n");           break;
		case K_MORE:       printf(">\n");            break;
		case K_LESS:       printf("<\n");            break;
		case K_MOREEQ:     printf(">=\n");           break;
		case K_LESSEQ:     printf("<=\n");           break;
		case K_EQUAL:      printf("==\n");           break;
		case K_NOT_EQUAL:  printf("!=\n");           break;
		case K_AND:        printf("&&\n");           break;
		case K_OR:         printf("||\n");           break;
		case K_POSITIVE:   printf("+\n");            break;
		case K_NEG:        printf("-\n");            break;
		case K_NUM:        printf("%i\n", value);    break;
		case K_PROGRAM:    printf("<program>\n");    break;
		case K_NONE:       printf("none\n");         break;
		case K_STATEMENTS: printf("<statements>\n"); break;
		case K_IF:         printf("<if>\n");         break;
		case K_ASSIGN:     printf("<assign>\n");     break;
		case K_PAREN_EXPR: printf("<paren_expr>\n"); break;
		case K_ELSE:       printf("<expr>\n");       break;
		case K_IF_ELSE:    printf("<if-else>\n");    break;
	}
}

/*
 * The '│' symbol cannot be saved to the pointer so I save '|' and output '│'.
 */
static void print_prefix(int prefix_len)
{
	for (int i = 0; i < prefix_len; i++)
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
static void new_prefix(int prefix_len, int is_left)
{
	prefix = realloc(prefix, prefix_len + 4);

	for (int i = prefix_len; i < prefix_len + 4; ++i)
		prefix[i] = ' ';

	if (is_left)
		prefix[prefix_len] = '|';
}

void print_node(Node *n, int prefix_len, int is_left)
{
	if (n != NULL)
	{
		print_prefix(prefix_len);

		printf("%s", (is_left ? "├── " : "└── "));

		print_kind(n->kind, n->value);

		new_prefix(prefix_len, is_left);
		prefix_len += 4;

		if (n->kind == K_STATEMENTS)
		{
			Node *buffer_node;
			for (int i = 0; i < n->node_list->length; ++i)
			{
				buffer_node = n->node_list->data[i];
				/*
				 * if node in node_list is last that
				 * is_left (n->node_list_length - (i + 1))
				 * is zero
				 */
				print_node(buffer_node, prefix_len, n->node_list->length - (i + 1));
			}
		}
		else
		{
			print_node(n->lhs, prefix_len, 1);
			print_node(n->rhs, prefix_len, 0);
		}
	}
}

void start_print_node(Node *n)
{
	prefix = NULL;

	print_kind(n->kind, n->value);

	print_node(n->rhs, 0, 0);
}

