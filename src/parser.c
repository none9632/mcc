#include "../include/parser.h"

#define MAX_STRING_SIZE 50

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
		node->n1 = unary();

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
		buf_node->n1 = node;
		buf_node->n2 = unary();

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
		buf_node->n1 = node;
		buf_node->n2 = term();

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
		buf_node->n1 = node;
		buf_node->n2 = add_and_sub();

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
		buf_node->n1 = node;
		buf_node->n2 = comp_op();

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
		buf_node->n1 = node;
		buf_node->n2 = equality_op();

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
		buf_node->n1 = node;
		buf_node->n2 = and();

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
	Node  *node;

	node = expr();

	check_tok(';');

	return node;
}

// initialization variable
static void init_var()
{
}

// initialization while
static void init_while()
{
}

// initialization do while
static void init_do_while()
{
}

// initialization if
static void init_if()
{
}

static void init_print()
{

}

static void init_input()
{
}

// is_loop need for check break or continue within loop or not
static Node *statement()
{
	Token *t = tokens->data[count_tk];
	Node  *node;

	switch (t->type)
	{
		case TK_IF:
			init_if();
			break;
		case TK_WHILE:
			init_while();
			break;
		case TK_DO:
			init_do_while();
			break;
		case TK_PRINT:
			init_print();
			break;
		case TK_INPUT:
			init_input();
			break;
		case TK_INT:
			init_var();
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

	node->kind      = K_PROGRAM;
	node->node_list = new_vec();

	while (count_tk < tokens->length && t->type != '}')
	{
		vec_push(node->node_list, statement());
		t = tokens->data[count_tk];
	}

	check_tok('}');

	return node;
}

Node *parsing(Vector *_tokens)
{
	tokens      = _tokens;
	count_tk    = 0;

	Node *node = statements();

	return node;
}