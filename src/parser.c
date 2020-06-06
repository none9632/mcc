#include "../include/parser.h"

static Vector  *commands;
static Vector  *tokens;
static Table_N *table_names;
static int      count_tk;

static void expr();
static void statements();

static Command *new_command(int command, int value)
{
	Command *c = malloc(sizeof(Command));
	c->command = command;
	c->value = value;
	return c;
}

static Token *check_tok(int type)
{
	Token *t = tokens->data[count_tk];

	if (count_tk >= tokens->length || t->type != type)
	{
		// Take last token
		if (count_tk >= tokens->length)
			t = tokens->data[--tokens->length];

		char message[t->length + 50];
		snprintf(message, sizeof(message), "%s%c%s",
				 "expected '", type, "' character");
		error(message, t->line, t->column);
	}

	t = tokens->data[++count_tk];
	return t;
}

static void factor()
{
	Token *t = tokens->data[count_tk];

	// Take last token
	if (count_tk >= tokens->length)
	{
		t = tokens->data[--tokens->length];
		error("expected declaration or statement at end of input",
			  t->line, t->column);
	}

	if (t->type == TK_NUM)
	{
		vec_push(commands, new_command(CM_GET, t->value));
		++count_tk;
	}
	else if (t->type == TK_IDENT)
	{
		Name *n = find(table_names, t->str);

		if (n == NULL)
		{
			char message[t->length + 50];
			snprintf(message, sizeof(message), "%c%s%s",
					 '\'', t->str, "' undeclared");
			error(message, t->line, t->column);
		}

		Command *c = new_command(CM_GET_TABLE, 0);
		c->table_TN = n;
		vec_push(commands, c);
		++count_tk;
	}
	else if (t->type == '(')
	{
		++count_tk;
		expr();
		check_tok(')');
	}
	else
		error("expected expression", t->line, t->column);
}

static void unary()
{
	Token *t = tokens->data[count_tk];

	if (count_tk < tokens->length && (t->type == '-' || t->type == '+'))
	{
		++count_tk;

		unary();

		if (t->type == '-')
			vec_push(commands, new_command(CM_NEG, 0));
	}

	factor();
}

static void term()
{
	unary();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->length && (t->type == '*' || t->type == '/' || t->type == '%'))
	{
		++count_tk;

		unary();

		Command *c = malloc(sizeof(Command));
		switch (t->type)
		{
			case '*': c->command = CM_MULT; break;
			case '/': c->command = CM_DIV;  break;
			case '%': c->command = CM_MOD;  break;
		}
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void add_and_sub()
{
	term();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->length && (t->type == '+' || t->type == '-'))
	{
		++count_tk;

		term();

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

// comparison operators
static void comp_op()
{
	add_and_sub();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->length &&
			(t->type == '>' || t->type == '<' || t->type == TK_MOREEQ ||
			 t->type == TK_LESSEQ))
	{
		++count_tk;

		add_and_sub();

		Command *c = malloc(sizeof(Command));
		switch (t->type)
		{
			case '>':       c->command = CM_MORE;   break;
			case '<':       c->command = CM_LESS;   break;
			case TK_MOREEQ: c->command = CM_MOREEQ; break;
			case TK_LESSEQ: c->command = CM_LESSEQ; break;
		}
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void equality_op()
{
	comp_op();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->length && (t->type == TK_EQUAL || t->type == TK_NOTEQ))
	{
		++count_tk;

		comp_op();

		Command *c = malloc(sizeof(Command));
		switch (t->type)
		{
			case TK_EQUAL:  c->command = CM_EQUAL;  break;
			case TK_NOTEQ:  c->command = CM_NOTEQ;  break;
		}
		vec_push(commands, c);

		t = tokens->data[count_tk];
	}
}

static void and()
{
	equality_op();
	Token *t = tokens->data[count_tk];

	while (count_tk < tokens->length && t->type == TK_AND)
	{
		++count_tk;

		equality_op();

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

	while (count_tk < tokens->length && t->type == TK_OR)
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
static void assign()
{
	Token *t = tokens->data[count_tk];
	Command *c = malloc(sizeof(Command));

	if (t->type == TK_IDENT)
	{
		Name *n = find(table_names, t->str);

		t = tokens->data[++count_tk];

		switch (t->type)
		{
			case TK_PLUSA:  c->command = CM_PLUSA;  break;
			case TK_MINUSA: c->command = CM_MINUSA; break;
			case TK_MULTA:  c->command = CM_MULTA;  break;
			case TK_DIVA:   c->command = CM_DIVA;   break;
			case TK_MODA:   c->command = CM_MODA;   break;
			case '=':       c->command = CM_ASSIGN; break;
		}

		if (n == NULL)
		{
			char message[t->length + 50];
			snprintf(message, sizeof(message), "%c%s%s",
					 '\'', t->str, "' undeclared");
			error(message, t->line, t->column);
		}

		++count_tk;
		expr();

		vec_push(commands, c);
		c = new_command(CM_STORE, 0);
		c->table_TN = n;
		vec_push(commands, c);
	}
	else
		expr();

	check_tok(';');
}

// initialization variable
static void init_var()
{
	Token *t;

	do
	{
		Name *n = new_name();
		n->type = TK_INT;

		t = tokens->data[++count_tk];

		if (t->type != TK_IDENT)
			error("expected identifier", t->line, t->column);

		if (find(table_names, t->str) != NULL)
			error("redefinition", t->line, t->column);

		n->name = t->str;
		t = tokens->data[++count_tk];

		if (t->type == '=')
		{
			++count_tk;
			expr();

			vec_push(commands, new_command(CM_ASSIGN, 0));
			Command *c = new_command(CM_STORE, 0);
			c->table_TN = n;
			vec_push(commands, c);
		}
		else if (t->type != ';' && t->type != ',')
			error("expected ‘=’, ‘,’, ‘;’", t->line, t->column);

		vec_push(table_names->names, n);

		t = tokens->data[count_tk];
	}
	while (count_tk < tokens->length && t->type == ',');

	check_tok(';');
}

// initialization while
static void init_while()
{
	++count_tk;

	check_tok('(');
	expr();
	check_tok(')');

	Command *c = new_command(CM_WHILE, 0);

	vec_push(commands, c);
	statements();
	vec_push(commands, new_command(CM_STOP_WHILE, 0));

	c->value = commands->length - 1;
}

// initialization do while
static void init_do_while()
{
	++count_tk;

	Command *c = new_command(CM_DO, 0);

	vec_push(commands, c);
	statements();
	vec_push(commands, new_command(CM_STOP_WHILE, 0));
	
	Token *t = tokens->data[count_tk++];
	if (t->type != TK_WHILE)
		error("expected 'while' token", t->line, t->column);

	check_tok('(');
	expr();
	check_tok(')');

	vec_push(commands, new_command(CM_WHILE, 0));

	c->value = commands->length - 1;

	check_tok(';');
}

// initialization if
static void init_if()
{
	Token *t;

	++count_tk;

	check_tok('(');
	expr();
	check_tok(')');

	vec_push(commands, new_command(CM_IF, 0));
	statements();
	vec_push(commands, new_command(CM_STOP_IF, 0));

	t = tokens->data[count_tk];
	if (t->type == TK_ELSE)
	{
		vec_push(commands, new_command(CM_ELSE, 0));
		statements();
		vec_push(commands, new_command(CM_STOP_IF, 0));
	}

	vec_push(commands, new_command(CM_END_IF, 0));
}

static void init_print()
{
	Token *t;

	++count_tk;
	check_tok('(');
	--count_tk;

	do
	{
		t = tokens->data[++count_tk];

		if (t->type == TK_IDENT || t->type == TK_NUM || t->type == '(')
		{
			expr();
			vec_push(commands, new_command(CM_PRINT, 0));
		}
		else if (t->type == TK_STR)
		{
			Command *c = new_command(CM_PRINTS, 0);
			c->data = t->str;
			vec_push(commands, c);
			++count_tk;
		}
		else
			error("expected \"string\" or identifier", t->line, t->column);

		t = tokens->data[count_tk];
	} while (count_tk < tokens->length && t->type == ',');

	check_tok(')');
	check_tok(';');
}

static void init_input()
{
	++count_tk;
	Token *t = check_tok('(');

	if (t->type != TK_IDENT)
		error("expected identifier", t->line, t->column);
	++count_tk;

	Name *n = find(table_names, t->str);

	if (n == NULL)
	{
		char message[t->length + 50];
		snprintf(message, sizeof(message), "%c%s%s",
				 '\'', t->str, "' undeclared");
		error(message, t->line, t->column);
	}

	Command *c = new_command(CM_INPUT, 0);
	c->table_TN = n;
	vec_push(commands, c);

	check_tok(')');
	check_tok(';');
}

// is_loop need for check break or continue within loop or not
static void statement()
{
	Token *t = tokens->data[count_tk];

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
			assign();
			break;
		case ';':
			++count_tk;
			break;
		default:
			error("syntax error", 0, 0);
			break;
	}
}

static void statements()
{
	Table_N *prev_tn = table_names;
	table_names = new_table_n(table_names);
	Token *t = check_tok('{');

	while (count_tk < tokens->length && t->type != '}')
	{
		statement();
		t = tokens->data[count_tk];
	}

	check_tok('}');

	free(table_names);
	table_names = prev_tn;
}

Vector *parsing(Vector *_tokens)
{
	tokens = _tokens;
	commands = new_vec();
	count_tk = 0;

	statements();
	vec_push(commands, new_command(CM_STOP, 0));

	return commands;
}