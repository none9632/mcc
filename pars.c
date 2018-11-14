#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "pars.h"
#include "table.h"
#include "scan.h"
#include "error.h"
#include "gen.h"

//char *bufferARGV;
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
		nextTok();
		return expr();
	}
	return noneType;
}

TypeVar factor(void) {
	TypeVar typeVar;
	TokenType op;
	if (tokenType == minusTok || tokenType == plusTok) {
		if (tokenType == minusTok) 
			minus = 1;
		nextTok();
	}
	typeVar = term();
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
	TypeVar typeVar;
	TokenType op;
	if (tokenType == minusTok || tokenType == plusTok) {
		if (tokenType == minusTok)
			minus = 1;
		nextTok();
	}
	typeVar = factor();
	if (typeVar == intType) {
		if (tokenType == plusTok || tokenType == minusTok ||
			tokenType == doblPTok || tokenType == doblMTok) {
			do {
				op = tokenType;
				nextTok();
				typeVar = factor();
				if (typeVar != intType) 
					error("syntax error");
				switch (op) {
				case plusTok: gen(CPLUS, NULL); break;
				case minusTok: gen(CMINUS, NULL); break;
				case doblPTok: gen(CPLUSONE, NULL); break;
				case doblMTok: gen(CMINONE, NULL); break;
				}
			} while (tokenType == plusTok || tokenType == minusTok);
		}
	}
	return typeVar;
}

TypeVar module(void){
	TypeVar typeVar;
	TokenType op;
	if (tokenType == minusTok || tokenType == plusTok) {
		if (tokenType == minusTok)
			minus = 1;
		nextTok();
	}
	typeVar = addend();
	if (typeVar == intType) {
		if (tokenType == modTok) {
			do {
				op = tokenType;
				nextTok();
				typeVar = addend();
				if (typeVar != intType)
					error("syntax error");
				switch (op) {
				case modTok: gen(CMOD, NULL); break;
				}
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
	TypeVar typeVar;
	if (tokenType == minusTok || tokenType == plusTok) {
		if (tokenType == minusTok)
			minus = 1;
		nextTok();
	}
	typeVar = test();
	if (tokenType == LbraketTok) {
		checkTok(LbraketTok);
		expr();
		checkTok(RbraketTok);
	}
	else if (tokenType == RbraketTok)
		nextTok();
	else if (typeVar == noneType) 
		error("syntax error");
	return typeVar;
}

void assing(void) {
	StackTok *nameVar = find(name);
	nextTok();
	checkTok(assignTok);
	expr();
	if (nameVar->value == NULL)
		nameVar->value = (int*)malloc(sizeof(int));
	gen(CLOAD, nameVar->value);
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

//void argv(void) {
//	int i = 0, memmory = 512;
//	nextTok();
//	checkTok(LbraketTok);
//	if (tokenType != doblQuotTok)
//		error("syntax error");
//	while (CH != '\"' || CH != EOF) {
//		if (i >= memmory || bufferARGV == NULL)
//			bufferARGV = malloc(memmory * sizeof(int));
//		getNextCH();
//		bufferARGV[i++] = CH;
//	}
//	checkTok(doblQuotTok);
//	checkTok(commaTok);
//	find(name);
//	nextTok();
//	while (tokenType != RbraketTok || tokenType != eofTok) {
//		if (tokenType != RbraketTok) {
//			checkTok(commaTok);
//			nextTok();
//		}
//		find(name);
//		nextTok();
//	}
//	checkTok(RbraketTok);
//	checkTok(semiTok);
//	free(bufferARGV);
//}

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
			gen(CSCAN, find(name)->value);
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