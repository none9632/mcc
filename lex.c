#include <ctype.h>
#include <malloc.h>
#include <string.h>

#include "lib.h"
#include "error.h"
#include "lex.h"

Vector  *table_keywords;
Keyword  symbols[] = {
	{"+=", TK_PLUSA},  {"-=", TK_MINUSA}, {"*=", TK_MULTA},
	{"/=", TK_DIVA},   {"%=", TK_MODA},   {"<=", TK_LESSEQ},
	{">=", TK_MOREEQ}, {"==", TK_EQUAL},  {"!=", TK_NOTEQ},
	{NULL, 0}
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
	push_table("if",     TK_IF);
	push_table("else",   TK_ELSE);
	push_table("do",     TK_DO);
	push_table("while",  TK_WHILE);
	push_table("for",    TK_FOR);
	push_table("int",    TK_INT);
	push_table("const",  TK_CONST);
	push_table("bool",   TK_BOOL);
	push_table("char",   TK_CHAR);
	push_table("input",  TK_INPUT);
	push_table("print",  TK_PRINT);
}

static int search_TK(char *name)
{
	Keyword *in_name;
	for (int i = 0; i < table_keywords->len; i++) {
		in_name = table_keywords->data[i];
		if (strcmp(name, in_name->data) == 0) {
			return in_name->type;
		}
	}

	return TK_IDENT;
}

static FILE* openFile(char* file_name)
{
	FILE *file;

	if ((file = fopen(file_name, "r")) == NULL)
		error("file can't open", 0);

	return file;
}

static char* read_file(FILE *file)
{
	char *str;
	char buffer[4096];
	int cread, len = 1;

	do {
		cread = fread(buffer, sizeof(char), 4096, file);
		len += cread;
		str = realloc(str, len);
		memcpy(str, buffer, cread);
	} while (cread != 0);

	return str;
}

static char* read_ident(Token *t, char* p_str)
{
	char *buf_p = p_str;
	int len = 1;

	while (isalnum(*p_str) || *p_str == '_') {
		p_str++;
		len++;
	}

	t->name = malloc(sizeof(char) * len);
	memcpy(t->name, buf_p, len - 1);
	t->name[len] = '\0';
	t->type = search_TK(t->name);

	return p_str;
}

static char* read_num(Token *t, char *p_str)
{
	t->value = 0;

	while (isdigit(*p_str)) {
		t->value = t->value * 10 + *p_str - '0';
		p_str++;
	}

	t->type = TK_NUM;

	return p_str;
}

static char* read_str(Token *t, char *p_str)
{
	char *buf_p = ++p_str;
	int len = 1;

	while (*p_str != '\"' && *p_str != '\0') {
		p_str++;
		len++;
	}
	if (*p_str == '\0')
		error("sytax error", 0);

	t->name = malloc(sizeof(char) * len);
	memcpy(t->name, buf_p, len - 1);
	t->name[len] = '\0';
	t->type = TK_STR;
	return ++p_str;
}

static int read_symbol(Token *t, char *p_str)
{
	for (int i = 0; symbols[i].data; i++) {
		if (!strncmp(symbols[i].data, p_str, 2)) {
			t->type = symbols[i].type;
			return 0;
		}
	}
	return -1;
}

static char *read_comment(char *p_str)
{
	if (*++p_str == '/') {
		while (*p_str != '\0') {
			if (*++p_str == '\n')
				break;
		}
		p_str++;
	}
	else if (*p_str == '*') {
		while (*p_str != '\0') {
			if (*p_str == '*' && *++p_str == '/')
				break;
		}
		p_str++;
	}
	return p_str;
}

static Vector* scan(char *p_str)
{
	Vector *tokens = new_vec();

	while (*p_str != '\0') {
		Token *t = malloc(sizeof(Token));

		// Whitespace
		while (isspace(*p_str))
			p_str++;

		// Comment
		if (*p_str == '/')
			p_str = read_comment(p_str);

		//Identifier
		if (isalpha(*p_str) || *p_str == '_') {
			p_str = read_ident(t, p_str);
			vec_push(tokens, t);
			continue;
		}

		// Number literal
		if (isdigit(*p_str)) {
			p_str = read_num(t, p_str);
			vec_push(tokens, t);
			continue;
		}

		// Multi-letter symbol
		if (read_symbol(t, p_str) == 0) {
			vec_push(tokens, t);
			p_str += 2;
			continue;
		}

		// Single-letter symbol
		if (strchr("():;{},*/+-%=<>", *p_str)) {
			t->type = *p_str++;
			vec_push(tokens, t);
			continue;
		}

		// String literal
		if (*p_str == '\"') {
			p_str = read_str(t, p_str);
			vec_push(tokens, t);
			continue;
		}

		error("syntax error", 0);
	}
	return tokens;
}

Vector* tokenize(char *file_name)
{
	enter_TK();

	FILE *file = openFile(file_name);
	char *str = read_file(file);
	Vector *tokens = scan(str);

	// Token *t;
	// for (int i = 0; i < tokens->len; i++) {
	// 	t = tokens->data[i];
	// 	printf("%s, %i, %i\n", t->name, t->type, t->value);
	// }

	free(str);
	fclose(file);
	return tokens;
}
