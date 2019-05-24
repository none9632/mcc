#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

#include "lib.h"
#include "table.h"
#include "pars.h"
#include "error.h"
#include "lex.h"

Vector *table_keywords;
Symbols symbols[] = {
	{"+=", PLUSA_TOK},  {"-=", MINUSA_TOK}, {"*=", MULTA_TOK},
	{"/=", DIVA_TOK},   {"%=", MODA_TOK},   {"<=", LESSEQ_TOK},
	{">=", MOREEQ_TOK}, {"==", EQUAL_TOK},  {"!=", NOTEQ_TOK},
	{NULL, 0}
};

void push_table(char *data, int type) {
	Keyword *name = malloc(sizeof(Keyword));
	name->data = data;
	name->type = type;
	vec_push(table_keywords, name);
}

void enter_TK() {
	table_keywords = new_vec();
	push_table("if",     IF_TOK);
	push_table("else",   ELSE_TOK);
	push_table("do",     TK_DO);
	push_table("while",  WHILE_TOK);
	push_table("for",    TK_FOR);
	push_table("int",    TK_INT);
	push_table("double", TK_DOUBLE);
	push_table("const",  TK_CONST);
	push_table("bool",   TK_BOOL);
	push_table("char",   TK_CHAR);
	push_table("input",  INPUT_TOK);
	push_table("print",  PRINT_TOK);
}

int search_TK(char *name) {
	Keyword *in_name;
	for (int i = 0; i < table_keywords->len; i++) {
		in_name = table_keywords->data[i];
		if (strcmp(name, in_name->data) == 0) {
			return in_name->type;
		}
	}
	return -1;
}

FILE* openFile(char* file_name) {
	FILE *file;
	if ((file = fopen(file_name, "r")) == NULL)
		error("file can't open", 0);
	return file;
}

char* read_file(FILE *file) {
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

char* read_ident(Token *t, char* p_str) {
	char *buf_p = p_str;
	int len = 1;
	while (isalnum(*p_str)) {
		p_str++;
		len++;
	}
	t->name = malloc(sizeof(char) * len);
	memcpy(t->name, buf_p, len - 1);
	t->name[len] = '\0';
	t->type = search_TK(t->name);
	if (t->type == -1) 
		t->type = IDENT_TOK;
	return p_str;
}

char* read_num(Token *t, char *p_str) {
	t->value = 0;
	while (isdigit(*p_str)) {
		t->value = t->value * 10 + *p_str - '0';
		p_str++;
	}
	t->type = NUM_TOK;
	return p_str;
}

char* read_str(Token *t, char *p_str) {
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

int read_symbol(Token *t, char *p_str) {
	for (int i = 0; symbols[i].data; i++) {
		if (!strncmp(symbols[i].data, p_str, 2)) {
			t->type = symbols[i].type;
			return 0;
		}
	}
	return -1;
}

char *read_comment(char *p_str) {
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

Vector* scan(char *p_str) {
	Vector *tokens = new_vec();
	while (*p_str != '\0') {
		while (isspace(*p_str))
			p_str++;
		if (*p_str == '/')
			p_str = read_comment(p_str);
		Token *t = malloc(sizeof(Token));
		if (isalpha(*p_str)) {
			p_str = read_ident(t, p_str);
			vec_push(tokens, t);
			continue;
		}
		if (isdigit(*p_str)) {
			p_str = read_num(t, p_str);
			vec_push(tokens, t);
			continue;
		}
		if (read_symbol(t, p_str) == 0) {
			vec_push(tokens, t);
			p_str += 2;
			continue;
		}
		if (strchr("():;{},*/+%=<>", *p_str)) {
			t->type = *p_str++;
			vec_push(tokens, t);
			continue;
		}
		if (*p_str == '\"') {
			p_str = read_str(t, p_str);
			vec_push(tokens, t);
			continue;
		}
		error("syntax error", 0);
	}
	return tokens;
}

Vector* tokenize(char *file_name) {
	FILE *file = openFile(file_name);
	char *str = read_file(file);
	enter_TK();
	Vector *tokens = scan(str);
	fclose(file);
	return tokens;
}
