#include <stdio.h>
#include <ctype.h>

#include "table.h"
#include "pars.h"
#include "error.h"
#include "scan.h"

extern FILE* file;
char CH, bufferCH = ' ';
TokenType tokenType;
int value;
char name[NAMESIZE];

void getNextCH(void) {
	CH = bufferCH;
	bufferCH = fgetc(file);
}

void eatComment(void) {
	while (CH != EOF || (CH != '*' && bufferCH != '/'))
		getNextCH();
}

void readIdent(void) {
	int i = 0;
	while (isalnum(CH)) {
		if (i > NAMESIZE - 1)
			error("big a name");
		name[i++] = CH;
		getNextCH();
	}
	name[NAMESIZE] = '\0';
	tokenType = searchTN(name);
	if (tokenType == 0)
		tokenType = nameTok;

}

void readNum(void) {
	value = 0;
	while (isdigit(CH)) {
		value *= 10 + CH - '0';
		getNextCH();
	}
}

void nextTok(void) {
	getNextCH();
	while (isspace(CH))
		getNextCH();
	if (isalpha(CH)) {
		readIdent();
	}
	else if (isalnum) {
		readNum();
	}
	else {
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
		case '%':
			tokenType = modTok;
			break;
		default:
			error("syntax error");
			break;
		}
	}
}

