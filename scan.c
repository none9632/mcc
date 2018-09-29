#include <stdio.h>
#include <ctype.h>

#include "pars.h"
#include "error.h"
#include "scan.h"

#define SIZETABLE 6

char CH, bufferCH = ' ';
int count = 0;
TokenType tokenType;
int number;
char name[NAMESIZE];

struct embeddedNames 
{
	char name[5];
	TokenType type;
} TableNames[SIZETABLE];

void enterTableNames(char *name, TokenType type) 
{
	strcpy(TableNames[count].name, name);
	TableNames[count++].type = type;
}

void searchTableNames(char *name) 
{
	for (int i = 0; i < SIZETABLE; i++) 
	{
		if (TableNames[i].name == name) 
		{
			tokenType = TableNames[i].type;
			return;
		}
	}
	tokenType = nameTok;
}

void getNextCH(void)
{
	CH = bufferCH;
	bufferCH = fgetc(file);
}

void eatComment(void)
{
	while (CH != EOF || (CH != '*' && bufferCH != '/'))
		getNextCH();
}

void readIdent(void)
{
	int i = 0;
	while (isalnum(CH))
	{
		if (i > NAMESIZE)
			error("big a name");
		name[i++] = CH;
		getNextCH();
	}
	searchTableNames(name);
}

void readNum(void)
{
	number = 0;
	while (isdigit(CH))
	{
		number *= 10 + CH - '0';
		getNextCH();
	}
}

void scanning(void)
{
	enter();
	getNextCH();
	while (isspace(CH))
		getNextCH();
	if (isalpha(CH))
	{
		readIdent();
	}
	else if (isalnum)
	{
		readNum();
	}
	else
	{
		switch (CH)
		{
		case '-':
			tokenType = minusTok;
			break;
		case '+':
			tokenType = plusTok;
			break;
		case '/':
			if (bufferCH == '*')
				eatComment();
			else
				tokenType = divTok;
			break;
		case '*':
			tokenType = multTok;
			break;
		case '(':
			tokenType = LbraketTok;
			break;
		case ')':
			tokenType = RbraketTok;
			break;
		case ';':
			tokenType = semiTok;
			break;
		case ',':
			tokenType = commaTok;
			break;
		case '>':
			if (bufferCH == '=')
				tokenType = moreEQTok;
			else
				tokenType = moreTok;
			break;
		case '<':
			if (bufferCH == '=')
				tokenType = lessEQTok;
			else
				tokenType = lessTok;
			break;
		case '=':
			if (bufferCH == '=')
				tokenType = compTok;
			else
				tokenType = equalTok;
			break;
		case '{':
			tokenType = RbracesTok;
			break;
		case '}':
			tokenType = LbracesTok;
			break;
		default:
			error("syntax error");
			break;
		}
	}
}

void enter(void) 
{
	enterTableNames("var", varTok);
	enterTableNames("mod", modTok);
	enterTableNames("while", whileTok);
	enterTableNames("if", ifTok);
	enterTableNames("else", elseTok);
	enterTableNames("for", forTok);
}
