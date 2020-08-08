#include "../include/lexer.h"

#define KEYWORDS_SIZE 10
#define SYMBOL_KEYWORDS_SIZE 25

typedef struct keyword
{
	char *data;
	int type;
	int length;
}
Keyword;

static int   line   = 1;
static int   column = 1;
static char *p_str;

static const Keyword keywords[KEYWORDS_SIZE] =
{
	{"if",    TK_IF,     2}, {"else",   TK_ELSE,   4}, {"do",   TK_DO,    2}, {"while", TK_WHILE,  5},
	{"for",   TK_FOR,    3}, {"return", TK_RETURN, 6}, {"void", TK_VOID,  4}, {"int",   TK_INT,    3},
	{"input", TK_INPUT,  5}, {"print",  TK_PRINT,  5}
};

static const Keyword symbol_keywords[SYMBOL_KEYWORDS_SIZE] =
{
	{"+=", TK_PLUSA, 2}, {"-=", TK_MINUSA, 2}, {"*=", TK_MULTA,  2}, {"/=", TK_DIVA,  2},
	{"%=", TK_MODA,  2}, {"<=", TK_LESSEQ, 2}, {">=", TK_MOREEQ, 2}, {"==", TK_EQUAL, 2},
	{"!=", TK_NOTEQ, 2}, {"||", TK_OR,     2}, {"&&", TK_AND,    2}, {"+",  '+',      1},
	{"-",  '-',      1}, {"*",  '*',       1}, {"/",  '/',       1}, {"%",  '%',      1},
	{"=",  '=',      1}, {"<",  '<',       1}, {">",  '>',       1}, {"(",  '(',      1},
	{")",  ')',      1}, {"{",  '{',       1}, {"}",  '}',       1}, {";",  ';',      1},
	{",",  ',',      1}
};

static Token *new_token(int type)
{
	Token *token = malloc(sizeof(Token));

	if (token == NULL)
		func_error();

	token->type   = type;
	token->line   = line;
	token->column = column;

	return token;
}

static void next_char()
{
	++column;
	++p_str;
}

static char *read_file(FILE *file)
{
	char  *text;
	char   buffer[4096];
	size_t count_read, length;

	text   = NULL;
	length = 1;

	while ((count_read = fread(buffer, sizeof(char), 4096, file)) != 0)
	{
		text = realloc(text, length + count_read);

		if (text == NULL)
			func_error();

		memcpy(text + length - 1, buffer, count_read);

		length += count_read;
	}

	if (!feof(file))
		func_error();

	text[length - 1] = '\0';

	return text;
}

static int search_keyword(char *str)
{
	for (int i = 0; i < KEYWORDS_SIZE; ++i)
	{
		if (strcmp(keywords[i].data, str) == 0)
			return keywords[i].type;
	}

	return TK_IDENT;
}

static int search_symbol_keyword()
{
	for (int i = 0; i < SYMBOL_KEYWORDS_SIZE; ++i)
	{
		if (!strncmp(symbol_keywords[i].data, p_str, symbol_keywords[i].length))
			return i;
	}
	return -1;
}

static void read_ident(Token *token)
{
	char  *buf_p  = p_str;
	size_t length = 1;                 // 1 need for '\0' symbol in the end of string

	while (isalnum(*p_str) || *p_str == '_')
	{
		++length;
		next_char();
	}

	token->str = malloc(sizeof(char) * length);

	if (token->str == NULL)
		func_error();

	memcpy(token->str, buf_p, length - 1);
	token->str[length - 1] = '\0';
	token->type            = search_keyword(token->str);
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
	char  *buf_p      = ++p_str;
	size_t length     = 1;            // 1 need for '\0' symbol in the end of string
	int    buf_column = column++;

	while (*p_str != '\"' && *p_str != '\0' && *p_str != '\n')
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
		error(line, buf_column, "missing terminating '\"' character");

	next_char();

	token->str = malloc(sizeof(char) * length);

	if (token->str == NULL)
		func_error();

	memcpy(token->str, buf_p, length - 1);
	token->str[length - 1] = '\0';
}

static void read_comment()
{
	if (!strncmp(p_str, "//", 2))
	{
		while (*p_str != '\0' && *p_str != '\n')
			++p_str;
		++p_str;
		++line;
		column = 1;
	}
	else
	{
		while (*p_str != '\0')
		{
			if (*p_str == '\n')
			{
				++line;
				column = 0;
			}
			if (*p_str == '*' && *(p_str + 1) == '/')
			{
				next_char();
				break;
			}
			next_char();
		}

		if (*p_str == '\0')
			error(line, --column, "expected '*/' characters");

		next_char();
	}
}

static Vector *scan()
{
	Vector *tokens = new_vector();
	int     buffer;

	while (*p_str != '\0')
	{
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
				next_char();
			}
		}

		// Comment
		else if (!strncmp(p_str, "/*", 2) || !strncmp(p_str, "//", 2))
			read_comment();

		// Symbol keywords
		else if ((buffer = search_symbol_keyword()) != -1)
		{
			Token *token = new_token(symbol_keywords[buffer].type);

			p_str  += symbol_keywords[buffer].length;
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
			error(line, column, "unknown character");
	}

	vec_push(tokens, new_token(TK_EOF));

	return tokens;
}

Vector *lexer(char *file_name)
{
	FILE *file = fopen(file_name, "r+");

	if (file == NULL)
		func_error();

	char   *text   = (p_str = read_file(file));
	Vector *tokens = scan();

	fclose(file);
	free(text);

	return tokens;
}