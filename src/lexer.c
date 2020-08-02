#include "../include/lexer.h"

#define TABLE_KEYWORDS_SIZE 33

typedef struct keyword
{
	char *data;
	int type;
	int length;
}
Keyword;

static int line   = 1;
static int column = 1;

static const Keyword table_keywords[TABLE_KEYWORDS_SIZE] =
{
	{"if",  TK_IF,    2}, {"else", TK_ELSE,   4}, {"do",    TK_DO,     2}, {"while", TK_WHILE, 5},
	{"for", TK_FOR,   3}, {"int",  TK_INT,    3}, {"input", TK_INPUT,  5}, {"print", TK_PRINT, 5},
	{"+=",  TK_PLUSA, 2}, {"-=",   TK_MINUSA, 2}, {"*=",    TK_MULTA,  2}, {"/=",    TK_DIVA,  2},
	{"%=",  TK_MODA,  2}, {"<=",   TK_LESSEQ, 2}, {">=",    TK_MOREEQ, 2}, {"==",    TK_EQUAL, 2},
	{"!=",  TK_NOTEQ, 2}, {"||",   TK_OR,     2}, {"&&",    TK_AND,    2}, {"+",     '+',      1},
	{"-",   '-',      1}, {"*",    '*',       1}, {"/",     '/',       1}, {"%",     '%',      1},
	{"=",   '=',      1}, {"<",    '<',       1}, {">",     '>',       1}, {"(",     '(',      1},
	{")",   ')',      1}, {"{",    '{',       1}, {"}",     '}',       1}, {";",     ';',      1},
	{",",   ',',      1}
};

static char *read_file(FILE *file)
{
	char  *str;
	char   buffer[4096];
	size_t count_read, length;

	str    = NULL;
	length = 1;

	do
	{
		count_read = fread(buffer, sizeof(char), 4096, file);
		length    += count_read;
		str        = realloc(str, length);

		if (str == NULL)
			error(0, 0, "memory allocation error in read_file()");

		memcpy(str + length - count_read - 1, buffer, count_read);
	}
	while (count_read != 0);

	return str;
}

static int search_keyword(Token *token, char *p_str)
{
	for (int i = 0; i < TABLE_KEYWORDS_SIZE; i++)
	{
		if (!strncmp(table_keywords[i].data, p_str, table_keywords[i].length))
		{
			token->type   = table_keywords[i].type;
			token->length = table_keywords[i].length;

			return 0;
		}
	}

	return -1;
}

static char *read_ident(Token *token, char *p_str)
{
	char  *buf_p  = p_str;
	size_t length = 1;                 // 1 need for '\0' symbol in the end of string

	while (isalnum(*p_str) || *p_str == '_')
	{
		++p_str;
		++length;
		++column;
	}

	token->str = malloc(sizeof(char) * length);

	if (token->str == NULL)
		error(0, 0, "memory allocation error in read_ident()");

	memcpy(token->str, buf_p, length - 1);

	token->str[length - 1] = '\0';
	token->type            = TK_IDENT;
	token->length          = length;

	return p_str;
}

static char *read_num(Token *token, char *p_str)
{
	token->value = 0;

	while (isdigit(*p_str))
	{
		token->value = token->value * 10 + *p_str - '0';
		++p_str;
		++column;
	}

	token->type = TK_NUM;

	return p_str;
}

static char *read_str(Token *token, char *p_str)
{
	char  *buf_p  = ++p_str;
	size_t length = 1;               // 1 need for '\0' symbol in the end of string

	++column;

	while (*p_str != '\"' && *p_str != '\0' && *p_str != '\n')
	{
		if (*p_str == '\\' && *(p_str + 1) == '\"')
		{
			++p_str;
			++length;
			++column;
		}
		++p_str;
		++length;
		++column;
	}

	if (*p_str != '\"')
		error(line, --column, "missing terminating \" character");

	++column;
	token->str = malloc(sizeof(char) * length);

	if (token->str == NULL)
		error(0, 0, "memory allocation error in read_str()");

	memcpy(token->str, buf_p, length - 1);

	token->str[length - 1] = '\0';
	token->type            = TK_STR;

	return ++p_str;
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
			error(line, --column, "expected '*/' characters");

		++p_str;
		++column;
	}

	return p_str;
}

static Vector *scan(char *p_str)
{
	Vector *tokens = new_vector();
	Token  *token;

	while (*p_str != '\0')
	{
		token = malloc(sizeof(Token));

		if (token == NULL)
			error(0, 0, "memory allocation error in scan()");

		// Whitespace
		if (isspace(*p_str))
		{
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
		}

		// Comment
		else if (!strncmp(p_str, "/*", 2) || !strncmp(p_str, "//", 2))
			p_str = read_comment(p_str);

		// Keywords
		else if (search_keyword(token, p_str) == 0)
		{
			token->line   = line;
			token->column = column;
			p_str        += token->length;
			column       += token->length;

			vec_push(tokens, token);
		}

		// Identifier
		else if (isalpha(*p_str) || *p_str == '_')
		{
			token->line   = line;
			token->column = column;
			p_str         = read_ident(token, p_str);

			vec_push(tokens, token);
		}

		// Number literal
		else if (isdigit(*p_str))
		{
			token->line   = line;
			token->column = column;
			p_str         = read_num(token, p_str);

			vec_push(tokens, token);
		}

		// String literal
		else if (*p_str == '\"')
		{
			token->line   = line;
			token->column = column;
			p_str         = read_str(token, p_str);

			vec_push(tokens, token);
		}

		// Unknown character
		else
			error(line, column, "unknown character");
	}

	token         = malloc(sizeof(Token));
	token->type   = TK_EOF;
	token->line   = line;
	token->column = column;

	vec_push(tokens, token);

	return tokens;
}

Vector *lexer(char *file_name)
{
	FILE *file = fopen(file_name, "r");

	if (file == NULL)
		error(0, 0, "file can't open");

	char   *str    = read_file(file);
	Vector *tokens = scan(str);

	fclose(file);
	free(str);

	return tokens;
}