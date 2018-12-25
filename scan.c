#include <stdio.h>
#include <ctype.h>

#include "table.h"
#include "pars.h"
#include "error.h"
#include "scan.h"

extern FILE* file;
char CH, bufferCH;
TokenType tokenType;
int nValue; // num value
char name[NAMESIZE];
int posLine = 1, posSym = 0; // character and line position

void getNextCH() {
	CH = bufferCH;
	bufferCH = fgetc(file);
	posSym++;
}

void readIdent() {
	int i = 0;
	while (isalnum(CH)) {
		if (i > NAMESIZE - 1)
			error("big the name", 1);
		name[i++] = CH;
		getNextCH();
	}
	name[i] = '\0';
	tokenType = searchTN(name);
	if (tokenType == noneTok)
		tokenType = nameTok;
}

void readNum() {
	int i = 10;
	nValue = 0;
	while (isdigit(CH)) {
		nValue = nValue * 10 + CH - '0';
		getNextCH();
	}
	tokenType = numTok;
}

void nextTok() {
	while (isspace(CH)) {
		if (CH == '\n') {
			posSym = 0;
			posLine++;
		}
		getNextCH();
	}
	if (isalpha(CH))
		readIdent();
	else if (isdigit(CH))
		readNum();
	else {
		switch (CH)	{
		case '+':
			if (bufferCH == '=') {
				tokenType = plusATok;
				getNextCH();
			}
			else
				tokenType = plusTok;
			getNextCH();
			break;
		case '-':
			if (bufferCH == '=') {
				tokenType = minusATok;
				getNextCH();
			}
			else
				tokenType = minusTok;
			getNextCH();
			break;
		case '/':
			if (bufferCH == '*') {
				while (CH != EOF) {
					if (CH == '*' && bufferCH == '/')
						break;
					getNextCH();
				}
				getNextCH();
				getNextCH();
				nextTok();
				break;
			}
			else if (bufferCH == '/') {
				while (CH != EOF) {
					if (CH == '\n')
						break;
					getNextCH();
				}
				getNextCH();
				nextTok();
				break;
			}
			else if (bufferCH == '=') {
				tokenType = divATok;
				getNextCH();
			}
			else
				tokenType = divTok;
			getNextCH();
			break;
		case '*':
			if (bufferCH == '=') {
				tokenType = multATok;
				getNextCH();
			}
			else
				tokenType = multTok;
			getNextCH();
			break;
		case '(':
			tokenType = LbraketTok;
			getNextCH();
			break;
		case ')':
			tokenType = RbraketTok;
			getNextCH();
			break;
		case ';':
			tokenType = semiTok;
			getNextCH();
			break;
		case ',':
			tokenType = commaTok;
			getNextCH();
			break;
		case '>':
			if (bufferCH == '=') {
				tokenType = moreEQTok;
				getNextCH();
			}
			else
				tokenType = moreTok;
			getNextCH();
			break;
		case '<':
			if (bufferCH == '=') {
				tokenType = lessEQTok;
				getNextCH();
			}
			else
				tokenType = lessTok;
			getNextCH();
			break;
		case '=':
			if (bufferCH == '=') {
				tokenType = equalTok;
				getNextCH();
			}
			else
				tokenType = assignTok;
			getNextCH();
			break;
		case '{':
			tokenType = LbracesTok;
			getNextCH();
			break;
		case '}':
			tokenType = RbracesTok;
			getNextCH();
			break;
		case '%':
			if (bufferCH == '=') {
				tokenType = modATok;
				getNextCH();
			}
			else
				tokenType = modTok;
			getNextCH();
			break;
		case '!':
			if (bufferCH == '=') {
				tokenType = notEQTok;
				getNextCH();
			}
			else
				error("syntax error", 1);
			getNextCH();
			break;
		case '\"':
			tokenType = doblQuotTok;
			getNextCH();
			break;
		case EOF:
			tokenType = eofTok;
			break;
		default:
			error("syntax error", 1);
			break;
		}
	}
}
