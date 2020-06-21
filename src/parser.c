#include "../include/parser.h"

static Vector *tokens;
static Table  *symbol_table;
static int     count_tk;

static Node *expr();
static Node *statements();

static Token *expect_tok(int type)
{
	Token *token = tokens->data[count_tk];

	if (count_tk >= tokens->length || token->type != type)
	{
		// If the counter is larger than the number of tokens
		// then we take the last token.
		if (count_tk >= tokens->length)
			token = tokens->data[--tokens->length];

		switch (type)
		{
			case TK_IDENT:
				error(token->line, token->column, "expected identifier");
				break;
			case TK_WHILE:
				error(token->line, token->column, "expected 'while'");
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

	if (count_tk < tokens->length && token->type == type)
	{
		++count_tk;
		return 1;
	}

	return 0;
}

static Symbol *find_symbol(Token *token)
{
	Symbol *symbol = find(symbol_table, token->str);

	if (symbol == NULL)
		error(token->line, token->column, "'%s' is undeclared", token->str);

	return symbol;
}

static Node *factor()
{
	Token *token = tokens->data[count_tk];
	Node  *node;

	if (check_tok(TK_NUM))
	{
		node        = new_node(K_NUM);
		node->value = token->value;
	}
	else if (check_tok(TK_IDENT))
	{
		Symbol *symbol = find_symbol(token);

		node         = new_node(K_VAR);
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
		node      = new_node(K_NONE);
		node->rhs = unary();

		switch (token->type)
		{
			case '-': node->kind = K_NEG;      break;
			case '+': node->kind = K_POSITIVE; break;
		}
	}

	if (node == NULL)
		node = factor();

	return node;
}

static Node *term()
{
	Node  *node  = unary();
	Token *token = tokens->data[count_tk];

	while (check_tok('*') ||
		   check_tok('/') ||
		   check_tok('%') )
	{
		Node *buf_node = new_node(K_NONE);

		buf_node->lhs = node;
		buf_node->rhs = unary();
		node          = buf_node;

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

static Node *add_and_sub()
{
	Node  *node  = term();
	Token *token = tokens->data[count_tk];

	while (check_tok('+') ||
		   check_tok('-') )
	{
		Node *buf_node = new_node(K_NONE);

		buf_node->lhs = node;
		buf_node->rhs = term();
		node          = buf_node;

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
	Node  *node  = add_and_sub();
	Token *token = tokens->data[count_tk];

	while (check_tok('>')       ||
		   check_tok('<')       ||
		   check_tok(TK_MOREEQ) ||
		   check_tok(TK_LESSEQ) )
	{
		Node *buf_node = new_node(K_NONE);

		buf_node->lhs = node;
		buf_node->rhs = add_and_sub();
		node          = buf_node;

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

		buf_node->lhs = node;
		buf_node->rhs = comp_op();
		node          = buf_node;

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

		buf_node->lhs = node;
		buf_node->rhs = equality_op();
		node          = buf_node;
	}

	return node;
}

static Node *or()
{
	Node *node = and();

	while (check_tok(TK_OR))
	{
		Node *buf_node = new_node(K_OR);

		buf_node->lhs = node;
		buf_node->rhs = and();
		node          = buf_node;
	}

	return node;
}

static Node *expr()
{
	Node *node = or();

	return node;
}

static Node *primary_expr()
{
	Node *node = new_node(K_EXPR);

	node->rhs = expr();

	return node;
}

// assignment function
static Node *assign_expression()
{
	Token *token = tokens->data[count_tk++];
	Node  *node;

	if (check_tok(TK_PLUSA)  ||
		check_tok(TK_MINUSA) ||
		check_tok(TK_MULTA)  ||
		check_tok(TK_DIVA)   ||
		check_tok(TK_MODA)   ||
		check_tok('=')       )
	{
		Symbol *symbol = find_symbol(token);

		token = tokens->data[count_tk - 1];

		switch (token->type)
		{
			case TK_PLUSA:  node = new_node(K_PLUSA);  break;
			case TK_MINUSA: node = new_node(K_MINUSA); break;
			case TK_MULTA:  node = new_node(K_MULTA);  break;
			case TK_DIVA:   node = new_node(K_DIVA);   break;
			case TK_MODA:   node = new_node(K_MODA);   break;
			case '=':       node = new_node(K_ASSIGN); break;
		}

		node->lhs         = new_node(K_VAR);
		node->lhs->symbol = symbol;
		node->rhs         = expr();
	}
	else
	{
		--count_tk;
		node = primary_expr();
	}

	expect_tok(';');

	return node;
}

// initialization variable
static Node *init_var()
{
	++count_tk;

	Token *token;
	Node  *node  = new_node(K_INIT_VARS);
	Node  *var;

	node->node_list = new_vector();

	do
	{
		Symbol *symbol = new_symbol(TK_INT);

		token = tokens->data[count_tk];

		expect_tok(TK_IDENT);

		if (find(symbol_table, token->str) != NULL)
			error(token->line, token->column, "redefinition of '%s'", token->str);

		symbol->name = token->str;

		vec_push(symbol_table->symbols, symbol);

		var         = new_node(K_VAR);
		var->symbol = symbol;

		if (check_tok('='))
		{
			Node *assign = new_node(K_ASSIGN);

			assign->lhs = var;
			assign->rhs = expr();
			var         = assign;
		}

		vec_push(node->node_list, var);
	}
	while (check_tok(','));

	expect_tok(';');

	return node;
}

// initialization while
static Node *init_while()
{
	++count_tk;

	Node *node = new_node(K_WHILE);

	expect_tok('(');

	node->lhs = primary_expr();

	expect_tok(')');

	node->rhs = statements();

	return node;
}

// initialization do while
static Node *init_do_while()
{
	++count_tk;

	Node *node = new_node(K_DO_WHILE);
	node->lhs  = statements();

	expect_tok(TK_WHILE);
	expect_tok('(');

	node->rhs = primary_expr();

	expect_tok(')');
	expect_tok(';');

	return node;
}

// initialization if
static Node *init_if()
{
	++count_tk;

	Node *node = new_node(K_IF);

	expect_tok('(');

	node->lhs = primary_expr();

	expect_tok(')');

	node->rhs = statements();

	if (check_tok(TK_ELSE))
	{
		Node *buffer_node = node;

		node           = new_node(K_IF_ELSE);
		node->lhs      = buffer_node;
		node->rhs      = new_node(K_ELSE);
		node->rhs->rhs = statements();
	}

	return node;
}

static Node *init_print()
{
	++count_tk;

	Node *node = new_node(K_PRINT);

	node->node_list = new_vector();

	expect_tok('(');

	do
	{
		if (check_tok(TK_IDENT) ||
			check_tok(TK_NUM)   ||
			check_tok('(')      ||
			check_tok('-')      ||
			check_tok('+'))
		{
			--count_tk;
			vec_push(node->node_list, primary_expr());
		}
		else if (check_tok(TK_STR))
		{
			Token *token  = tokens->data[count_tk - 1];
			Node  *string = new_node(K_STRING);

			string->str = token->str;

			vec_push(node->node_list, string);
		}
	}
	while (check_tok(','));

	expect_tok(')');
	expect_tok(';');

	return node;
}

static Node *init_input()
{
	++count_tk;

	Token  *token  = expect_tok('(');
	Node   *node   = new_node(K_INPUT);
	Symbol *symbol;

	expect_tok(TK_IDENT);

	symbol            = find_symbol(token);
	node->rhs         = new_node(K_VAR);
	node->rhs->symbol = symbol;

	expect_tok(')');
	expect_tok(';');

	return node;
}

// is_loop need for check break or continue within loop or not
static Node *statement()
{
	Token *token = tokens->data[count_tk];
	Node  *node  = NULL;

	switch (token->type)
	{
		case TK_IF:
			node = init_if();
			break;
		case TK_WHILE:
			node = init_while();
			break;
		case TK_DO:
			node = init_do_while();
			break;
		case TK_PRINT:
			node = init_print();
			break;
		case TK_INPUT:
			node = init_input();
			break;
		case TK_INT:
			node = init_var();
			break;
		case TK_IDENT:
		case TK_NUM:
		case '(':
		case '-':
		case '+':
			node = assign_expression();
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

	symbol_table    = new_table(symbol_table);
	node->node_list = new_vector();

	while (count_tk < tokens->length && token->type != '}')
	{
		Node *returned_node = statement();

		if (returned_node != NULL)
			vec_push(node->node_list, returned_node);

		token = tokens->data[count_tk];
	}

	expect_tok('}');

	symbol_table = symbol_table->prev;

	return node;
}

Node *parsing(Vector *_tokens)
{
	tokens   = _tokens;
	count_tk = 0;

	Node *node = new_node(K_PROGRAM);

	node->rhs = statements();

	return node;
}