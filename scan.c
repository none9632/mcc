#include "scan.h"

char CH, bufferCH = ' ';
int token;

void getNextCH(void)
{
	CH = bufferCH;
	bufferCH = fgetc(file);
}

void eatComment(void)
{
	while (CH != EOF || (CH != '*' && bufferCH != '/'))
	{
		getNextCH();
	}
}

void readIdent(void)
{
	getNextCH();
	while (isalnum(CH))
	{
		getNextCH();
	}
}

void readNum(void)
{
	getNextCH();
	while (isdigit(CH))
	{
		getNextCH();
	}
}

void scanning(void)
{
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
			token = minusTok;
			break;
		case '+':
			token = plusTok;
			break;
		case '/':
			if (bufferCH == '*')
				eatComment();
			else
				token = divTok;
			break;
		case '*':
			token = multTok;
			break;
		case '(':
			token = LbraketTok;
			break;
		case ')':
			token = RbraketTok;
			break;
		case ';':
			token = semiTok;
			break;
		case ',':
			token = commaTok;
			break;
		case '>':
			if (bufferCH == '=')
				token = moreEQTok;
			else
				token = moreTok;
			break;
		case '<':
			if (bufferCH == '=')
				token = lessEQTok;
			else
				token = lessTok;
			break;
		case '=':
			if (bufferCH == '=')
				token = compTok;
			else
				token = equalTok;
		default:
			error("syntax error");
			break;
		}
	}
}
