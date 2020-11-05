#include <stdio.h>

#include "../include/vector.h"
#include "../include/table.h"
#include "../include/lexer.h"
#include "../include/error.h"
#include "../include/node.h"

#define GLOBAL_MODE 0
#define LOCAL_MODE 1

Vector *string_list;

static Vector *tokens;
static Table *symbol_table;
static size_t count_tk;
static int size_local_vars;

static Node *expr();
static Node *statements();

static Node *make_sum_node(u_int8_t kind, Symbol *symbol)
{
	return new_node(kind, 0, symbol, NULL, NULL);
}

static Node *make_list_node(u_int8_t kind)
{
	return new_node(kind, 0, NULL, new_vector(), NULL);
}

static Node *make_bin_node(u_int8_t kind, Node *lhs, Node *rhs)
{
	return new_node(kind, 0, NULL, lhs, rhs);
}

static Node *make_node(u_int8_t kind)
{
	return new_node(kind, 0, NULL, NULL, NULL);
}

static Token *expect_tok(u_int8_t type)
{
	Token *token = tokens->data[count_tk];

	if (token->type != type)
	{
		switch (type)
		{
			case TK_IDENT:
				error(token, "expected identifier");
				break;
			case TK_WHILE:
				error(token, "expected 'while'");
				break;
			case TK_INT:
				error(token, "expected type specifier");
				break;
			case TK_STR:
				error(token, "expected string argument");
				break;
			default:
				error(token, "expected '%c' character", type);
				break;
		}
	}

	return tokens->data[++count_tk];
}

static int check_tok(u_int8_t type)
{
	Token *token = tokens->data[count_tk];

	if (token->type == type)
	{
		++count_tk;
		return 1;
	}

	return 0;
}

static Symbol *find_symbol(Token *token)
{
	if (token->type != TK_IDENT)
		error(token, "expected identifier");

	Symbol *symbol = find_all(symbol_table, token->str);

	if (symbol == NULL)
		error(token, "'%s' is undeclared", token->str);

	return symbol;
}

static void is_redefinition(Token *token)
{
	expect_tok(TK_IDENT);
	if (find(symbol_table, token->str) != NULL)
		error(token, "redefinition of '%s'", token->str);
}

static int8_t is_assignment_op()
{
	Token *token = tokens->data[count_tk];

	return (token->type == TK_PLUSA        ||
		    token->type == TK_MINUSA       ||
			token->type == TK_MULTA        ||
			token->type == TK_DIVA         ||
			token->type == TK_MODA         ||
			token->type == TK_BIT_ANDA     ||
			token->type == TK_BIT_XORA     ||
			token->type == TK_BIT_ORA      ||
			token->type == TK_LEFT_SHIFTA  ||
			token->type == TK_RIGHT_SHIFTA ||
			token->type == '='             );
}

static Node *params(int num_params)
{
	Node *node = make_list_node(K_PARAMS);

	do
	{
		--num_params;
		if (num_params < 0)
			break;

		vec_push(node->u.node_list, expr());
	}
	while (check_tok(','));

	Token *token = tokens->data[count_tk];

	if (num_params < 0)
		error(token, "too many arguments to function call");
	else if (num_params > 0)
		error(token, "too few arguments to function call");

	return node;
}

static Node *factor()
{
	Token *token = tokens->data[count_tk];
	Node *node = NULL;

	if (check_tok(TK_NUM))
	{
		node = make_node(K_NUM);
		node->value = token->value;
	}
	else if (check_tok(TK_IDENT))
	{
		Symbol *symbol = find_symbol(token);

		if (symbol->type == S_VAR)
		{
			node = make_sum_node(K_VAR, symbol);
		}
		else if (symbol->type == S_GVAR)
		{
			node = make_sum_node(K_GVAR, symbol);
		}
		else
		{
			node = make_sum_node(K_CALL_FUNC, symbol);
			token = expect_tok('(');
			if (token->type != ')' && token->type != TK_EOF)
				node->rhs = params(symbol->value);
			expect_tok(')');
		}
	}
	else if (check_tok('('))
	{
		node = expr();
		expect_tok(')');
	}
	else
	{
		error(token, "expected expression");
	}

	return node;
}

static Node *postfix()
{
	Node *node = factor();
	Token *token = tokens->data[count_tk];

	if (check_tok(TK_INC))
	{
		node = make_bin_node(K_POST_INC, NULL, node);
		node->symbol = node->rhs->symbol;
		if (node->rhs->kind != K_VAR && node->rhs->kind != K_GVAR)
			error(token, "lvalue required as increment operand");
	}
	else if (check_tok(TK_DEC))
	{
		node = make_bin_node(K_POST_DEC, NULL, node);
		node->symbol = node->rhs->symbol;
		if (node->rhs->kind != K_VAR && node->rhs->kind != K_GVAR)
			error(token, "lvalue required as decrement operand");
	}

	return node;
}

static Node *unary()
{
	Node *node = NULL;
	Token *token = tokens->data[count_tk];

	if (check_tok('+'))
	{
		node = unary();
	}
	else if (check_tok('-'))
	{
		node = make_bin_node(K_NEG, NULL, unary());
	}
	else if (check_tok('&'))
	{
		node = make_bin_node(K_ADDRESS, NULL, postfix());
		if (node->rhs->kind != K_VAR && node->rhs->kind != K_GVAR)
			error(token, "lvalue required as unary '&' operand");
	}
	else if (check_tok('!'))
	{
		node = make_bin_node(K_LOG_NOT, NULL, unary());
	}
	else if (check_tok('~'))
	{
		node = make_bin_node(K_BIT_NOT, NULL, unary());
	}
	else if (check_tok(TK_INC))
	{
		node = make_bin_node(K_PRE_INC, NULL, postfix());
		if (node->rhs->kind != K_VAR && node->rhs->kind != K_GVAR)
			error(token, "lvalue required as increment operand");
	}
	else if (check_tok(TK_DEC))
	{
		node = make_bin_node(K_PRE_DEC, NULL, postfix());
		if (node->rhs->kind != K_VAR && node->rhs->kind != K_GVAR)
			error(token, "lvalue required as decrement operand");
	}
	else
	{
		node = postfix();
	}

	return node;
}

static Node *mult()
{
	Node *node = unary();
	Token *token = tokens->data[count_tk];

	while (check_tok('*') ||
		   check_tok('/') ||
		   check_tok('%') )
	{
		node = make_bin_node(K_NONE, node, unary());
		switch (token->type)
		{
			case '*': node->kind = K_MULT; break;
			case '/': node->kind = K_DIV;  break;
			case '%': node->kind = K_MOD;  break;
		}

		token = tokens->data[count_tk];
	}

	return node;
}

static Node *add()
{
	Node *node = mult();
	Token *token = tokens->data[count_tk];

	while (check_tok('+') ||
		   check_tok('-') )
	{
		node = make_bin_node(K_NONE, node, mult());

		switch (token->type)
		{
			case '+': node->kind = K_ADD; break;
			case '-': node->kind = K_SUB; break;
		}

		token = tokens->data[count_tk];
	}

	return node;
}

static Node *shift()
{
	Node *node = add();
	Token *token = tokens->data[count_tk];

	while (check_tok(TK_LEFT_SHIFT)  ||
		   check_tok(TK_RIGHT_SHIFT) )
	{
		node = make_bin_node(K_NONE, node, add());

		switch (token->type)
		{
			case TK_LEFT_SHIFT:  node->kind = K_LEFT_SHIFT; break;
			case TK_RIGHT_SHIFT: node->kind = K_RIGHT_SHIFT; break;
		}

		token = tokens->data[count_tk];
	}

	return node;
}

static Node *comp_op()
{
	Node *node = shift();
	Token *token = tokens->data[count_tk];

	while (check_tok('>')       ||
		   check_tok('<')       ||
		   check_tok(TK_MOREEQ) ||
		   check_tok(TK_LESSEQ) )
	{
		node = make_bin_node(K_NONE, node, add());

		switch (token->type)
		{
			case '>':       node->kind = K_MORE;   break;
			case '<':       node->kind = K_LESS;   break;
			case TK_MOREEQ: node->kind = K_MOREEQ; break;
			case TK_LESSEQ: node->kind = K_LESSEQ; break;
		}

		token = tokens->data[count_tk];
	}

	return node;
}

static Node *equality_op()
{
	Node *node = comp_op();
	Token *token = tokens->data[count_tk];

	while (check_tok(TK_EQUAL) ||
		   check_tok(TK_NOTEQ) )
	{
		node = make_bin_node(K_NONE, node, comp_op());

		switch (token->type)
		{
			case TK_EQUAL: node->kind = K_EQUAL;     break;
			case TK_NOTEQ: node->kind = K_NOT_EQUAL; break;
		}

		token = tokens->data[count_tk];
	}

	return node;
}

static Node *bit_and()
{
	Node *node = equality_op();

	while (check_tok('&'))
		node = make_bin_node(K_BIT_AND, node, equality_op());

	return node;
}

static Node *bit_xor()
{
	Node *node = bit_and();

	while (check_tok('^'))
		node = make_bin_node(K_BIT_XOR, node, bit_and());

	return node;
}

static Node *bit_or()
{
	Node *node = bit_xor();

	while (check_tok('|'))
		node = make_bin_node(K_BIT_OR, node, bit_xor());

	return node;
}

static Node *and()
{
	Node *node = bit_or();

	while (check_tok(TK_AND))
		node = make_bin_node(K_AND, node, equality_op());

	return node;
}

static Node *or()
{
	Node *node = and();

	while (check_tok(TK_OR))
		node = make_bin_node(K_OR, node, and());

	return node;
}

static Node *assign()
{
	Token *token = tokens->data[count_tk++];
	Node *node;

	if (token->type == TK_IDENT && is_assignment_op())
	{
		Symbol *symbol = find_symbol(token);
		token = tokens->data[count_tk++];

		switch (token->type)
		{
			case TK_PLUSA:        node = make_node(K_ADDA);         break;
			case TK_MINUSA:       node = make_node(K_SUBA);         break;
			case TK_MULTA:        node = make_node(K_MULTA);        break;
			case TK_DIVA:         node = make_node(K_DIVA);         break;
			case TK_MODA:         node = make_node(K_MODA);         break;
			case TK_BIT_ANDA:     node = make_node(K_BIT_ANDA);     break;
			case TK_BIT_XORA:     node = make_node(K_BIT_XORA);     break;
			case TK_BIT_ORA:      node = make_node(K_BIT_ORA);      break;
			case TK_LEFT_SHIFTA:  node = make_node(K_LEFT_SHIFTA);  break;
			case TK_RIGHT_SHIFTA: node = make_node(K_RIGHT_SHIFTA); break;
			case '=':             node = make_node(K_ASSIGN);       break;
		}

		if (symbol->type == S_GVAR)
			node->u.lhs = make_sum_node(K_GVAR, symbol);
		else
			node->u.lhs = make_sum_node(K_VAR, symbol);

		node->rhs = assign();
	}
	else
	{
		--count_tk;
		node = or();
	}

	return node;
}

static Node *expr()
{
	Node *node = assign();

	if (is_assignment_op())
	{
		Token *token = tokens->data[count_tk];
		error(token, "lvalue required as left operand of assignment");
	}

	return node;
}

static Node *primary_expr()
{
	Node *node = make_bin_node(K_EXPR, NULL, expr());

	return node;
}

static Node *parse_expr_stmt()
{
	Node *node = primary_expr();

	expect_tok(';');
	return node;
}

static Node *parse_init_vars(int8_t mode)
{
	++count_tk;

	Node *node = make_list_node(K_INIT_VARS);

	do
	{
		Token *token = tokens->data[count_tk];
		is_redefinition(token);

		Symbol *symbol = new_symbol(S_GVAR + mode, token->str, symbol_table);
		Node *var = make_sum_node(K_GVAR + mode, symbol);

		size_local_vars += 4;

		if (check_tok('='))
			var = make_bin_node(K_ASSIGN, var, expr());

		vec_push(node->u.node_list, var);
	}
	while (check_tok(','));

	return node;
}

static Node *parse_vars_stmt(int8_t mode)
{
	Node *node = parse_init_vars(mode);

	expect_tok(';');
	return node;
}

static Node *parse_if_else()
{
	++count_tk;

	Node *node = make_node(K_IF);

	expect_tok('(');
	node->u.lhs = expr();
	expect_tok(')');
	node->rhs = statements();

	if (check_tok(TK_ELSE))
	{
		Node *n_else = make_bin_node(K_ELSE, NULL, statements());
		node = make_bin_node(K_IF_ELSE, node, n_else);
	}

	return node;
}

static Node *parse_while()
{
	++count_tk;

	Node *node = make_node(K_WHILE);

	expect_tok('(');
	node->u.lhs = expr();
	expect_tok(')');
	node->rhs = statements();

	return node;
}

static Node *parse_do_while()
{
	++count_tk;

	Node *node = make_node(K_DO_WHILE);

	node->u.lhs = statements();
	expect_tok(TK_WHILE);
	expect_tok('(');
	node->rhs = expr();
	expect_tok(')');
	expect_tok(';');

	return node;
}

static Node *parse_for()
{
	++count_tk;

	Token *token = expect_tok('(');
	Node *node = make_list_node(K_FOR);

	symbol_table = new_table(symbol_table);

	if (token->type == TK_INT)
		vec_push(node->u.node_list, parse_init_vars(LOCAL_MODE));
	else if (token->type != ';' && token->type != TK_EOF)
		vec_push(node->u.node_list, primary_expr());
	else
		vec_push(node->u.node_list, make_node(K_NONE));

	token = expect_tok(';');

	if (token->type != ';' && token->type != TK_EOF)
		vec_push(node->u.node_list, primary_expr());
	else
		vec_push(node->u.node_list, make_node(K_NONE));

	token = expect_tok(';');

	if (token->type != ')' && token->type != TK_EOF)
		vec_push(node->u.node_list, primary_expr());
	else
		vec_push(node->u.node_list, make_node(K_NONE));

	expect_tok(')');

	vec_push(node->u.node_list, statements());

	symbol_table = symbol_table->prev;

	return node;
}

static Node *parse_printf()
{
	++count_tk;

	Node *node = make_list_node(K_PRINTF);
	Token *token = expect_tok('(');

	expect_tok(TK_STR);

	Node *string = make_node(K_STRING);
	string->value = string_list->length;

	vec_push(string_list, token->str);
	vec_push(node->u.node_list, string);

	while (check_tok(','))
		vec_push(node->u.node_list, expr());

	expect_tok(')');
	expect_tok(';');

	return node;
}

static Node *parse_scanf()
{
	++count_tk;

	Node *node = make_list_node(K_SCANF);

	expect_tok('(');

	do
	{
		Token *token = tokens->data[count_tk++];
		Symbol *symbol = find_symbol(token);
		Node *buf_node;

		if (symbol->type == S_GVAR)
			buf_node = make_sum_node(K_GVAR, symbol);
		else
			buf_node = make_sum_node(K_VAR, symbol);

		vec_push(node->u.node_list, buf_node);
	}
	while (check_tok(','));

	expect_tok(')');
	expect_tok(';');

	return node;
}

static Node *parse_ret_stmt()
{
	++count_tk;

	Node *node = make_bin_node(K_RETURN, NULL, expr());

	expect_tok(';');

	return node;
}

static Node *statement()
{
	Token *token = tokens->data[count_tk];
	Node *node = NULL;

	switch (token->type)
	{
		case TK_IDENT:
		case TK_NUM:
		case '(':
		case '-':
		case '+':
		case TK_INC:
		case TK_DEC:
			node = parse_expr_stmt();
			break;
		case TK_INT:
			node = parse_vars_stmt(LOCAL_MODE);
			break;
		case TK_IF:
			node = parse_if_else();
			break;
		case TK_WHILE:
			node = parse_while();
			break;
		case TK_DO:
			node = parse_do_while();
			break;
		case TK_FOR:
			node = parse_for();
			break;
		case TK_PRINTF:
			node = parse_printf();
			break;
		case TK_SCANF:
			node = parse_scanf();
			break;
		case TK_RETURN:
			node = parse_ret_stmt();
			break;
		case ';':
			++count_tk;
			break;
		default:
			error(token, "wrong statement");
			break;
	}

	return node;
}

static Node *statements()
{
	Token *token = expect_tok('{');
	Node *node = make_list_node(K_STATEMENTS);

	symbol_table = new_table(symbol_table);

	while (token->type != '}' && token->type != TK_EOF)
	{
		Node *returned_node = statement();

		if (returned_node != NULL)
			vec_push(node->u.node_list, returned_node);

		token = tokens->data[count_tk];
	}

	expect_tok('}');

	symbol_table = symbol_table->prev;

	return node;
}

static Node *init_params(int *num_params)
{
	Node *node = make_list_node(K_INIT_PARAMS);

	do
	{
		Token *token = expect_tok(TK_INT);
		is_redefinition(token);

		Symbol *symbol = new_symbol(S_VAR, token->str, symbol_table);
		Node *var = make_sum_node(K_VAR, symbol);

		vec_push(node->u.node_list, var);

		*num_params += 1;
	}
	while (check_tok(','));

	return node;
}

static Node *parse_func()
{
	Token *token = expect_tok(TK_INT);
	is_redefinition(token);

	Symbol *symbol = new_symbol(S_FUNC, token->str, symbol_table);
	Node *node = make_sum_node(K_FUNC, symbol);

	symbol_table = new_table(symbol_table);

	token = expect_tok('(');

	if (token->type == TK_INT)
		node->u.lhs = init_params(&symbol->value);
	else
		node->u.lhs = make_node(K_NONE);

	expect_tok(')');

	size_local_vars = 0;
	node->rhs = statements();
	// size_local_vars must be a multiple of 16
	node->value = (size_local_vars / 16 + (size_local_vars % 16 != 0)) * 16;

	symbol_table = symbol_table->prev;

	return node;
}

static Node *parse_prog()
{
	Token *token = tokens->data[count_tk];
	Node *node = make_list_node(K_PROGRAM);

	while (token->type != TK_EOF)
	{
		/*
		 * In this case it checks whether the token we want to check exists.
		 * int <name> (
		 *            ^
		 * If it is not exist, it does not matter what is called parse_func()
		 * or parse_vars_stmt(), the result is the same in both cases.
		 */
		if (count_tk + 2 < tokens->length - 1)
			token = tokens->data[count_tk + 2];

		if (token->type == '(')
			vec_push(node->u.node_list, parse_func());
		else
			vec_push(node->u.node_list, parse_vars_stmt(GLOBAL_MODE));

		token = tokens->data[count_tk];
	}

	return node;
}

Node *parsing(Vector *_tokens)
{
	tokens = _tokens;
	count_tk = 0;
	string_list = new_vector();
	symbol_table = new_table(NULL);

	Node *tree = parse_prog();

	free_tokens(tokens);
	return tree;
}
