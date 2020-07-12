#include "../include/gen.h"

FILE *output_file;

static int gen_expr(Node *node)
{
	if (node != NULL)
	{
		int reg1 = gen_expr(node->lhs);
		int reg2 = gen_expr(node->rhs);

		switch (node->kind)
		{
			case K_ADD:
				return cg_add(reg1, reg2);
			case K_SUB:
				return cg_sub(reg1, reg2);
			case K_MULT:
				return cg_mult(reg1, reg2);
			case K_DIV:
				return cg_div(reg1, reg2);
			case K_MOD:
				return cg_mod(reg1, reg2);
			case K_NEG:
				return cg_neg(reg2);
			case K_NUM:
				return cg_load(node->value);
			default:
				error(0, 0, "unknown ast kind");
		}
	}
	return -1;
}

static void gen_print(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];
		switch (buf_node->kind)
		{
			case K_EXPR:
				cg_print_int(gen_expr(buf_node->rhs));
				break;
			case K_STRING:
				cg_print_str(buf_node->value);
				break;
		}
	}
}

static void gen_statements(Node *node)
{
	for (int i = 0; i < node->node_list->length; ++i)
	{
		Node *buf_node = node->node_list->data[i];
		switch (buf_node->kind)
		{
			case K_EXPR:
				gen_expr(buf_node->rhs);
				break;
			case K_PRINT:
				gen_print(buf_node->node_list);
				break;
		}
	}
}

void gen(Node *tree)
{
	output_file = fopen("output.s", "w+");

	if (output_file == NULL)
		error(0, 0, "file output.asm can't be created");

	cg_start();
	gen_statements(tree->rhs);
	cg_end();
}