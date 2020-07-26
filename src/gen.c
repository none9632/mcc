#include "../include/gen.h"

FILE *output_file;
int   l_count;

static void gen_statements(Node *node);

static int get_label()
{
	return l_count++;
}

static int gen_expr(Node *node)
{
	if (node != NULL)
	{
		int reg1 = gen_expr(node->lhs);
		int reg2 = gen_expr(node->rhs);

		switch (node->kind)
		{
			case K_OR:
				return cg_or(reg1, reg2, get_label(), get_label());
			case K_AND:
				return cg_and(reg1, reg2, get_label(), get_label());
			case K_EQUAL:
				return cg_compare(reg1, reg2, "sete");
			case K_NOT_EQUAL:
				return cg_compare(reg1, reg2, "setne");
			case K_MORE:
				return cg_compare(reg1, reg2, "setg");
			case K_LESS:
				return cg_compare(reg1, reg2, "setl");
			case K_MOREEQ:
				return cg_compare(reg1, reg2, "setge");
			case K_LESSEQ:
				return cg_compare(reg1, reg2, "setle");
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
			case K_VAR:
				return cg_load_gsym(node->symbol->name);
			default:
				error(0, 0, "unknown ast kind");
		}
	}

	return -1;
}

static void gen_init_vars(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		if (buf_node->kind == K_VAR)
		{
			char *name = buf_node->symbol->name;
			cg_gsym(name);
		}
		else
		{
			char *name = buf_node->lhs->symbol->name;
			int   reg  = gen_expr(buf_node->rhs);
			cg_gsym(name);
			cg_store_gsym(reg, name);
		}
	}
}

static void gen_assign(Node *node)
{
	char *name = node->lhs->symbol->name;
	int   reg1 = gen_expr(node->rhs);
	int   reg2;

	if (node->kind != K_ASSIGN)
		reg2 = cg_load_gsym(name);

	switch (node->kind)
	{
		case K_ADDA:  reg1 = cg_add(reg1, reg2);  break;
		case K_SUBA:  reg1 = cg_sub(reg2, reg1);  break;
		case K_MULTA: reg1 = cg_mult(reg1, reg2); break;
		case K_DIVA:  reg1 = cg_div(reg2, reg1);  break;
		case K_MODA:  reg1 = cg_mod(reg2, reg1);  break;
	}

	cg_store_gsym(reg1, name);
}

static void gen_print(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];
		int   reg;

		switch (buf_node->kind)
		{
			case K_EXPR:
				reg = gen_expr(buf_node->rhs);
				cg_print_int(reg);
				break;
			case K_STRING:
				cg_print_str(buf_node->value);
				break;
		}
	}
}

static void gen_input(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];
		cg_input(buf_node->symbol->name);
	}
}

static void gen_if(Node *node)
{
	int reg   = gen_expr(node->lhs);
	int label = get_label();

	cg_condit_jmp(reg, label);
	gen_statements(node->rhs);
	cg_label(label);
}

static void gen_if_else(Node *node)
{
	Node *n_if   = node->lhs;
	Node *n_else = node->rhs;
	int   reg    = gen_expr(n_if->lhs);
	int   label1 = get_label();
	int   label2 = get_label();

	cg_condit_jmp(reg, label1);
	gen_statements(n_if->rhs);
	cg_jmp(label2);
	cg_label(label1);
	gen_statements(n_else->rhs);
	cg_label(label2);
}

static void gen_while(Node *node)
{
	int label1 = get_label();
	int label2 = get_label();
	int reg;

	cg_label(label1);
	reg = gen_expr(node->lhs);
	cg_condit_jmp(reg, label2);
	gen_statements(node->rhs);
	cg_jmp(label1);
	cg_label(label2);
}

static void gen_do_while(Node *node)
{
	int label1 = get_label();
	int label2 = get_label();
	int reg;

	cg_label(label1);
	gen_statements(node->lhs);
	reg = gen_expr(node->rhs);
	cg_condit_jmp(reg, label2);
	cg_jmp(label1);
	cg_label(label2);
}

static void gen_statements(Node *node)
{
	for (int i = 0; i < node->node_list->length; ++i)
	{
		Node *buf_node = node->node_list->data[i];
		switch (buf_node->kind)
		{
			case K_INIT_VARS:
				gen_init_vars(buf_node->node_list);
				break;
			case K_ADDA:
			case K_SUBA:
			case K_MULTA:
			case K_DIVA:
			case K_MODA:
			case K_ASSIGN:
				gen_assign(buf_node);
				break;
			case K_PRINT:
				gen_print(buf_node->node_list);
				break;
			case K_INPUT:
				gen_input(buf_node->node_list);
				break;
			case K_IF_ELSE:
				gen_if_else(buf_node);
				break;
			case K_IF:
				gen_if(buf_node);
				break;
			case K_WHILE:
				gen_while(buf_node);
				break;
			case K_DO_WHILE:
				gen_do_while(buf_node);
				break;
		}
	}
}

void gen(Node *tree)
{
	output_file = fopen("output.s", "w+");
	l_count     = 0;

	if (output_file == NULL)
		error(0, 0, "file output.asm can't be created");

	cg_start();
	gen_statements(tree->rhs);
	cg_end();
}