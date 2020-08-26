#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

#include "vector.h"
#include "lexer.h"
#include "error.h"

#define KEYWORDS_SIZE 9
#define SYMBOL_KEYWORDS_SIZE 27

typedef struct keyword
{
	char *data;
	u_int8_t type;
	u_int8_t length;
}
Keyword;

static FILE *input_file;
static uint line = 0;
static uint column = 0;
static char *p_str = NULL;
static char *source_line = NULL;

static const Keyword keywords[KEYWORDS_SIZE] =
{
	{"if",    TK_IF,    2}, {"else",   TK_ELSE,   4}, {"do",  TK_DO,  2}, {"while", TK_WHILE, 5},
	{"for",   TK_FOR,   3}, {"return", TK_RETURN, 6}, {"int", TK_INT, 3}, {"input", TK_INPUT, 5},
	{"print", TK_PRINT, 5}
};

static const Keyword symbol_keywords[SYMBOL_KEYWORDS_SIZE] =
{
	{"++", TK_INC,    2}, {"--", TK_DEC,   2}, {"+=", TK_PLUSA, 2}, {"-=", TK_MINUSA, 2},
	{"*=", TK_MULTA,  2}, {"/=", TK_DIVA,  2}, {"%=", TK_MODA,  2}, {"<=", TK_LESSEQ, 2},
	{">=", TK_MOREEQ, 2}, {"==", TK_EQUAL, 2}, {"!=", TK_NOTEQ, 2}, {"||", TK_OR,     2},
	{"&&", TK_AND,    2}, {"+",  '+',      1}, {"-",  '-',      1}, {"*",  '*',       1},
	{"/",  '/',       1}, {"%",  '%',      1}, {"=",  '=',      1}, {"<",  '<',       1},
	{">",  '>',       1}, {"(",  '(',      1}, {")",  ')',      1}, {"{",  '{',       1},
	{"}",  '}',       1}, {";",  ';',      1}, {",",  ',',      1}
};

static Token *new_token(u_int8_t type)
{
	Token *token = malloc(sizeof(Token));

	if (token == NULL)
		func_error();

	token->type = type;
	token->source_line = source_line;
	token->line = line;
	token->column = column;

	return token;
}

static Token *make_error_token()
{
	Token *token = new_token(TK_EOF);

	token->source_line = source_line;
	token->line = line;
	token->column = column;

	return token;
}

static void new_source_line()
{
	int length = 0;
	int capacity = 8;
	char ch;
	source_line = malloc(sizeof(char) * capacity);

	if (source_line == NULL)
		func_error();

	do
	{
		ch = fgetc(input_file);

		if (length + 1 >= capacity)
		{
			capacity *= 2;
			source_line = realloc(source_line, sizeof(char) * capacity);

			if (source_line == NULL)
				func_error();
		}

		source_line[length++] = ch;
	}
	while (ch != '\n' && ch != EOF);

	if (ch == EOF)
	{
		++length;
		if (length >= capacity)
		{
			capacity *= 2;
			source_line = realloc(source_line, sizeof(char) * capacity);

			if (source_line == NULL)
				func_error();
		}
	}

	source_line[length - 1] = '\0';

	p_str = source_line;
	++line;
	column = 1;
}

static void next_char()
{
	++column;
	++p_str;
}

static u_int8_t search_keyword(char *str)
{
	for (u_int8_t i = 0; i < KEYWORDS_SIZE; ++i)
	{
		if (!strcmp(keywords[i].data, str))
			return keywords[i].type;
	}

	return TK_IDENT;
}

static int8_t search_symbol_keyword()
{
	for (u_int8_t i = 0; i < SYMBOL_KEYWORDS_SIZE; ++i)
	{
		if (!strncmp(symbol_keywords[i].data, p_str, symbol_keywords[i].length))
			return i;
	}
	return -1;
}

static void read_ident(Token *token)
{
	char *buf_p = p_str;
	uint length = 0;

	while (isalnum(*p_str) || *p_str == '_')
	{
		++length;
		next_char();
	}

	// + 1 need for '\0' symbol in the end of string
	token->str = malloc(sizeof(char) * (length + 1));

	if (token->str == NULL)
		func_error();

	memcpy(token->str, buf_p, length);
	token->str[length] = '\0';
	token->type = search_keyword(token->str);
}

static void read_num(Token *token)
{
	token->value = 0;

	while (isdigit(*p_str))
	{
		token->value = token->value * 10 + *p_str - '0';
		next_char();
	}
}

static void read_str(Token *token)
{
	char *buf_p = ++p_str;
	int buf_column = column++;
	uint length = 0;

	while (*p_str != '\"' && *p_str != EOF && *p_str != '\0')
	{
		if (*p_str == '\\' && *(p_str + 1) == '\"')
		{
			++length;
			next_char();
		}
		++length;
		next_char();
	}

	if (*p_str != '\"')
	{
		/*
		 * Replaces the end of file character with a space.
		 * It is necessary to correctly output the error message
		 */
		if (*p_str == EOF)
			*p_str = ' ';

		token = make_error_token();
		token->column = buf_column;
		error(token, "missing terminating '\"' character");
	}

	next_char();

	// + 1 need for '\0' symbol in the end of string
	token->str = malloc(sizeof(char) * (length + 1));

	if (token->str == NULL)
		func_error();

	memcpy(token->str, buf_p, length);
	token->str[length] = '\0';
}

static void read_comment()
{
	if (!strncmp(p_str, "//", 2))
	{
		new_source_line();
	}
	else
	{
		while (*p_str != EOF)
		{
			if (*p_str == '\0')
				new_source_line();

			if (*p_str == '*' && *(p_str + 1) == '/')
			{
				next_char();
				break;
			}
			next_char();
		}

		if (*p_str == EOF)
		{
			/*
			 * Replaces the end of file character with a space.
			 * It is necessary to correctly output the error message
			 */
			*p_str = ' ';

			error(make_error_token(), "expected '*/' characters");
		}

		next_char();
	}
}

static Vector *scan()
{
	Vector *tokens = new_vector();
	int8_t buffer;

	while (*p_str != EOF)
	{
		// New line
		if (*p_str == '\0')
			new_source_line();

		// Whitespace
		else if (isspace(*p_str))
		{
			while (isspace(*p_str))
				next_char();
		}

		// Comment
		else if (!strncmp(p_str, "/*", 2) || !strncmp(p_str, "//", 2))
			read_comment();

		// Symbol keywords
		else if ((buffer = search_symbol_keyword()) != -1)
		{
			Token *token = new_token(symbol_keywords[buffer].type);

			p_str += symbol_keywords[buffer].length;
			column += symbol_keywords[buffer].length;

			vec_push(tokens, token);
		}

		// Identifier or keyword
		else if (isalpha(*p_str) || *p_str == '_')
		{
			Token *token = new_token(TK_IDENT);

			read_ident(token);
			vec_push(tokens, token);
		}

		// Number literal
		else if (isdigit(*p_str))
		{
			Token *token = new_token(TK_NUM);

			read_num(token);
			vec_push(tokens, token);
		}

		// String literal
		else if (*p_str == '\"')
		{
			Token *token = new_token(TK_STR);

			read_str(token);
			vec_push(tokens, token);
		}

		// Unknown character
		else
			error(make_error_token(), "unknown character");
	}

	/*
	 * Replaces the end of file character with a space.
	 * It is necessary to correctly output the error message
	 */
	source_line[column - 1] = ' ';

	vec_push(tokens, new_token(TK_EOF));

	return tokens;
}

Vector *lexer(char *file_name)
{
	input_file = fopen(file_name, "r+");

	if (input_file == NULL)
		func_error();

	new_source_line();

	Vector *tokens = scan();

	fclose(input_file);
	return tokens;
}