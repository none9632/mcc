#ifndef TABLE_H
#define TABLE_H

#define SIZETABLE 9
#define NAMESIZE 32

typedef enum {
	semiTok, commaTok, nameTok, numTok, LbraketTok,
	RbraketTok, multTok, divTok, moreTok, lessTok,
	moreEQTok, lessEQTok, assignTok, minusTok, plusTok,
	equalTok, modTok, RbracesTok, LbracesTok, forTok,
	elseTok, whileTok, ifTok, typeTok, varTok, noneTok,
	notEQTok, printTok, inputTok, doblQuotTok, eofTok,
	multATok, divATok, plusATok, minusATok, modATok,
	printlnTok,
} TokenType;

typedef enum {
	intType, noneType, constType, boolType
} TypeVar;

struct embeddedNames {
	char name[7];
	TokenType type;
} TableNames[SIZETABLE];

typedef struct Token {
	char name[NAMESIZE];
	int *value;
	int constType;
	struct Token* low;
} StackTok;

extern StackTok *top;

void newToken(char *name, int value, int constType);
TokenType searchTN(char *name);
StackTok* find(char *name);
TypeVar findType(char *name);
void enter();

#endif
