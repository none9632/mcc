#include "../include/parser.h"

#define MAX_STRING_SIZE 25

static Vector *tokens;
static int     count_tk;

static Node *expr();
static Node *statements();

static Token *check_tok(int type)
{
	Token *t = tokens->data[count_tk];

	if (count_tk >= tokens->length || t->type != type)
	{
		// Take last token
		if (count_tk >= tokens->length)
			t = tokens->data[--tokens->length];

		char message[t->length + MAX_STRING_SIZE];
		snprintf(message, sizeof(message), "%s%c%s",
				 "expected '", type, "' character");
		error(message, t->line, t->column);
	}

	t = tokens->data[++count_tk];
	return t;
}

static Node *factor()
{
	Token *t = tokens->data[count_tk];
	Node  *node = new_node();

	if (count_tk >= tokens->length)
	{
		t = tokens->data[--tokens->length];
		error("expected declaration or statement at end of input",
			  t->line, t->column);
	}

	if (t->type == TK_NUM)
	{
		node->kind = K_NUM;
		node->value = t->value;
		++count_tk;
	}
	else if (t->type == TK_IDENT)
	{
	}
	else if (t->type == '(')
	{
		++count_tk;
		node = expr();
		check_tok(')');
	}
	else
		error("expected expression", t->line, t->column);

	return node;
}

static Node *unary()
{
	Token *t    = tokens->data[count_tk];
	Node  *node = NULL;

	if (count_tk < tokens->length && (t->type == '-' || t->type == '+'))
	{
		++count_tk;

		node = new_node();
		node->rhs = unary();

		switch (t->type)
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
	Node  *node = unary();
	Token *t    = tokens->data[count_tk];

	while (count_tk < tokens->length && (t->type == '*' || t->type == '/' || t->type == '%'))
	{
		++count_tk;

		Node *buf_node = new_node();
		buf_node->lhs = node;
		buf_node->rhs = unary();

		node = buf_node;

		switch (t->type)
		{
			case '*': node->kind = K_MULT; break;
			case '/': node->kind = K_DIV;  break;
			case '%': node->kind = K_MOD;  break;
		}

		t = tokens->data[count_tk];
	}

	return node;
}

static Node *add_and_sub()
{
	Node  *node = term();
	Token *t    = tokens->data[count_tk];

	while (count_tk < tokens->length && (t->type == '+' || t->type == '-'))
	{
		++count_tk;

		Node *buf_node = new_node();
		buf_node->lhs = node;
		buf_node->rhs = term();

		node = buf_node;

		switch (t->type)
		{
			case '+': node->kind = K_ADD; break;
			case '-': node->kind = K_SUB; break;
		}

		t = tokens->data[count_tk];
	}

	return node;
}

// comparison operators
static Node *comp_op()
{
	Node  *node = add_and_sub();
	Token *t    = tokens->data[count_tk];

	while (count_tk < tokens->length &&
		   (t->type == '>' || t->type == '<' || t->type == TK_MOREEQ ||
			t->type == TK_LESSEQ))
	{
		++count_tk;


		Node *buf_node = new_node();
		buf_node->lhs = node;
		buf_node->rhs = add_and_sub();

		node = buf_node;

		switch (t->type)
		{
			case '>':       node->kind = K_MORE;   break;
			case '<':       node->kind = K_LESS;   break;
			case TK_MOREEQ: node->kind = K_MOREEQ; break;
			case TK_LESSEQ: node->kind = K_LESSEQ; break;
		}

		t = tokens->data[count_tk];
	}

	return node;
}

static Node *equality_op()
{
	Node  *node = comp_op();
	Token *t    = tokens->data[count_tk];

	while (count_tk < tokens->length && (t->type == TK_EQUAL || t->type == TK_NOTEQ))
	{
		++count_tk;

		Node *buf_node = new_node();
		buf_node->lhs = node;
		buf_node->rhs = comp_op();

		node = buf_node;

		switch (t->type)
		{
			case TK_EQUAL: node->kind = K_EQUAL;     break;
			case TK_NOTEQ: node->kind = K_NOT_EQUAL; break;
		}

		t = tokens->data[count_tk];
	}

	return node;
}

static Node *and()
{
	Node  *node = equality_op();
	Token *t    = tokens->data[count_tk];

	while (count_tk < tokens->length && t->type == TK_AND)
	{
		++count_tk;

		Node *buf_node = new_node();
		buf_node->lhs = node;
		buf_node->rhs = equality_op();

		node = buf_node;

		node->kind = K_AND;

		t = tokens->data[count_tk];
	}

	return node;
}

static Node *or()
{
	Node  *node = and();
	Token *t    = tokens->data[count_tk];

	while (count_tk < tokens->length && t->type == TK_OR)
	{
		++count_tk;

		Node *buf_node = new_node();
		buf_node->lhs = node;
		buf_node->rhs = and();

		node = buf_node;

		node->kind = K_OR;

		t = tokens->data[count_tk];
	}

	return node;
}

static Node *expr()
{
	Node *node = or();

	return node;
}

// assignment function
static Node *assign()
{
	Node *node = new_node();

	node->kind = K_ASSIGN;
	node->rhs = expr();

	check_tok(';');

	return node;
}

// initialization variable
static Node *init_var()
{
	Node *node = new_node();

	return node;
}

// initialization while
static Node *init_while()
{
	Node *node = new_node();

	node->kind = K_WHILE;
	node->lhs = new_node();
	node->lhs->kind = K_PAREN_EXPR;

	++count_tk;

	check_tok('(');
	node->lhs->rhs = expr();
	check_tok(')');

	node->rhs = statements();

	return node;
}

// initialization do while
static Node *init_do_while()
{
	Node *node = new_node();

	++count_tk;

	node->kind = K_DO_WHILE;
	node->lhs = statements();

	check_tok(TK_WHILE);
	check_tok('(');
	node->rhs = new_node();
	node->rhs->kind = K_PAREN_EXPR;
	node->rhs->rhs = expr();
	check_tok(')');

	check_tok(';');

	return node;
}

// initialization if
static Node *init_if()
{
	Token *t;
	Node  *node = new_node();

	node->kind = K_IF;
	node->lhs = new_node();
	node->lhs->kind = K_PAREN_EXPR;

	++count_tk;

	check_tok('(');
	node->lhs->rhs = expr();
	check_tok(')');

	node->rhs = statements();

	t = tokens->data[count_tk];
	if (t->type == TK_ELSE)
	{
		++count_tk;
		Node *buffer_node = node;

		node = new_node();
		node->kind = K_IF_ELSE;
		node->lhs = buffer_node;
		node->rhs = new_node();
		node->rhs->kind = K_ELSE;
		node->rhs->rhs = statements();
	}

	return node;
}

static Node *init_print()
{
	Node  *node = new_node();
	Token *t;

	node->kind = K_PRINT;
	node->node_list = new_vec();

	++count_tk;
	check_tok('(');
	--count_tk;

	do
	{
		t = tokens->data[++count_tk];

		if (t->type == TK_IDENT || t->type == TK_NUM || t->type == '(' ||
			t->type == '-'      || t->type == '+')
		{
			vec_push(node->node_list, expr());
		}
		else if (t->type == TK_STR)
		{
			Node *buffer_node = new_node();

			buffer_node->kind = K_STRING;
			buffer_node->str = t->str;

			vec_push(node->node_list, buffer_node);
			count_tk++;
		}
		else
			error("expected \"string\" or expression", t->line, t->column);

		t = tokens->data[count_tk];
	}
	while (count_tk < tokens->length && t->type == ',');

	check_tok(')');
	check_tok(';');

	return node;
}

static Node *init_input()
{
	Node *node = new_node();

	return node;
}

// is_loop need for check break or continue within loop or not
static Node *statement()
{
	Token *t    = tokens->data[count_tk];
	Node  *node = NULL;

	switch (t->type)
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
			node = assign();
			break;
		case ';':
			++count_tk;
			break;
		default:
			error("syntax error", 0, 0);
			break;
	}

	return node;
}

static Node *statements()
{
	Token *t    = check_tok('{');
	Node  *node = new_node();

	node->kind      = K_STATEMENTS;
	node->node_list = new_vec();

	while (count_tk < tokens->length && t->type != '}')
	{
		Node *returned_node = statement();

		if (returned_node != NULL)
			vec_push(node->node_list, returned_node);

		t = tokens->data[count_tk];
	}

	check_tok('}');

	return node;
}

Node *parsing(Vector *_tokens)
{
	tokens      = _tokens;
	count_tk    = 0;

	Node *node = new_node();
	node->kind = K_PROGRAM;
	node->rhs = statements();

	return node;
}