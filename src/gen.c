#include <stdlib.h>
#include <stdio.h>

#include "gen_x86_64.h"
#include "node.h"
#include "error.h"
#include "vector.h"

FILE *output_file;
uint var_offset;
uint ret_label;

static void   gen_statements (Vector *node_list);
static int8_t gen_expr       (Node *node);

static uint get_label()
{
	static uint label_count = 0;

	return label_count++;
}

static int8_t is_assignment_op(u_int8_t kind)
{
	return (kind == K_ASSIGN       ||
		    kind == K_ADDA         ||
		    kind == K_SUBA         ||
		    kind == K_MULTA        ||
		    kind == K_DIVA         ||
		    kind == K_MODA         ||
			kind == K_BIT_ANDA     ||
			kind == K_BIT_XORA     ||
			kind == K_BIT_ORA      ||
			kind == K_LEFT_SHIFTA  ||
			kind == K_RIGHT_SHIFTA );
}

static int8_t gen_func_call(Node *node)
{
	Node *params = node->rhs;
	int8_t *buf_frl = cg_save_all_reg();
	int num_params = 0;

	if (params != NULL)
		num_params = params->u.node_list->length;

	for (int i = num_params - 1; i >= 0; --i)
	{
		Node *buf_node = params->u.node_list->data[i];
		int8_t reg = gen_expr(buf_node);
		cg_push_stack(reg);
	}

	cg_func_call(node->symbol->name);

	for (int i = 0; i < num_params; ++i)
		cg_pop_stack();

	cg_ret_all_reg(buf_frl);
	return cg_ret_value();
}

static int8_t gen_assign_stmt(Node *node)
{
	char *pointer = node->u.lhs->symbol->pointer;
	char *name = node->u.lhs->symbol->name;
	uint offset = node->u.lhs->symbol->value;
	int8_t reg1, reg2;

	reg1 = gen_expr(node->rhs);
	if (node->kind != K_ASSIGN)
	{
		if (node->u.lhs->kind == K_GVAR)
			reg2 = cg_load_gvar(name);
		else
			reg2 = cg_load_var(pointer, offset);
	}

	switch (node->kind)
	{
		case K_ADDA:         reg1 = cg_add(reg1, reg2);         break;
		case K_SUBA:         reg1 = cg_sub(reg2, reg1);         break;
		case K_MULTA:        reg1 = cg_mult(reg1, reg2);        break;
		case K_DIVA:         reg1 = cg_div(reg2, reg1);         break;
		case K_MODA:         reg1 = cg_mod(reg2, reg1);         break;
		case K_BIT_ANDA:     reg1 = cg_bit_and(reg2, reg1);     break;
		case K_BIT_XORA:     reg1 = cg_bit_xor(reg2, reg1);     break;
		case K_BIT_ORA:      reg1 = cg_bit_or(reg2, reg1);      break;
		case K_LEFT_SHIFTA:  reg1 = cg_left_shift(reg2, reg1);  break;
		case K_RIGHT_SHIFTA: reg1 = cg_right_shift(reg2, reg1); break;
	}

	if (node->u.lhs->kind == K_GVAR)
		return cg_store_gvar(reg1, name);
	else
		return cg_store_var(reg1, pointer, offset);
}

static int8_t gen_expr(Node *node)
{
	if (node != NULL)
	{
		if (node->kind == K_CALL_FUNC)
			return gen_func_call(node);

		if (is_assignment_op(node->kind))
			return gen_assign_stmt(node);

		int8_t reg1 = gen_expr(node->u.lhs);
		int8_t reg2 = gen_expr(node->rhs);

		switch (node->kind)
		{
			case K_OR:
				return cg_or(reg1, reg2, get_label(), get_label());
			case K_AND:
				return cg_and(reg1, reg2, get_label(), get_label());
			case K_BIT_OR:
				return cg_bit_or(reg1, reg2);
			case K_BIT_XOR:
				return cg_bit_xor(reg1, reg2);
			case K_BIT_AND:
				return cg_bit_and(reg1, reg2);
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
			case K_LEFT_SHIFT:
				return cg_left_shift(reg1, reg2);
			case K_RIGHT_SHIFT:
				return cg_right_shift(reg1, reg2);
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
			case K_PRE_INC:
				return cg_pre_inc(reg2, node->rhs->symbol->pointer, node->rhs->symbol->value);
			case K_PRE_DEC:
				return cg_pre_dec(reg2, node->rhs->symbol->pointer, node->rhs->symbol->value);
			case K_POST_INC:
				cg_post_inc(node->rhs->symbol->pointer, node->rhs->symbol->value);
				return reg2;
			case K_POST_DEC:
				cg_post_dec(node->rhs->symbol->pointer, node->rhs->symbol->value);
				return reg2;
			case K_NUM:
				return cg_load_num(node->value);
			case K_VAR:
				return cg_load_var(node->symbol->pointer, node->symbol->value);
			case K_GVAR:
				return cg_load_gvar(node->symbol->name);
			default:
				error(NULL, "unknown ast kind");
		}
	}

	// 'return -1' will never be used. This way there will be no warning
	return -1;
}

static void gen_init_vars(Vector *node_list)
{
	for (uint i = 0; i < node_list->length; ++i, ++var_offset)
	{
		Node *buf_node = node_list->data[i];

		if (buf_node->kind == K_VAR)
		{
			buf_node->symbol->pointer = "%rsp";
			buf_node->symbol->value = var_offset * 4;
		}
		else
		{
			buf_node->u.lhs->symbol->pointer = "%rsp";
			buf_node->u.lhs->symbol->value = var_offset * 4;

			int8_t reg = gen_expr(buf_node->rhs);
			cg_store_var(reg, "%rsp", buf_node->u.lhs->symbol->value);
			free_reg(reg);
		}
	}
}

static void gen_printf(Vector *node_list)
{
	for (uint i = node_list->length - 1; i > 0; --i)
	{
		Node *buf_node = node_list->data[i];
		int8_t reg1 = gen_expr(buf_node);

		cg_arg_printf(reg1, i);
	}

	Node *str = node_list->data[0];
	cg_printf(str->value, node_list->length - 1);
}

static void gen_scanf(Vector *node_list)
{
	for (uint i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		if (buf_node->kind == K_GVAR)
			cg_scanf(buf_node->symbol->name, 0, GLOBAL_MODE);
		else
			cg_scanf(buf_node->symbol->pointer, buf_node->symbol->value, LOCAL_MODE);
	}
}

static void gen_if(Node *node)
{
	int8_t reg = gen_expr(node->u.lhs);
	uint label = get_label();

	cg_condit_jmp(reg, label);
	gen_statements(node->rhs->u.node_list);
	cg_label(label);
}

static void gen_if_else(Node *node)
{
	Node *n_if = node->u.lhs;
	Node *n_else = node->rhs;

	int8_t reg = gen_expr(n_if->u.lhs);
	uint label1 = get_label();
	uint label2 = get_label();

	cg_condit_jmp(reg, label1);
	gen_statements(n_if->rhs->u.node_list);
	cg_jmp(label2);
	cg_label(label1);
	gen_statements(n_else->rhs->u.node_list);
	cg_label(label2);
}

static void gen_while(Node *node)
{
	uint label1 = get_label();
	uint label2 = get_label();
	int8_t reg;

	cg_label(label1);
	reg = gen_expr(node->u.lhs);
	cg_condit_jmp(reg, label2);
	gen_statements(node->rhs->u.node_list);
	cg_jmp(label1);
	cg_label(label2);
}

static void gen_do_while(Node *node)
{
	uint label1 = get_label();
	uint label2 = get_label();
	int8_t reg;

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

	uint label1 = get_label();
	uint label2 = get_label();

	if (arg1->kind == K_INIT_VARS)
		gen_init_vars(arg1->u.node_list);
	else if (arg1->kind == K_EXPR)
		gen_expr(arg1->rhs);

	cg_label(label1);

	if (arg2->kind == K_EXPR)
	{
		int8_t reg = gen_expr(arg2->rhs);
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
	int8_t reg = gen_expr(node->rhs);
	cg_ret_func(reg, ret_label);
}

static void gen_statements(Vector *node_list)
{
	for (uint i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		switch (buf_node->kind)
		{
			case K_INIT_VARS: gen_init_vars(buf_node->u.node_list); break;
			case K_EXPR:      free_reg(gen_expr(buf_node->rhs));    break;
			case K_PRINTF:    gen_printf(buf_node->u.node_list);    break;
			case K_SCANF:     gen_scanf(buf_node->u.node_list);     break;
			case K_IF_ELSE:   gen_if_else(buf_node);                break;
			case K_IF:        gen_if(buf_node);                     break;
			case K_WHILE:     gen_while(buf_node);                  break;
			case K_DO_WHILE:  gen_do_while(buf_node);               break;
			case K_FOR:       gen_for(buf_node->u.node_list);       break;
			case K_RETURN:    gen_return(buf_node);                 break;
		}
	}
}

static void gen_init_params(Vector *node_list)
{
	for (uint i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		buf_node->symbol->pointer = "%rbp";
		buf_node->symbol->value = (i + 2) * 8;
	}
}

static void gen_func(Node *node)
{
	ret_label = get_label();
	var_offset = 0;

	cg_start_func(node->symbol->name, node->value);

	if (node->u.lhs->kind == K_INIT_PARAMS)
		gen_init_params(node->u.lhs->u.node_list);

	if (node->rhs->kind == K_STATEMENTS)
		gen_statements(node->rhs->u.node_list);

	cg_end_func(ret_label, node->value);
}

static void gen_init_gvars(Vector *node_list)
{
	for (uint i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		if (buf_node->kind == K_GVAR)
		{
			cg_init_gvar(buf_node->symbol->name);
		}
		else
		{
			int8_t reg = gen_expr(buf_node->rhs);
			cg_init_gvar(buf_node->u.lhs->symbol->name);
			cg_store_gvar(reg, buf_node->u.lhs->symbol->name);
			free_reg(reg);
		}
	}
}

static void gen_prog(Vector *node_list)
{
	for (uint i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		if (buf_node->kind == K_INIT_VARS)
			gen_init_gvars(buf_node->u.node_list);
	}

	cg_end_start();

	for (uint i = 0; i < node_list->length; ++i)
	{
		Node *buf_node = node_list->data[i];

		if (buf_node->kind == K_FUNC)
			gen_func(buf_node);
	}
}

void gen(Node *tree)
{
	output_file = fopen("output.s", "w+");

	if (output_file == NULL)
		func_error();

	cg_start_prog();
	gen_prog(tree->u.node_list);

	fclose(output_file);
}