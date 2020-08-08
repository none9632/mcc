#include "../include/gen.h"

#define MAX_STR_SIZE 25

FILE  *output_file;
size_t offset;
int    ret_label;
int    l_count;

static void gen_statements (Vector *node_list);
static int  gen_expr       (Node *node);

static int get_label()
{
	return l_count++;
}

static int is_assignment_op(int kind)
{
	return (kind == K_ASSIGN ||
		    kind == K_ADDA   ||
		    kind == K_SUBA   ||
		    kind == K_MULTA  ||
		    kind == K_DIVA   ||
		    kind == K_MODA   );
}

static int gen_func_call(Node *node)
{
	Node *params  = node->rhs;
	int  *buf_frl = cg_save_all_reg();

	for (int i = params->u.node_list->length - 1; i >= 0; --i)
	{
		Node *buf_node = params->u.node_list->data[i];
		int reg = gen_expr(buf_node);
		cg_push_stack(reg);
	}

	cg_func_call(node->symbol->name);

	for (int i = 0; i < params->u.node_list->length; ++i)
		cg_pop_stack();

	int ret_val = cg_ret_all_reg(buf_frl);
	free(buf_frl);

	return ret_val;
}

static int gen_assign_stmt(Node *node)
{
	int reg1, reg2;
	char *pointer = node->u.lhs->symbol->pointer;

	reg1 = gen_expr(node->rhs);
	if (node->kind != K_ASSIGN)
		reg2 = cg_load_gsym(pointer);

	switch (node->kind)
	{
		case K_ADDA:  reg1 = cg_add(reg1, reg2);  break;
		case K_SUBA:  reg1 = cg_sub(reg2, reg1);  break;
		case K_MULTA: reg1 = cg_mult(reg1, reg2); break;
		case K_DIVA:  reg1 = cg_div(reg2, reg1);  break;
		case K_MODA:  reg1 = cg_mod(reg2, reg1);  break;
	}

	return cg_store_gsym(reg1, pointer);
}

static int gen_expr(Node *node)
{
	if (node != NULL)
	{
		if (node->kind == K_FUNC)
			return gen_func_call(node);

		if (is_assignment_op(node->kind))
			return gen_assign_stmt(node);

		int reg1 = gen_expr(node->u.lhs);
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
				return cg_load_gsym(node->symbol->pointer);
			default:
				error(0, 0, "unknown ast kind");
		}
	}

	// 'return -1' will never be used. This way there will be no warning
	return -1;
}

static void gen_init_vars(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i, ++offset)
	{
		Node *buf_node = node_list->data[i];
		char *str      = malloc(sizeof(char) * MAX_STR_SIZE);
		sprintf(str, "%li(%%rsp)", offset * 8);

		if (buf_node->kind == K_VAR)
		{
			buf_node->symbol->pointer = str;
			cg_uninit_var(str);
		}
		else
		{
			buf_node->u.lhs->symbol->pointer = str;
			int reg = gen_expr(buf_node->rhs);
			cg_store_gsym(reg, str);
			free_reg(reg);
		}
	}
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
		cg_input(buf_node->symbol->pointer);
	}
}

static void gen_if(Node *node)
{
	int reg   = gen_expr(node->u.lhs);
	int label = get_label();

	cg_condit_jmp(reg, label);
	gen_statements(node->rhs->u.node_list);
	cg_label(label);
}

static void gen_if_else(Node *node)
{
	Node *n_if   = node->u.lhs;
	Node *n_else = node->rhs;

	int   reg    = gen_expr(n_if->u.lhs);
	int   label1 = get_label();
	int   label2 = get_label();

	cg_condit_jmp(reg, label1);
	gen_statements(n_if->rhs->u.node_list);
	cg_jmp(label2);
	cg_label(label1);
	gen_statements(n_else->rhs->u.node_list);
	cg_label(label2);
}

static void gen_while(Node *node)
{
	int label1 = get_label();
	int label2 = get_label();
	int reg;

	cg_label(label1);
	reg = gen_expr(node->u.lhs);
	cg_condit_jmp(reg, label2);
	gen_statements(node->rhs->u.node_list);
	cg_jmp(label1);
	cg_label(label2);
}

static void gen_do_while(Node *node)
{
	int label1 = get_label();
	int label2 = get_label();
	int reg;

	cg_label(label1);
	gen_statements(node->u.lhs->u.node_list);
	reg = gen_expr(node->rhs);
	cg_condit_jmp(reg, label2);
	cg_jmp(label1);
	cg_label(label2);
}

static void gen_for(Vector *node_list)
{
	Node *arg1 = node_list->data[0];
	Node *arg2 = node_list->data[1];
	Node *arg3 = node_list->data[2];
	Node *stmt = node_list->data[3];

	int label1 = get_label();
	int label2 = get_label();

	if (arg1->kind == K_INIT_VARS)
		gen_init_vars(arg1->u.node_list);
	else if (arg1->kind == K_EXPR)
		gen_expr(arg1->rhs);

	cg_label(label1);

	if (arg2->kind == K_EXPR)
	{
		int reg = gen_expr(arg2->rhs);
		cg_condit_jmp(reg, label2);
	}

	gen_statements(stmt->u.node_list);

	if (arg3->kind == K_EXPR)
		gen_expr(arg3->rhs);

	cg_jmp(label1);
	cg_label(label2);
}

static void gen_return(Node *node)
{
	int reg = gen_expr(node->rhs);

	cg_ret_func(reg, ret_label);
}

static void gen_statements(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];
		int   reg;
		switch (buf_node->kind)
		{
			case K_INIT_VARS:
				gen_init_vars(buf_node->u.node_list);
				break;
			case K_EXPR:
				reg = gen_expr(buf_node->rhs);
				free_reg(reg);
				break;
			case K_PRINT:
				gen_print(buf_node->u.node_list);
				break;
			case K_INPUT:
				gen_input(buf_node->u.node_list);
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
			case K_FOR:
				gen_for(buf_node->u.node_list);
				break;
			case K_RETURN:
				gen_return(buf_node);
				break;
		}
	}
}

static void gen_init_params(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];
		char *str      = malloc(sizeof(char) * MAX_STR_SIZE);

		sprintf(str, "%i(%%rbp)", (i + 2) * 8);
		buf_node->symbol->pointer = str;
	}
}

static void gen_func(Vector *node_list)
{
	for (int i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		ret_label = get_label();
		offset    = 0;

		cg_start_func(buf_node->symbol->name, buf_node->value);

		if (buf_node->u.lhs->kind == K_INIT_PARAMS)
			gen_init_params(buf_node->u.lhs->u.node_list);

		if (buf_node->rhs->kind == K_STATEMENTS)
			gen_statements(buf_node->rhs->u.node_list);

		cg_end_func(ret_label, buf_node->value);
	}
}

void gen(Node *tree)
{
	output_file = fopen("output.s", "w+");
	l_count     = 0;

	if (output_file == NULL)
		func_error();

	cg_start_prog();
	gen_func(tree->u.node_list);
}