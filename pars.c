#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "vector.h"
#include "pars.h"
#include "table_names.h"
#include "lex.h"
#include "error.h"
#include "launch.h"

Vector  *commands;
Vector  *tokens;
Table_N *table_names;
int      count_tk;

static Command* new_command(int command, int value)
{
	Command *c = malloc(sizeof(Command));
	c->command = command;
	c->value = value;
	return c;
}

static Token* check_tok(int type)
{
	Token *t = tokens->data[count_tk];

	if (count_tk == tokens->len)
		t = tokens->data[--count_tk];

	if (t->type != type) {
		char message[t->len + 50];
		snprintf(message, sizeof(message), "%s%c%s",
			"expected '", type, "' character");
		error(message, t->line, t->column);
	}

	t = tokens->data[++count_tk];
	return t;
}

static void expr();
static void statement();

static void term()
{
	Token *t = tokens->data[count_tk];
	char bufferTT = 0;

	if (count_tk < tokens->len && (t->type == '-' || t->type == '+')) {
		bufferTT = t->type;
		t = tokens->data[++count_tk];
	}

	if (count_tk == tokens->len) {
		t = tokens->data[--count_tk];
		error("expected declaration or statement at end of input",
			t->line, t->column);
	}

	if (t->type == TK_NUM) {
		vec_push(commands, new_command(CM_GET, t->value));
		++count_tk;
	}
	else if (t->type == TK_IDENT) {
		Name *n = find(table_names, t->name);

		if (n == NULL) {
			char message[t->len + 50];
			snprintf(message, sizeof(message), "%c%s%s",
				'\'', t->name, "' undeclared");
			error(message, t->line, t->column);
		}

		Command *c = new_command(CM_GET_TABLE, 0);
		c->table_TN = n;
		vec_push(commands, c);
		++count_tk;
	}
	else if (t->type == '(') {
		++count_tk;
		expr();
		check_tok(')');
	}
	else if (t->type != ';') {
		error("expected expression before token", t->line, t->column);
	}

	if (bufferTT == '-')
		vec_push(commands, new_command(CM_NEG, 0));
}

static void factor()
{
	term();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->len && (t->type == '*' || t->type == '/'))
	{
		++count_tk;
		term();

		Command *c = malloc(sizeof(Command));
		switch (t->type)
		{
		case '*': c->command = CM_MULT; break;
		case '/': c->command = CM_DIV;  break;
		}
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void add_and_sub()
{
	factor();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->len && (t->type == '+' || t->type == '-'))
	{
		++count_tk;
		factor();

		Command *c = malloc(sizeof(Command));
		switch (t->type)
		{
		case '+': c->command = CM_PLUS;  break;
		case '-': c->command = CM_MINUS; break;
		}
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void module()
{
	add_and_sub();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->len && (t->type == '%'))
	{
		++count_tk;
		add_and_sub();

		vec_push(commands, new_command(CM_MOD, 0));
		t = tokens->data[count_tk];
	}
}

// comparison operators
static void comp_op()
{
	module();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->len &&
			(t->type == '>'       || t->type == '<'      || t->type == TK_MOREEQ ||
			 t->type == TK_LESSEQ || t->type == TK_EQUAL || t->type == TK_NOTEQ))
		{
		++count_tk;
		module();

		Command *c = malloc(sizeof(Command));
		switch (t->type)
		{
		case '>':       c->command = CM_MORE;   break;
		case '<':       c->command = CM_LESS;   break;
		case TK_MOREEQ: c->command = CM_MOREEQ; break;
		case TK_LESSEQ: c->command = CM_LESSEQ; break;
		case TK_EQUAL:  c->command = CM_EQUAL;  break;
		case TK_NOTEQ:  c->command = CM_NOTEQ;  break;
		}
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void and()
{
	comp_op();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->len && t->type == TK_AND)
	{
		++count_tk;
		comp_op();

		Command *c = malloc(sizeof(Command));
		c->command = CM_AND;
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void or()
{
	and();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->len && t->type == TK_OR)
	{
		++count_tk;
		and();

		Command *c = malloc(sizeof(Command));
		c->command = CM_OR;
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void expr()
{
	or();
}

// assignment function
static void assign ()
{
	Token *t = tokens->data[count_tk];
	Name *n = find(table_names, t->name);

	if (n == NULL)
	{
		char message[t->len + 50];
		snprintf(message, sizeof(message), "%c%s%s",
			'\'', t->name, "' undeclared");
		error(message, t->line, t->column);
	}

	t = tokens->data[++count_tk];
	++count_tk;
	expr();

	Command *c = new_command(CM_STORE, 0);
	c->table_TN = n;
	vec_push(commands, c);

	c = malloc(sizeof(Command));
	switch (t->type) {
	case TK_PLUSA:  c->command = CM_PLUSA;  break;
	case TK_MINUSA: c->command = CM_MINUSA; break;
	case TK_MULTA:  c->command = CM_MULTA;  break;
	case TK_DIVA:   c->command = CM_DIVA;   break;
	case TK_MODA:   c->command = CM_MODA;   break;
	case '=':      	c->command = CM_ASSIGN; break;
	default: error("expected '=', '+=', '-=', '*=', '/=', '%='",
		t->line, t->column); break;
	}
	vec_push(commands, c);
}

// initialization variable
static void init_var()
{
	Token *t = tokens->data[count_tk];
	Name *general_n = new_name();

	if (t->type == TK_CONST) {
		general_n->is_const = 1;
		t = tokens->data[++count_tk];
	}

	general_n->type = t->type;

	if (t->type == TK_INT) {
		do {
			Name *n = new_name();
			n->is_const = general_n->is_const;
			n->type = general_n->type;

			t = tokens->data[++count_tk];
			check_tok(TK_IDENT);

			if (find(table_names, t->name) != NULL) 
				error("redefinition", t->line, t->column);

			n->name = t->name;
			t = tokens->data[count_tk];

			if (t->type == '=') {
				++count_tk;
				expr();

				Command *c = new_command(CM_STORE, 0);
				c->table_TN = n;
				vec_push(commands, c);
				vec_push(commands, new_command(CM_ASSIGN, 0));
			}
			else if (t->type != ';' && t->type != ',')
				error("expected ‘=’, ‘,’, ‘;’", t->line, t->column);

			vec_push(table_names->names, n);

			t = tokens->data[count_tk];
		} while (count_tk < tokens->len && t->type == ',');

		if (count_tk == tokens->len) {
			t = tokens->data[--count_tk];
			error("expected ';' character", t->line, t->column);
		}
	}
	else
		error("unknown type", t->line, t->column);
}


// initialization while
static void init_while()
{
	++count_tk;

	check_tok('(');
	expr();
	check_tok(')');

	vec_push(commands, new_command(CM_WHILE, 0));
	statement();
	vec_push(commands, new_command(CM_STOP_WHILE, 0));
}

// initialization if
static void init_if()
{
	++count_tk;

	check_tok('(');
	expr();
	check_tok(')');

	vec_push(commands, new_command(CM_IF, 0));
	statement();
	vec_push(commands, new_command(CM_STOP_IF, 0));

	Token *t = tokens->data[count_tk];

	if (t->type == TK_ELSE)
	{
		vec_push(commands, new_command(CM_ELSE, 0));
		++count_tk;
		statement();
		vec_push(commands, new_command(CM_STOP_ELSE, 0));
	}
}

// output function initialization
static void init_print()
{
	++count_tk;
	Token *t = check_tok('(');
	--count_tk;

	do
	{
		t = tokens->data[++count_tk];

		if (t->type == TK_IDENT || t->type == TK_NUM || t->type == '(')
		{
			expr();

			Command *c = new_command(CM_PRINT, 0);
			vec_push(commands, c);
		}
		else if (t->type == TK_STR)
		{
			Command *c = new_command(CM_PRINTS, 0);
			c->data = t->name;
			vec_push(commands, c);
			++count_tk;
		}
		else
			error("expected \"string\" or identifier", t->line, t->column);

		t = tokens->data[count_tk];
	}
	while (count_tk < tokens->len && t->type == ',');

	if (count_tk == tokens->len)
	{
		t = tokens->data[--count_tk];
		error("expected ) character", t->line, t->column);
	}

	check_tok(')');
}

// initialization of the input function
static void init_input()
{
	++count_tk;
	Token *t = check_tok('(');

	if (t->type != TK_IDENT)
		error("expected identifier", t->line, t->column);
	++count_tk;

	Name *n = find(table_names, t->name);

	if (n == NULL)
	{
		char message[t->len + 50];
		snprintf(message, sizeof(message), "%c%s%s",
			'\'', t->name, "' undeclared");
		error(message, t->line, t->column);
	}

	Command *c = new_command(CM_INPUT, 0);
	c->table_TN = n;
	vec_push(commands, c);

	check_tok(')');
}

static void statement()
{
	Table_N *prev_tn = table_names;
	table_names = new_table_n(table_names);
	Token *t = tokens->data[count_tk];

	t = check_tok('{');

	if (t->type == '}') {
		++count_tk;
		return;
	}

	while (count_tk < tokens->len && t->type != '}')
	{
		if (t->type == TK_IDENT)
			assign();
		else if (t->type == TK_INT || t->type == TK_CONST)
			init_var();
		else if (t->type == TK_WHILE)
		{
			init_while();
			t = tokens->data[count_tk];
			continue;
		}
		else if (t->type == TK_IF)
		{
			init_if();
			t = tokens->data[count_tk];
			continue;
		}
		else if (t->type == TK_PRINT)
			init_print();
		else if (t->type == TK_INPUT)
			init_input();
		else if (t->type == ';');
		else 
			error("syntax error", t->line, t->column);

		t = check_tok(';');
	}

	if (count_tk == tokens->len)
	{
		t = tokens->data[--count_tk];
		error("missing terminating } character", t->line, t->column);
	}

	check_tok('}');

	table_names = prev_tn;
}

Vector* parsing(Vector *_tokens)
{
	tokens = _tokens;
	commands = new_vec();
	count_tk = 0;

	statement();

	// for test
	/*
	for (int i = 0; i < commands->len; i++)
	{
		Command *c1 = commands->data[i];
		printf("%i, %i, %s\n", c1->command, c1->value, c1->data);
	}
	*/

	vec_push(commands, new_command(CM_STOP, 0));

	return commands;
}