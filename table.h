#ifndef TABLE_H
#define TABLE_H

#define SIZETABLE 4
#define NAMESIZE 32

typedef enum {
	semiTok, commaTok, nameTok, numTok, LbraketTok,
	RbraketTok, multTok, divTok, moreTok, lessTok,
	moreEQTok, lessEQTok, equalTok, minusTok, plusTok,
	compTok, modTok, RbracesTok, LbracesTok, forTok,
	elseTok, whileTok, ifTok, startTok, noneTok,
	typeTok
} TokenType;

typedef enum {
	intType, doubleType, charType, noneType
} TypeVar;

struct embeddedNames {
	char name[6];
	TokenType type;
} TableNames[SIZETABLE];

typedef struct Token {
	char name[NAMESIZE];
	int value;
	TokenType tokenType;
	TypeVar typeVar;
	struct Token* low;
} StackTok;

void newToken(char *_name, int _number, TokenType _tokenType, TypeVar _typeVar);
int searchTN(char *_name);
StackTok* find(char *_name);
TypeVar findType(char *_name);
void enter(void);

#endif