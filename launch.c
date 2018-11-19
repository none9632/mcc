#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h> 
#include <malloc.h>

#include "launch.h"
#include "gen.h"

int bufferCount, countL = 0;
int *point, *bufferPoint;

int CDriver(Commands command, int *var) {
	switch (command.command) {
	case CNONE:
		if (var != NULL)
			*var = command.var;
		break;
	case CNONEP:
		if (var != NULL) {
			int *point = command.var;
			*var = *point;
		}
		break;
	case CNEGP:
		if (var != NULL) {
			int *point = command.var;
			*var = -*point;
		}
		break;
	case CPLUS:
		if (var != NULL) {
			int bufferVar;
			bufferCount--;
			*var = CDriver(commands[bufferCount], var);
			bufferCount--;
			bufferVar = *var;
			*var = CDriver(commands[bufferCount], var) + bufferVar;
		}
		break;
	case CMINUS:
		if (var != NULL) {
			int bufferVar;
			bufferCount--;
			*var = CDriver(commands[bufferCount], var);
			bufferCount--;
			bufferVar = *var;
			*var = CDriver(commands[bufferCount], var) - bufferVar;
		}
		break;
	case CMULT:
		if (var != NULL) {
			int bufferVar;
			bufferCount--;
			*var = CDriver(commands[bufferCount], var);
			bufferCount--;
			bufferVar = *var;
			*var = CDriver(commands[bufferCount], var) * bufferVar;
		}
		break;
	case CDIV:
		if (var != NULL) {
			int bufferVar;
			bufferCount--;
			*var = CDriver(commands[bufferCount], var);
			bufferCount--;
			bufferVar = *var;
			*var = CDriver(commands[bufferCount], var) / bufferVar;
		}
		break;
	case CMOD:
		if (var != NULL) {
			int bufferVar;
			bufferCount--;
			*var = CDriver(commands[bufferCount], var);
			bufferCount--;
			bufferVar = *var;
			*var = CDriver(commands[bufferCount], var) % bufferVar;
		}
		break;
	case CPLUSONE:
		if (var != NULL)
			*var++;
		break;
	case CMINONE:
		if (var != NULL)
			*var--;
		break;
	case CMORE:
		break;
	case CMOREEQ:
		break;
	case CLESS:
		break;
	case CLESSEQ:
		break;
	case CEQUAL:
		break;
	case CNOTEQ:
		break;
	case CPRINT:
		if (command.var == NULL)
			error("syntax error");
		point = command.var;
		printf("%d", *point);
		break;
	case CSCAN:
		point = command.var;
		scanf("%d", point);
		break;
	case CLOAD:
		point = (int*)malloc(sizeof(int));
		bufferCount--;
		*point = CDriver(commands[bufferCount], point);
		bufferPoint = command.var;
		bufferCount = countL;
		bufferCount++;
		switch (commands[bufferCount].command)
		{
		case CPLUSA:
			*bufferPoint += *point;
			break;
		case CMINUSA:
			*bufferPoint -= *point;
			break;
		case CMULTA:
			*bufferPoint *= *point;
			break;
		case CDIVA:
			*bufferPoint /= *point;
			break;
		case CMODA:
			*bufferPoint %= *point;
			break;
		case CASSIGN:
			*bufferPoint = *point;
			break;
		default:
			error("syntax error");
			break;
		}
		break;
	}
	if (var == NULL)
		return 0;
	return *var;
}

void launching(void) {
	Commands command;
	while (commands[countL].command != CSTOP) {
		/*switch (commands[count].command) {
		case CNONE:
			printf("cnone - %d - %d\n", commands[count].var, count);
			break;
		case CNONEP:
			printf("cnonep - %d - %d\n", commands[count].var, count);
			break;
		case CLOAD:
			printf("cload - %d - %d\n", commands[count].var, count);
			break;
		case CMULT:
			printf("cmult- %d\n", count);
			break;
		case CPLUS:
			printf("cplus - %d\n", count);
			break;
		case CPRINT:
			printf("cprint - %d - %d\n", commands[count].var, count);
			break;
		case CSCAN:
			puts("cscan\n");
			break;
		case CMINONE:
			puts("cminone\n");
			break;
		case CPLUSONE:
			puts("cplusone\n");
			break;
		case CMORE:
			puts("cmore\n");
			break;
		case CLESS:
			puts("cless\n");
			break;
		case CNOTEQ:
			puts("cnoteq\n");
			break;
		case CEQUAL:
			puts("cequal\n");
			break;
		case CLESSEQ:
			puts("clesseq\n");
			break;
		case CMOREEQ:
			puts("cmoreeq\n");
			break;
		case CDIV:
			puts("cdiv\n");
			break;
		case CMOD:
			puts("cmod\n");
			break;
		case CMODA:
			puts("cmoda\n");
			break;
		case CPLUSA:
			puts("cplusa\n");
			break;
		case CMINUSA:
			puts("cminusa\n");
			break;
		case CDIVA:
			puts("cdiva\n");
			break;
		case CMULTA:
			puts("cmulta\n");
			break;
		}*/
		bufferCount = countL;
		CDriver(commands[countL], NULL);
		countL++;
	}
}