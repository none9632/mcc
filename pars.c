#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "table.h"
#include "scan.h"
#include "error.h"
#include "gen.h"
#include "launch.h"
#include "pars.h"

extern FILE* file;
int minus = 0;

void checkTok(int _tokenType) {
	if (tokenType != _tokenType)
		error("syntax error", 1);
	nextTok();
}

TypeVar term() {
	if (tokenType == minusTok || tokenType == plusTok) {
		if (tokenType == minusTok)
			minus = 1;
		nextTok();
	}
	if (tokenType == numTok) {
		if (minus == 1) {
			nValue = -nValue;
			minus = 0;
		}
		int *bufferPoint = malloc(sizeof(int));
		*bufferPoint = nValue;
		gen(CNONE, bufferPoint);
		nextTok();
		return intType;
	}
	else if (tokenType == nameTok) {
		StackTok *nameVar = find(name);
		if (nameVar->value == NULL)
			error("syntax error", 1);
		if (minus == 1) {
			gen(CNEGP, nameVar->value);
			minus = 0;
		}
		else
			gen(CNONEP, nameVar->value);
		nextTok();
		return intType;
	}
	else if (tokenType == LbraketTok) {
		int bufferMinus = 0;
		nextTok();
		if (minus == 1) {
			bufferMinus = 1;
			minus = 0;
		}
		TypeVar typeVar = expr();
		checkTok(RbraketTok);
		if (bufferMinus == 1) {
			gen(CNEG, NULL);
		}
		return typeVar;
	}
	return noneType;
}

TypeVar factor() {
	TypeVar typeVar = term();
	TokenType op;
	if (tokenType == multTok || tokenType == divTok) {
		do {
			op = tokenType;
			nextTok();
			typeVar = term();
			switch (op) {
			case multTok: gen(CMULT, NULL); break;
			case divTok: gen(CDIV, NULL); break;
			}
		} while (tokenType == multTok || tokenType == divTok);
	}
	return typeVar;
}

TypeVar addend() {
	TypeVar typeVar = factor();
	TokenType op;
	if (tokenType == plusTok || tokenType == minusTok) {
		do {
			op = tokenType;
			nextTok();
			typeVar = factor();
			switch (op) {
			case plusTok: gen(CPLUS, NULL); break;
			case minusTok: gen(CMINUS, NULL); break;
			}
		} while (tokenType == plusTok || tokenType == minusTok);
	}
	return typeVar;
}

TypeVar module(){
	TypeVar typeVar = addend();
	TokenType op;
	if (typeVar == intType) {
		if (tokenType == modTok) {
			do {
				op = tokenType;
				nextTok();
				typeVar = addend();
				if (typeVar != intType)
					error("syntax error", 1);
				gen(CMOD, NULL);
			} while (tokenType == modTok);
		}
	}
	return typeVar;
}

TypeVar test() {
	TypeVar typeVar = module();
	TokenType op;
	if (tokenType == equalTok || tokenType == moreTok || tokenType == moreEQTok ||
		tokenType == lessTok || tokenType == lessEQTok || tokenType == notEQTok) {
		op = tokenType;
		nextTok();
		typeVar = module();
		switch (op) {
		case equalTok: gen(CEQUAL, NULL); break;
		case moreTok: gen(CMORE, NULL); break;
		case moreEQTok: gen(CMOREEQ, NULL); break;
		case lessTok: gen(CLESS, NULL); break;
		case lessEQTok: gen(CLESSEQ, NULL); break;
		case notEQTok: gen(CNOTEQ, NULL); break;
		}
		return boolType;
	}
	return typeVar;
}

TypeVar expr() {
	TypeVar typeVar = test();
	if (typeVar == noneType)
		error("syntax error", 1);
	return typeVar;
}

// assignment function
void assing() {
	StackTok *nameVar = find(name);
	TokenType bufferTT;
	if (nameVar->constType == 1)
		error("syntax error", 1);
	nextTok();
	bufferTT = tokenType;
	nextTok();
	expr();
	if (nameVar->value == NULL) {
		if (tokenType == assignTok)
			nameVar->value = malloc(sizeof(int));
		else
			error("syntax error", 1);
	}
	gen(CLOAD, nameVar->value);
	switch (bufferTT) {
	case plusATok: gen(CPLUSA, NULL); break;
	case minusATok: gen(CMINUSA, NULL); break;
	case multATok: gen(CMULTA, NULL); break;
	case divATok: gen(CDIVA, NULL); break;
	case modATok: gen(CMODA, NULL); break;
	case assignTok: gen(CASSIGN, NULL); break;
	default: error("syntax error", 1); break;
	}
	checkTok(semiTok);
}

// initialization variable
void initVar() {
	TypeVar typeVar = findType(name);
	int buffer;
	if (typeVar == constType) {
		buffer = 1;
		nextTok();
		typeVar = findType(name);
	}
	if (typeVar != noneType && typeVar != constType) {
		do {
			nextTok();
			checkTok(nameTok);
			if (tokenType == assignTok) {
				char bufferName[NAMESIZE];
				strcpy(bufferName, name);
				nextTok();
				if (typeVar != expr())
					error("syntax error", 1);
				newToken(bufferName, 1, buffer);
				gen(CLOAD, find(bufferName)->value);
				gen(CASSIGN, NULL);
			}
			else
				newToken(name, 0, buffer);
		} while (tokenType == commaTok);
	}
	else
		error("unknown type", 1);
	checkTok(semiTok);
}


// initialization while
void initWhile() {
	int *point = malloc(sizeof(int)),
		*bufferPoint = malloc(sizeof(int));
	nextTok();
	checkTok(LbraketTok);
	if (expr() != boolType)
		error("syntax error", 1);
	checkTok(RbraketTok);
	*bufferPoint = cGen - 1;
	gen(CWHILE, bufferPoint);
	gen(CJUMP, point);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
	*point = cGen;
	gen(CSTOP, NULL);
}

// initialization if
void initIf() {
	int *point1 = malloc(sizeof(int)),
		*point2 = malloc(sizeof(int));
	nextTok();
	checkTok(LbraketTok);
	if (expr() != boolType)
		error("syntax error", 1);
	checkTok(RbraketTok);
	gen(CIF, NULL);
	gen(CJUMP, point1);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
	*point1 = cGen - 1;
	gen(CSTOP, NULL);
	if (tokenType == elseTok) {
		gen(CELSE, NULL);
		gen(CJUMP, point2);
		nextTok();
		checkTok(LbracesTok);
		statement();
		checkTok(RbracesTok);
		*point2 = cGen - 1;
		gen(CSTOP, NULL);
	}
}

// output function initialization
void initPrint() {
	TokenType bufferTT = tokenType;
	nextTok();
	checkTok(LbraketTok);
	if (tokenType == nameTok) {
		gen(CPRINT, find(name)->value);
		nextTok();
	}
	else if (tokenType == numTok) {
		if (expr() != intType)
			error("syntax error", 1);
		gen(CPRINTN, NULL);
	}
	else if (tokenType == doblQuotTok) {
		int *text = malloc(sizeof(char) * 1024);
		int i = 0;
		while (CH != '\"' && CH != EOF) {
			if (i >= 1024)
				error("big the text", 1);
			text[i++] = CH;
			getNextCH();
		}
		if (CH == EOF)
			error("syntax error", 1);
		text[i] = '\0';
		getNextCH();
		gen(CPRINTS, text);
		nextTok();
	}
	else
		error("syntax error", 1);
	if (bufferTT == printlnTok)
		gen(CPRINTLN, NULL);
	checkTok(RbraketTok);
	checkTok(semiTok);
}

// initialization of the input function
void initInput() {
	nextTok();
	checkTok(LbraketTok);
	if (tokenType != nameTok)
		error("syntax error", 1);
	StackTok *nameVar = find(name);
	if (nameVar->value == NULL)
		nameVar->value = malloc(sizeof(int));
	gen(CINPUT, nameVar->value);
	nextTok();
	checkTok(RbraketTok);
	checkTok(semiTok);
}

void statement() {
	while (tokenType != RbracesTok && tokenType != eofTok) {
		if (tokenType == nameTok)
			assing();
		else if (tokenType == typeTok)
			initVar();
		else if (tokenType == whileTok)
			initWhile();
		else if (tokenType == ifTok)
			initIf();
		else if (tokenType == printTok || tokenType == printlnTok)
			initPrint();
		else if (tokenType == inputTok)
			initInput();
	}
	if (tokenType == eofTok)
		error("syntax error", 1);
}

void parsing(void) {
	newToken("", 0, 2);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
	gen(CSTOP, NULL);
}
