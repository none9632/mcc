#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "pars.h"
#include "table.h"
#include "scan.h"
#include "error.h"
#include "gen.h"

extern FILE* file;
int minus = 0;

void checkTok(TokenType _tokenType) {
	if (tokenType != _tokenType)
		error("syntax error");
	nextTok();
}

TypeVar term(void) {
	if (tokenType == minusTok || tokenType == plusTok) {
		if (tokenType == minusTok)
			minus = 1;
		nextTok();
	}
	if (tokenType == numTok) {
		if (minus == 1) {
			value = -value;
			minus = 0;
		}
		gen(CNONE, value);
		nextTok();
		return intType;
	}
	else if (tokenType == nameTok) {
		StackTok *nameVar = find(name);
		if (nameVar->value == NULL)
			error("syntax error");
		if (minus == 1) {
			gen(CNEGP, nameVar->value);
			minus = 0;
		}
		else 
			gen(CNONEP, nameVar->value);
		nextTok();
		return nameVar->typeVar;
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

TypeVar factor(void) {
	TypeVar typeVar = term();
	TokenType op;
	if (typeVar == intType) {
		if (tokenType == multTok || tokenType == divTok) {
			do {
				op = tokenType;
				nextTok();
				typeVar = term();
				if (typeVar != intType)
					error("syntax error");
				switch (op) {
				case multTok: gen(CMULT, NULL); break;
				case divTok: gen(CDIV, NULL); break;
				}
			} while (tokenType == multTok || tokenType == divTok);
		}
	}
	return typeVar;
}

TypeVar addend(void) {
	TypeVar typeVar = factor();
	TokenType op;
	if (typeVar == intType) {
		if (tokenType == plusTok || tokenType == minusTok) {
			do {
				op = tokenType;
				nextTok();
				typeVar = factor();
				if (typeVar != intType) 
					error("syntax error");
				switch (op) {
				case plusTok: gen(CPLUS, NULL); break;
				case minusTok: gen(CMINUS, NULL); break;
				}
			} while (tokenType == plusTok || tokenType == minusTok);
		}
	}
	return typeVar;
}

TypeVar module(void){
	TypeVar typeVar = addend();
	TokenType op;
	if (typeVar == intType) {
		if (tokenType == modTok) {
			do {
				op = tokenType;
				nextTok();
				typeVar = addend();
				if (typeVar != intType)
					error("syntax error");
				gen(CMOD, NULL);
			} while (tokenType == modTok);
		}
	}
	return typeVar;
}

TypeVar test(void) {
	TypeVar typeVar = module();
	TokenType op;
	if (typeVar == intType) {
		if (tokenType == equalTok || tokenType == moreTok || tokenType == moreEQTok ||
			tokenType == lessTok || tokenType == lessEQTok || tokenType == notEQTok) {
			op = tokenType;
			nextTok();
			typeVar = module();
			if (typeVar != intType)
				error("syntax error");
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
	}
	return typeVar;
}

TypeVar expr(void) { 
	TypeVar typeVar = test();
	if (typeVar == noneType) 
		error("syntax error");
	return typeVar;
}

void assing(void) {
	StackTok *nameVar = find(name);
	TokenType bufferTT;
	nextTok();
	bufferTT = tokenType;
	nextTok();
	expr(); 
	if (nameVar->value == NULL) {
		if (tokenType == assignTok)
			nameVar->value = (int*)malloc(sizeof(int));
		else
			error("syntax error");
	}
	gen(CLOAD, nameVar->value);
	switch (bufferTT) {
	case plusATok: gen(CPLUSA, NULL); break;
	case minusATok: gen(CMINUSA, NULL); break;
	case multATok: gen(CMULTA, NULL); break;
	case divATok: gen(CDIVA, NULL); break;
	case modATok: gen(CMODA, NULL); break;
	case assignTok: gen(CASSIGN, NULL); break;
	default: error("syntax error"); break;
	}
	checkTok(semiTok);
}

void initVar(void) {
	TypeVar typeVar = findType(name);
	if (typeVar != noneType) {
		nextTok();
		checkTok(nameTok);
		if (tokenType == assignTok) {
			char bufferName[NAMESIZE];
			strcpy(bufferName, name);
			nextTok();
			if (typeVar != expr())
				error("syntax error");
			newToken(bufferName, 1, varTok, typeVar);
			gen(CLOAD, find(bufferName)->value);
			gen(CASSIGN, NULL);
		}
		else
			newToken(name, NULL, varTok, typeVar);
	}
	else
		error("unknown type");
	checkTok(semiTok);
}

void initWhile(void) {
	int *point = (int*)malloc(sizeof(int));
	nextTok();
	checkTok(LbraketTok);
	if (expr() != boolType)
		error("syntax error");
	checkTok(RbraketTok);
	gen(CWHILE, cGen - 2);
	gen(CJUMP, point);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
	*point = cGen;
	gen(CSTOP, NULL);
}

void initIf(void) {
	int *point1 = (int*)malloc(sizeof(int)),
		*point2 = (int*)malloc(sizeof(int));
	nextTok();
	checkTok(LbraketTok);
	if (expr() != boolType) 
		error("syntax error");
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

void statement(void) {
	while (tokenType != RbracesTok && tokenType != eofTok) {
		if (tokenType == nameTok) {
			puts("a");
			assing();
		}
		else if (tokenType == typeTok) {
			puts("b");
			initVar();
		}
		else if (tokenType == whileTok) {
			puts("c");
			initWhile();
		}
		else if (tokenType == ifTok) {
			puts("s");
			initIf();
		}
		else if (tokenType == printTok) {
			puts("d");
			nextTok();
			checkTok(LbraketTok);
			if (tokenType == nameTok) {
				gen(CPRINT, find(name)->value);
				nextTok();
			}
			else if (tokenType == numTok) {
				if (expr() != intType) 
					error("syntax error");
				gen(CPRINTN, NULL);
			}
			else if (tokenType == doblQuotTok) {
				char *text = (char*)malloc(sizeof(char) * 1024);
				int i = 0;
				while (CH != '\"' && CH != EOF) {
					if (i >= 1024)
						error("big the text");
					text[i++] = CH;
					getNextCH();
				}
				if (CH == EOF) 
					error("syntax error");
				text[i] = '\0';
				getNextCH();
				gen(CPRINTS, text);
				nextTok();
			}
			else
				error("syntax error");
			checkTok(RbraketTok);
			checkTok(semiTok);
		}
		else if (tokenType == scanTok) {
			puts("n");
			nextTok();
			checkTok(LbraketTok);
			if (tokenType != nameTok)
				error("syntax error");
			StackTok *nameVar = find(name);
			if (nameVar->value == NULL) 
				nameVar->value = (int*)malloc(sizeof(int));
			gen(CSCAN, nameVar->value);
			nextTok();
			checkTok(RbraketTok);
			checkTok(semiTok);
		}
	}
	if (tokenType == eofTok)
		error("syntax error");
}

void parsing(void) {
	newToken("", NULL, startTok, noneType);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
	gen(CSTOP, NULL);
	gen(CEND, NULL); // for the test
}