#ifndef SCAN_H
#define SCAN_H

#include "table.h"

extern TokenType tokenType;
extern int value;
extern char name[NAMESIZE];

void nextTok(void);	

#endif