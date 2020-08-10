#include "../include/parser.h"

static Vector *tokens;
Vector        *string_list;
static Table  *symbol_table;
static int     count_tk;
static size_t  size_local_vars;

static Node *expr();
static Node *statements();

static Token *expect_tok(int type)
{
	Token *token = tokens->data[count_tk];

	if (token->type != type)
	{
		// To display the line and column correctly
		if (token->type == TK_EOF)
			token = tokens->data[count_tk - 1];

		switch (type)
		{
			case TK_IDENT:
				error(token->line, token->column, "expected identifier");
				break;
			case TK_WHILE:
				error(token->line, token->column, "expected 'while'");
				break;
			case TK_INT:
				if (token->str != NULL)
					error(token->line, token->column, "unknown type name '%s'", token->str);
				else
					error(token->line, token->column, "expected parameter declarator", token->str);
				break;
			default:
				error(token->line, token->column, "expected '%c' character", type);
				break;
		}
	}

	return tokens->data[++count_tk];
}

static int check_tok(int type)
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
	Symbol *symbol = find_all(symbol_table, token->str);

	if (symbol == NULL)
		error(token->line, token->column, "'%s' is undeclared", token->str);

	return symbol;
}

static int is_assignment_op()
{
	return (check_tok(TK_PLUSA)  ||
		    check_tok(TK_MINUSA) ||
		    check_tok(TK_MULTA)  ||
		    check_tok(TK_DIVA)   ||
		    check_tok(TK_MODA)   ||
		    check_tok('=')       );
}

static Node *params(int num_params)
{
	Node *node = new_node(K_PARAMS);
	node->u.node_list = new_vector();

	do
	{
		--num_params;
		if (num_params < 0)
			break;

		Node *buf_node = expr();
		vec_push(node->u.node_list, buf_node);
	}
	while (check_tok(','));

	Token *token = tokens->data[count_tk];

	if (num_params < 0)
		error(token->line, token->column, "too many arguments to function call");
	else if (num_params > 0)
		error(token->line, token->column, "too few arguments to function call");

	return node;
}

static Node *factor()
{
	Token *token = tokens->data[count_tk];
	Node  *node  = NULL;

	if (check_tok(TK_NUM))
	{
		node        = new_node(K_NUM);
		node->value = token->value;
	}
	else if (check_tok(TK_IDENT))
	{
		Symbol *symbol = find_symbol(token);

		if (symbol->type == S_VAR)
			node = new_node(K_VAR);
		else
		{
			token = expect_tok('(');
			node  = new_node(K_CALL_FUNC);
			if (token->type != ')' && token->type != TK_EOF)
				node->rhs = params(symbol->value);
			expect_tok(')');
		}

		node->symbol = symbol;
	}
	else if (check_tok('('))
	{
		node = expr();
		expect_tok(')');
	}
	else
		error(token->line, token->column, "expected expression");

	return node;
}

static Node *unary()
{
	Token *token = tokens->data[count_tk];
	Node  *node  = NULL;

	if (check_tok('-') ||
	    check_tok('+') )
	{
		switch (token->type)
		{
			case '+':
				node = unary();
				break;
			case '-':
				node      = new_node(K_NEG);
				node->rhs = unary();
				break;

		}
	}

	if (node == NULL)
		node = factor();

	return node;
}

static Node *mult()
{
	Node  *node  = unary();
	Token *token = tokens->data[count_tk];

	while (check_tok('*') ||
		   check_tok('/') ||
		   check_tok('%') )
	{
		Node *buf_node = new_node(K_NONE);

		buf_node->u.lhs = node;
		buf_node->rhs   = unary();
		node            = buf_node;

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
	Node  *node  = mult();
	Token *token = tokens->data[count_tk];

	while (check_tok('+') ||
		   check_tok('-') )
	{
		Node *buf_node = new_node(K_NONE);

		buf_node->u.lhs = node;
		buf_node->rhs   = mult();
		node            = buf_node;

		switch (token->type)
		{
			case '+': node->kind = K_ADD; break;
			case '-': node->kind = K_SUB; break;
		}

		token = tokens->data[count_tk];
	}

	return node;
}

// comparison operators
static Node *comp_op()
{
	Node  *node  = add();
	Token *token = tokens->data[count_tk];

	while (check_tok('>')       ||
		   check_tok('<')       ||
		   check_tok(TK_MOREEQ) ||
		   check_tok(TK_LESSEQ) )
	{
		Node *buf_node = new_node(K_NONE);

		buf_node->u.lhs = node;
		buf_node->rhs   = add();
		node            = buf_node;

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
	Node  *node  = comp_op();
	Token *token = tokens->data[count_tk];

	while (check_tok(TK_EQUAL) ||
		   check_tok(TK_NOTEQ) )
	{
		Node *buf_node = new_node(K_NONE);

		buf_node->u.lhs = node;
		buf_node->rhs   = comp_op();
		node            = buf_node;

		switch (token->type)
		{
			case TK_EQUAL: node->kind = K_EQUAL;     break;
			case TK_NOTEQ: node->kind = K_NOT_EQUAL; break;
		}

		token = tokens->data[count_tk];
	}

	return node;
}

static Node *and()
{
	Node *node = equality_op();

	while (check_tok(TK_AND))
	{
		Node *buf_node = new_node(K_AND);

		buf_node->u.lhs = node;
		buf_node->rhs   = equality_op();
		node            = buf_node;
	}

	return node;
}

static Node *or()
{
	Node *node = and();

	while (check_tok(TK_OR))
	{
		Node *buf_node = new_node(K_OR);

		buf_node->u.lhs = node;
		buf_node->rhs   = and();
		node            = buf_node;
	}

	return node;
}

static Node *assign()
{
	Token *token = tokens->data[count_tk++];
	Node  *node;

	if (is_assignment_op())
	{
		Symbol *symbol = find_symbol(token);
		token = tokens->data[count_tk - 1];

		switch (token->type)
		{
			case TK_PLUSA:  node = new_node(K_ADDA);   break;
			case TK_MINUSA: node = new_node(K_SUBA);   break;
			case TK_MULTA:  node = new_node(K_MULTA);  break;
			case TK_DIVA:   node = new_node(K_DIVA);   break;
			case TK_MODA:   node = new_node(K_MODA);   break;
			case '=':       node = new_node(K_ASSIGN); break;
		}

		node->u.lhs         = new_node(K_VAR);
		node->u.lhs->symbol = symbol;
		node->rhs           = assign();
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
		Token *token = tokens->data[count_tk - 1];
		error(token->line, token->column,
			  "lvalue required as left operand of assignment");
	}

	return node;
}

static Node *primary_expr()
{
	Node *node = new_node(K_EXPR);

	node->rhs = expr();

	return node;
}

static Node *parse_expr_stmt()
{
	Node *node = primary_expr();

	expect_tok(';');
	return node;
}

static Node *parse_init_vars()
{
	++count_tk;

	Node *node = new_node(K_INIT_VARS);
	node->u.node_list = new_vector();

	do
	{
		Symbol *symbol = new_symbol(S_VAR);
		Token  *token  = tokens->data[count_tk];
		Node   *var    = new_node(K_VAR);

		expect_tok(TK_IDENT);

		if (find(symbol_table, token->str) != NULL)
			error(token->line, token->column, "redefinition of '%s'", token->str);

		symbol->name     = token->str;
		var->symbol      = symbol;
		size_local_vars += 8;

		vec_push(symbol_table->symbols, symbol);

		if (check_tok('='))
		{
			Node *assign = new_node(K_ASSIGN);
			assign->u.lhs = var;
			assign->rhs   = expr();
			var           = assign;
		}

		vec_push(node->u.node_list, var);
	}
	while (check_tok(','));

	return node;
}

static Node *parse_vars_stmt()
{
	Node *node = parse_init_vars();

	expect_tok(';');
	return node;
}

static Node *parse_while()
{
	++count_tk;

	Node *node = new_node(K_WHILE);

	expect_tok('(');
	node->u.lhs = expr();
	expect_tok(')');

	node->rhs = statements();

	return node;
}

static Node *parse_do_while()
{
	++count_tk;

	Node *node = new_node(K_DO_WHILE);
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

	Node  *node  = new_node(K_FOR);
	Token *token = expect_tok('(');

	node->u.node_list = new_vector();
	symbol_table      = new_table(symbol_table);

	if (token->type == TK_INT)
		vec_push(node->u.node_list, parse_init_vars());
	else if (token->type != ';' && token->type != TK_EOF)
		vec_push(node->u.node_list, primary_expr());
	else
		vec_push(node->u.node_list, new_node(K_NONE));

	token = expect_tok(';');

	if (token->type != ';' && token->type != TK_EOF)
		vec_push(node->u.node_list, primary_expr());
	else
		vec_push(node->u.node_list, new_node(K_NONE));

	token = expect_tok(';');

	if (token->type != ')' && token->type != TK_EOF)
		vec_push(node->u.node_list, primary_expr());
	else
		vec_push(node->u.node_list, new_node(K_NONE));

	expect_tok(')');

	vec_push(node->u.node_list, statements());

	symbol_table = symbol_table->prev;

	return node;
}

static Node *parse_if_else()
{
	++count_tk;

	Node *node = new_node(K_IF);

	expect_tok('(');
	node->u.lhs = expr();
	expect_tok(')');

	node->rhs = statements();

	if (check_tok(TK_ELSE))
	{
		Node *buffer_node = node;

		node           = new_node(K_IF_ELSE);
		node->u.lhs    = buffer_node;
		node->rhs      = new_node(K_ELSE);
		node->rhs->rhs = statements();
	}

	return node;
}

static Node *parse_print()
{
	++count_tk;

	Node *node = new_node(K_PRINT);
	node->u.node_list = new_vector();

	expect_tok('(');

	do
	{
		if (check_tok(TK_IDENT) ||
			check_tok(TK_NUM)   ||
			check_tok('(')      ||
			check_tok('-')      ||
			check_tok('+')      )
		{
			--count_tk;
			vec_push(node->u.node_list, primary_expr());
		}
		else if (check_tok(TK_STR))
		{
			Token *token  = tokens->data[count_tk - 1];
			Node  *string = new_node(K_STRING);
			string->value = string_list->length;

			vec_push(string_list, token->str);
			vec_push(node->u.node_list, string);
		}
	}
	while (check_tok(','));

	expect_tok(')');
	expect_tok(';');

	return node;
}

static Node *parse_input()
{
	++count_tk;

	Node *node = new_node(K_INPUT);
	node->u.node_list = new_vector();

	expect_tok('(');

	do
	{
		Token *token    = tokens->data[count_tk];
		Node  *buf_node = new_node(K_VAR);

		expect_tok(TK_IDENT);

		buf_node->symbol = find_symbol(token);

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

	Node *node = new_node(K_RETURN);

	if (check_tok(TK_IDENT) ||
		check_tok(TK_NUM)   ||
		check_tok('(')      ||
		check_tok('-')      ||
		check_tok('+')      )
	{
		--count_tk;
		node->rhs = expr();
	}

	expect_tok(';');

	return node;
}

static Node *statement()
{
	Token *token = tokens->data[count_tk];
	Node  *node  = NULL;

	switch (token->type)
	{
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
		case TK_PRINT:
			node = parse_print();
			break;
		case TK_INPUT:
			node = parse_input();
			break;
		case TK_INT:
			node = parse_vars_stmt();
			break;
		case TK_IDENT:
		case TK_NUM:
		case '(':
		case '-':
		case '+':
			node = parse_expr_stmt();
			break;
		case TK_RETURN:
			node = parse_ret_stmt();
			break;
		case ';':
			++count_tk;
			break;
		default:
			error(token->line, token->column, "syntax error");
			break;
	}

	return node;
}

static Node *statements()
{
	Token *token = expect_tok('{');
	Node  *node  = new_node(K_STATEMENTS);

	symbol_table      = new_table(symbol_table);
	node->u.node_list = new_vector();

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
	Node *node = new_node(K_INIT_PARAMS);
	node->u.node_list = new_vector();

	do
	{
		Token  *token  = expect_tok(TK_INT);
		Symbol *symbol = new_symbol(S_VAR);
		Node   *var    = new_node(K_VAR);

		expect_tok(TK_IDENT);

		symbol->name    = token->str;
		var->symbol     = symbol;

		vec_push(symbol_table->symbols, symbol);
		vec_push(node->u.node_list, var);

		*num_params += 1;
	}
	while (check_tok(','));

	return node;
}

static Node *parse_func()
{
	Node   *node   = new_node(K_FUNC);
	Token  *token  = expect_tok(TK_INT);
	Symbol *symbol = find(symbol_table, token->str);

	if (symbol != NULL)
		error(token->line, token->column, "redefinition of '%s'", symbol->name);

	symbol       = new_symbol(S_FUNC);
	symbol->name = token->str;
	node->symbol = symbol;

	vec_push(symbol_table->symbols, symbol);
	symbol_table    = new_table(symbol_table);

	expect_tok(TK_IDENT);
	token = expect_tok('(');

	if (token->type == TK_INT)
		node->u.lhs = init_params(&symbol->value);
	else
		node->u.lhs = new_node(K_NONE);

	expect_tok(')');

	size_local_vars = 0;
	node->rhs       = statements();

	if (size_local_vars % 16 != 0)
		size_local_vars = (size_local_vars / 16 + 1) * 16;
	node->value = size_local_vars;

	symbol_table = symbol_table->prev;

	return node;
}

static Node *parse_prog()
{
	Node  *node  = new_node(K_PROGRAM);
	Token *token = tokens->data[count_tk];

	node->u.node_list = new_vector();

	while (token->type != TK_EOF)
	{
		vec_push(node->u.node_list, parse_func());
		token = tokens->data[count_tk];
	}

	return node;
}

Node *parsing(Vector *_tokens)
{
	tokens       = _tokens;
	count_tk     = 0;
	string_list  = new_vector();
	symbol_table = new_table(NULL);

	Node *node = parse_prog();

	vec_free(tokens);
	return node;
}