#include <ctype.h>
#include <malloc.h>
#include <string.h>

#include "vector.h"
#include "error.h"
#include "lex.h"

struct keyword
{
	char *data;
	int type;
};

typedef struct keyword Keyword;

int line;
int column;
Vector *table_keywords;
Keyword symbols[] = {
	{"+=", TK_PLUSA},  {"-=", TK_MINUSA}, {"*=", TK_MULTA},
	{"/=", TK_DIVA},   {"%=", TK_MODA},   {"<=", TK_LESSEQ},
	{">=", TK_MOREEQ}, {"==", TK_EQUAL},  {"!=", TK_NOTEQ},
	{"||", TK_OR},     {"&&", TK_AND},    {NULL, 0}
};

static void push_table(char *data, int type)
{
	Keyword *name = malloc(sizeof(Keyword));
	name->data = data;
	name->type = type;
	vec_push(table_keywords, name);
}

static void enter_TK()
{
	table_keywords = new_vec();
	push_table("if",       TK_IF);
	push_table("else",     TK_ELSE);
	push_table("do",       TK_DO);
	push_table("while",    TK_WHILE);
	push_table("int",      TK_INT);
	push_table("const",    TK_CONST);
	push_table("input",    TK_INPUT);
	push_table("print",    TK_PRINT);
	push_table("break",    TK_BREAK);
	push_table("continue", TK_CONTINUE);
}

static int search_TK(char *name)
{
	Keyword *in_name;
	for (int i = 0; i < table_keywords->len; i++)
	{
		in_name = table_keywords->data[i];

		if (strcmp(name, in_name->data) == 0)
			return in_name->type;
	}

	return TK_IDENT;
}

static FILE *open_file(char *file_name)
{
	FILE *file;

	if ((file = fopen(file_name, "r")) == NULL)
		error("file can't open", 0, 0);

	return file;
}

static char *read_file(FILE *file)
{
	char *str = NULL;
	char buffer[4096];
	unsigned int count_read, len = 1;

	do
	{
		count_read = fread(buffer, sizeof(char), 4096, file);
		len += count_read;
		str = realloc(str, len);
		memcpy(str, buffer, count_read);
	} while (count_read != 0);

	return str;
}

static char *read_ident(Token *t, char *p_str)
{
	char *buf_p = p_str;
	int len = 1;

	while (isalnum(*p_str) || *p_str == '_')
	{
		++p_str;
		++len;
		++column;
	}

	t->name = malloc(sizeof(char) * len);
	memcpy(t->name, buf_p, len - 1);
	t->name[len] = '\0';
	t->type = search_TK(t->name);
	t->len = len;

	return p_str;
}

static char *read_num(Token *t, char *p_str)
{
	t->value = 0;

	while (isdigit(*p_str))
	{
		t->value = t->value * 10 + *p_str - '0';
		++p_str;
		++column;
	}

	t->type = TK_NUM;

	return p_str;
}

static char *remove_backslash(char *prev_str, int len)
{
	char *buf_str = malloc(sizeof(char) * len);
	int count = 0;

	while (*prev_str != '\0')
	{
		if (*prev_str == '\\')
		{
			switch (*++prev_str)
			{
			case 'n':
				buf_str[count++] = '\n';
				++prev_str;
				break;
			case 't':
				buf_str[count++] = '\t';
				++prev_str;
				break;
			case 'r':
				buf_str[count++] = '\r';
				++prev_str;
				break;
			default:
				buf_str[count++] = '\\';
				break;
			}
		}
		else
			buf_str[count++] = *prev_str++;
	}
	buf_str[count] = '\0';

	char *str = malloc(sizeof(char) * count);
	strcpy(str, buf_str);
	free(buf_str);

	return str;
}

static char *read_str(Token *t, char *p_str)
{
	char *buf_p = ++p_str;
	int len = 1;
	++column;

	while (*p_str != '\"' && *p_str != '\0')
	{
		if (*p_str == '\n')
		{
			++line;
			column = 0;
		}
		++p_str;
		++len;
		++column;
	}

	if (*p_str == '\0')
		error("missing terminating \" character", line, --column);

	++column;

	t->name = malloc(sizeof(char) * len);
	memcpy(t->name, buf_p, len - 1);
	t->name[len] = '\0';
	t->type = TK_STR;
	t->name = remove_backslash(t->name, len);

	return ++p_str;
}

static int read_symbol(Token *t, char *p_str)
{
	for (int i = 0; symbols[i].data; i++)
	{
		if (!strncmp(symbols[i].data, p_str, 2))
		{
			t->type = symbols[i].type;
			return 0;
		}
	}
	return -1;
}

static char *read_comment(char *p_str)
{
	if (!strncmp(p_str, "//", 2))
	{
		while (*p_str != '\0')
		{
			if (*++p_str == '\n')
				break;
		}
		++p_str;
		++line;
		column = 1;
	}
	else if (!strncmp(p_str, "/*", 2))
	{
		while (*p_str != '\0')
		{
			if (*p_str == '\n')
			{
				++line;
				column = 1;
			}
			if (*++p_str == '*' && *(p_str + 1) == '/')
			{
				column += 2;
				++p_str;
				break;
			}
			++column;
		}

		if (*p_str == '\0')
			error("expected */ characters", line, --column);

		++p_str;
		++column;
	}
	return p_str;
}

static Vector *scan(char *p_str)
{
	Vector *tokens = new_vec();

	while (*p_str != '\0')
	{
		Token *t = malloc(sizeof(Token));

		// Whitespace
		while (isspace(*p_str))
		{
			if (*p_str == '\n')
			{
				column = 0;
				++line;
			}
			++p_str;
			++column;
		}

		// Multi-letter symbol
		if (read_symbol(t, p_str) == 0)
		{
			t->line = line;
			t->column = column;
			vec_push(tokens, t);
			p_str += 2;
			column += 2;
			continue;
		}

		// Comment
		else if (!strncmp(p_str, "/*", 2) || !strncmp(p_str, "//", 2))
			p_str = read_comment(p_str);

		//Identifier
		else if (isalpha(*p_str) || *p_str == '_')
		{
			t->line = line;
			t->column = column;
			p_str = read_ident(t, p_str);
			vec_push(tokens, t);
		}

		// Number literal
		else if (isdigit(*p_str))
		{
			t->line = line;
			t->column = column;
			p_str = read_num(t, p_str);
			vec_push(tokens, t);
		}

		// Single-letter symbol
		else if (strchr("():;{},*/+-%=<>", *p_str))
		{
			if (*p_str == '\0')
				continue;

			t->type = *p_str++;
			t->line = line;
			t->column = column++;
			vec_push(tokens, t);
		}

		// String literal
		else if (*p_str == '\"')
		{
			t->line = line;
			t->column = column;
			p_str = read_str(t, p_str);
			vec_push(tokens, t);
		}
		
		// Unknown character
		else
			error("unknown character", line, column);
	}
	return tokens;
}

Vector *tokenize(char *file_name)
{
	enter_TK();

	line = 1;
	column = 1;

	FILE *file = open_file(file_name);
	char *str = read_file(file);
	Vector *tokens = scan(str);

	fclose(file);
	free(str);
	return tokens;
}