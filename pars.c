#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "pars.h"
#include "table.h"
#include "scan.h"
#include "error.h"
#include "gen.h"

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
	checkTok(whileTok);
	checkTok(LbraketTok);
	if (expr() != boolType)
		error("syntax error");
	checkTok(RbraketTok);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
}

void initIf(void) {
	checkTok(ifTok);
	checkTok(LbraketTok);
	if (expr() != boolType)
		error("syntax error");
	checkTok(RbraketTok);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
	if (tokenType == elseTok) {
		checkTok(elseTok);
		checkTok(LbracesTok);
		statement();
		checkTok(RbracesTok);
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
			if (tokenType != nameTok)
				error("syntax error");
			gen(CPRINT, find(name)->value);
			nextTok();
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
}

void parsing(void) {
	newToken("", NULL, startTok, noneType);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
	gen(CSTOP, NULL);
}