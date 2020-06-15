#include "../include/lexer.h"

#define TABLE_KEYWORDS_SIZE 7
#define TABLE_SYMBOLS_SIZE 26

typedef struct keyword
{
	char *data;
	int type;
	int length;
}
Keyword;

static int g_line = 1;
static int g_column = 1;

static const Keyword table_keywords[TABLE_KEYWORDS_SIZE] =
{
	{"if",       TK_IF,       0},
	{"else",     TK_ELSE,     0},
	{"do",       TK_DO,       0},
	{"while",    TK_WHILE,    0},
	{"int",      TK_INT,      0},
	{"input",    TK_INPUT,    0},
	{"print",    TK_PRINT,    0},
};

static const Keyword table_symbols[TABLE_SYMBOLS_SIZE] =
{
	{"+=", TK_PLUSA, 2},  {"-=", TK_MINUSA, 2},  {"*=", TK_MULTA, 2},   {"/=", TK_DIVA, 2},
	{"%=", TK_MODA, 2},   {"<=", TK_LESSEQ, 2},  {">=", TK_MOREEQ, 2},  {"==", TK_EQUAL, 2},
	{"!=", TK_NOTEQ, 2},  {"||", TK_OR, 2},      {"&&", TK_AND, 2},     {"+", '+', 1},
	{"-", '-', 1},        {"*", '*', 1},         {"/", '/', 1},         {"%", '%', 1},
	{"=", '=', 1},        {"<", '<', 1},         {">", '>', 1},         {"(", '(', 1},
	{")", ')', 1},        {"{", '{', 1},         {"}", '}', 1},         {";", ';', 1},
	{":", ':', 1},        {",", ',', 1}
};

static int search_TK(char *name)
{
	for (int i = 0; i < TABLE_KEYWORDS_SIZE; i++)
	{
		if (strcmp(name, table_keywords[i].data) == 0)
			return table_keywords[i].type;
	}

	return TK_IDENT;
}

static char *read_file(FILE *file)
{
	char *str = NULL;
	char buffer[4096];
	unsigned int count_read, length = 1;

	do
	{
		count_read = fread(buffer, sizeof(char), 4096, file);
		length += count_read;
		str = realloc(str, length);
		memcpy(str + length - count_read - 1, buffer, count_read);
	}
	while (count_read != 0);

	return str;
}

static int read_symbols(Token *t, char *p_str)
{
	for (int i = 0; i < TABLE_SYMBOLS_SIZE; i++)
	{
		if (!strncmp(table_symbols[i].data, p_str, table_symbols[i].length))
		{
			t->type = table_symbols[i].type;
			t->line = g_line;
			t->column = g_column;
			return table_symbols[i].length;
		}
	}
	return 0;
}

static char *read_ident(Token *t, char *p_str)
{
	char *buf_p = p_str;
	int length = 1;

	while (isalnum(*p_str) || *p_str == '_')
	{
		++p_str;
		++length;
		++g_column;
	}

	t->str = malloc(sizeof(char) * length);
	memcpy(t->str, buf_p, length - 1);
	t->str[length - 1] = '\0';
	t->type = search_TK(t->str);
	t->length = length;

	return p_str;
}

static char *read_num(Token *t, char *p_str)
{
	t->value = 0;

	while (isdigit(*p_str))
	{
		t->value = t->value * 10 + *p_str - '0';
		++p_str;
		++g_column;
	}

	t->type = TK_NUM;

	return p_str;
}

static char *read_str(Token *t, char *p_str)
{
	char *buf_p = ++p_str;
	int length = 1;
	++g_column;

	while (*p_str != '\"' && *p_str != '\0')
	{
		if (*p_str == '\n')
		{
			++g_line;
			g_column = 0;
		}
		++p_str;
		++length;
		++g_column;
	}

	if (*p_str == '\0')
		error("missing terminating \" character", g_line, --g_column);

	++g_column;

	t->str = malloc(sizeof(char) * length);
	memcpy(t->str, buf_p, length - 1);
	t->str[length - 1] = '\0';
	t->type = TK_STR;

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
		++g_line;
		g_column = 1;
	}
	else if (!strncmp(p_str, "/*", 2))
	{
		while (*p_str != '\0')
		{
			if (*p_str == '\n')
			{
				++g_line;
				g_column = 1;
			}
			if (*++p_str == '*' && *(p_str + 1) == '/')
			{
				g_column += 2;
				++p_str;
				break;
			}
			++g_column;
		}

		if (*p_str == '\0')
			error("expected */ characters", g_line, --g_column);

		++p_str;
		++g_column;
	}
	return p_str;
}

static Vector *scan(char *p_str)
{
	Vector *tokens = new_vec();
	int offset;

	while (*p_str != '\0')
	{
		Token *t = malloc(sizeof(Token));

		// Whitespace
		while (isspace(*p_str))
		{
			if (*p_str == '\n')
			{
				g_column = 0;
				++g_line;
			}
			++p_str;
			++g_column;
		}

		// Comment
		if (!strncmp(p_str, "/*", 2) || !strncmp(p_str, "//", 2))
			p_str = read_comment(p_str);

		// Symbols
		else if ((offset = read_symbols(t, p_str)) != 0)
		{
			p_str += offset;
			g_column += offset;
			vec_push(tokens, t);
		}

		// Identifier
		else if (isalpha(*p_str) || *p_str == '_')
		{
			t->line = g_line;
			t->column = g_column;
			p_str = read_ident(t, p_str);
			vec_push(tokens, t);
		}

		// Number literal
		else if (isdigit(*p_str))
		{
			t->line = g_line;
			t->column = g_column;
			p_str = read_num(t, p_str);
			vec_push(tokens, t);
		}

		// String literal
		else if (*p_str == '\"')
		{
			t->line = g_line;
			t->column = g_column;
			p_str = read_str(t, p_str);
			vec_push(tokens, t);
		}

		// Unknown character
		else
			error("unknown character", g_line, g_column);
	}

	return tokens;
}

Vector *lexer(char *file_name)
{
	FILE *file = fopen(file_name, "r");
	if (file == NULL)
	{
		printf("name: %s\n", file_name);
		error("file can't open", 0, 0);
	}

	char *str = read_file(file);
	Vector *tokens = scan(str);

	fclose(file);
	free(str);
	return tokens;
}