#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "pars.h"
#include "lib.h"
#include "table_names.h"
#include "lex.h"
#include "error.h"
#include "launch.h"

Vector *commands;
Vector *tokens;
Vector *table_names;
int     count_tk;

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
	if (t->type != type)
		error("syntax error", 1);
	t = tokens->data[++count_tk];
	return t;
}

static void expr();
static void statement();

static void term() 
{
	Token *t = tokens->data[count_tk];

	if (t->type == '-' || t->type == '+') {
		if (t->type == '-')
			vec_push(commands, new_command(CM_NEG, 0));
		count_tk++;
	}

	if (t->type == TK_NUM) {
		vec_push(commands, new_command(CM_GET, t->value));
		count_tk++;
	}
	else if (t->type == TK_IDENT) {
		Name *v = find(table_names, t->name);
		if (v == NULL)
			error("syntax error", 1);
		vec_push(commands, new_command(CM_GET_TABLE, v->value));
		count_tk++;
	}
	else if (t->type == '(') {
		count_tk++;
		expr();
		check_tok(')');
	}
}

static void factor()
{
	term();
	Token *t = tokens->data[count_tk];

	if (t->type == '*' || t->type == '/') {
		do {
			count_tk++;
			term();

			Command *c = malloc(sizeof(Command));
			switch (t->type) {
			case '*': c->command = CM_MULT; break;
			case '/': c->command = CM_DIV;  break;
			}
			vec_push(commands, c);

			t = tokens->data[count_tk];
		} while (t->type == '*' || t->type == '/');
	}
}

static void addend()
{
	factor();
	Token *t = tokens->data[count_tk];

	if (t->type == '+' || t->type == '-') {
		do {
			count_tk++;
			factor();

			Command *c = malloc(sizeof(Command));
			switch (t->type) {
			case '+': c->command = CM_PLUS;  break;
			case '-': c->command = CM_MINUS; break;
			}
			vec_push(commands, c);

			t = tokens->data[count_tk];
		} while (t->type == '+' || t->type == '-');
	}
}

static void module()
{
	addend();
	Token *t = tokens->data[count_tk];

	if (t->type == '%') {
		do {
			count_tk++;
			addend();
			vec_push(commands, new_command(CM_MOD, 0));
			t = tokens->data[count_tk];
		} while (t->type == '%');
	}
}

static void test() 
{
	module();
	Token *t = tokens->data[count_tk];

	if (t->type == '>' || t->type == '<' || t->type == TK_MOREEQ ||
		t->type == TK_LESSEQ || t->type == TK_EQUAL || t->type == TK_NOTEQ) {
		count_tk++;
		module();

		Command *c = malloc(sizeof(Command));
		switch (t->type) {
		case '>':       c->command = CM_MORE;   break;
		case '<':       c->command = CM_LESS;   break;
		case TK_MOREEQ: c->command = CM_MOREEQ; break;
		case TK_LESSEQ: c->command = CM_LESSEQ; break;
		case TK_EQUAL:  c->command = CM_EQUAL;  break;
		case TK_NOTEQ:  c->command = CM_NOTEQ;  break;
		}
		vec_push(commands, c);
	}
}

static void expr()
{
	test();
}

// assignment function
static void assign () 
{
	Token *t = tokens->data[count_tk];
	Name *n = find(table_names, t->name);

	if (n == NULL)
		error("syntax error", 0);

	t = tokens->data[++count_tk];
	++count_tk;
	expr();

	vec_push(commands, new_command(CM_STORE, n->value));
	Command *c = malloc(sizeof(Command));
	switch (t->type) {
	case TK_PLUSA:  c->command = CM_PLUSA;  break;
	case TK_MINUSA: c->command = CM_MINUSA; break;
	case TK_MULTA:  c->command = CM_MULTA;  break;
	case TK_DIVA:   c->command = CM_DIVA;   break;
	case TK_MODA:   c->command = CM_MODA;   break;
	case '=':      	c->command = CM_ASSIGN; break;
	default: error("syntax error", 0);    break;
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

	if (t->type == TK_BOOL || t->type == TK_INT || t->type == TK_CHAR ) {
		do {
			Name *n = new_name();
			n->is_const = general_n->is_const;
			n->type = general_n->type;

			t = tokens->data[++count_tk];
			check_tok(TK_IDENT);
			if (find(table_names, t->name) != NULL) 
				error("syntax error", 0);
			n->name = t->name;
			t = tokens->data[count_tk];

			if (t->type == '=') {
				count_tk++;
				expr();
				vec_push(commands, new_command(CM_STORE, 0));
				vec_push(commands, new_command(CM_ASSIGN, 0));
			}
			vec_push(table_names, n);
			t = tokens->data[count_tk];
		} while (t->type == ',' && count_tk < tokens->len);

		if (count_tk == tokens->len)
			error("syntax error", 0);
	}
	else
		error("unknown type", 1);
}


// initialization while
static void init_while()
{
	count_tk++;

	check_tok('(');
	expr();
	check_tok(')');

	vec_push(commands, new_command(CM_WHILE, 0));
	statement();
	vec_push(commands, new_command(CM_STOP, 0));
}

// initialization if
static void init_if()
{
	count_tk++;

	check_tok('(');
	expr();
	check_tok(')');

	vec_push(commands, new_command(CM_IF, 0));
	statement();
	vec_push(commands, new_command(CM_STOP, 0));

	Token *t = tokens->data[count_tk];
	if (t->type == TK_ELSE) {
		vec_push(commands, new_command(CM_ELSE, 0));
		count_tk++;
		statement();
		vec_push(commands, new_command(CM_STOP, 0));
	}
}

// output function initialization
static void init_print() 
{
	count_tk++;
	Token *t = check_tok('(');
	count_tk--;

	do {
		t = tokens->data[++count_tk];

		if (t->type == TK_IDENT || t->type == TK_NUM || t->type == '(') {
			int bufferTT = t->type;
			expr();
			Command *c = new_command(CM_PRINT, 0);
			vec_push(commands, c);
		}
		else if (t->type == TK_STR) {
			Command *c = new_command(CM_PRINTS, 0);
			
			c->data = t->name;
			vec_push(commands, c);
			count_tk++;
		}
		else
			error("syntax error", 1);

		t = tokens->data[count_tk];
	} while (t->type == ',' && count_tk < tokens->len);

	if (count_tk == tokens->len)
		error("syntax error", 0);

	check_tok(')');
}

// initialization of the input function
static void init_input()
{
	count_tk++;
	Token *t = check_tok('(');

	if (t->type != TK_IDENT)
		error("syntax error", 1);
	count_tk++;

	Name *n = find(table_names, t->name);
	Command *c = new_command(CM_INPUT, 0);
	c->table_TN = n;
	vec_push(commands, c);

	check_tok(')');
}

static void statement()
{
	Token *t = tokens->data[count_tk];

	t = check_tok('{');

	while (t->type != '}' && count_tk < tokens->len) {
		if (t->type == TK_IDENT)
			assign();
		else if (t->type == TK_INT || t->type == TK_BOOL || t->type == TK_CHAR 
				|| t->type == TK_CONST)
			init_var();
		else if (t->type == TK_WHILE) {
			init_while();
			t = tokens->data[count_tk];
			continue;
		}
		else if (t->type == TK_IF) {
			init_if();
			t = tokens->data[count_tk];
			continue;
		}
		else if (t->type == TK_PRINT)
			init_print();
		else if (t->type == TK_INPUT)
			init_input();

		t = check_tok(';');
	}

	if (count_tk == tokens->len)
		error("syntax error", 0);

	check_tok('}');
}

Vector* parsing(Vector *_tokens)
{
	tokens = _tokens;
	commands = new_vec();
	table_names = new_vec();
	count_tk = 0;

	statement();

	// for (int i = 0; i < commands->len; i++) {
	// 	Command *c1 = commands->data[i];
	// 	printf("%i, %i, %s\n", c1->command, c1->value, c1->data);
	// }

	return commands;
}