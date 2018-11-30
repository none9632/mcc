#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h> 
#include <malloc.h>
#include <string.h>

#include "launch.h"
#include "gen.h"

int b = 0;
int cLaunch = 0;  // counter in file launch
int bufferCount;
int *point; // for go to the pointer
int *bufferPoint;

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
	case CNEG:
		if (var != NULL) {
			point = (int*)malloc(sizeof(int));
			*point = CDriver(commands[--bufferCount], point);
			*var = -*point;
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
			*var = CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			*var = CDriver(commands[--bufferCount], var) + bufferVar;
		}
		break;
	case CMINUS:
		if (var != NULL) {
			int bufferVar;
			*var = CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			*var = CDriver(commands[--bufferCount], var) - bufferVar;
		}
		break;
	case CMULT:
		if (var != NULL) {
			int bufferVar;
			*var = CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			*var = CDriver(commands[--bufferCount], var) * bufferVar;
		}
		break;
	case CDIV:
		if (var != NULL) {
			int bufferVar;
			*var = CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			*var = CDriver(commands[--bufferCount], var) / bufferVar;
		}
		break;
	case CMOD:
		if (var != NULL) {
			int bufferVar;
			*var = CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			*var = CDriver(commands[--bufferCount], var) % bufferVar;
		}
		break;
	case CJUMP: {
		int *point = command.var;
		cLaunch = *point;
		break;
	}
	case CIF: {
		int saveCommand = commands[--bufferCount].command,
			*point = (int*)malloc(sizeof(int)),
			*bufferPoint = (int*)malloc(sizeof(int)),
			buffer = 0;
		*point = CDriver(commands[--bufferCount], point);
		*bufferPoint = CDriver(commands[--bufferCount], bufferPoint);
		cLaunch++;
		switch (saveCommand) {
		case CEQUAL:
			if (*bufferPoint == *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CNOTEQ:
			if (*bufferPoint != *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CMORE:
			if (*bufferPoint > *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CLESS:
			if (*bufferPoint < *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CMOREEQ:
			if (*bufferPoint >= *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CLESSEQ:
			if (*bufferPoint <= *point)
				cLaunch++;
			else
				buffer++;
			break;
		}
		launching(); 
		if (commands[++cLaunch].command == CELSE) {
			if (buffer == 1) 
				cLaunch += 2;
			else 
				cLaunch++;
			launching();
		}
		else
			cLaunch--;
		break;
	}
	case CWHILE: {
		int saveCommand = commands[--bufferCount].command,
			*point = (int*)malloc(sizeof(int)),
			*bufferPoint = (int*)malloc(sizeof(int));
		*point = CDriver(commands[--bufferCount], point);
		*bufferPoint = CDriver(commands[--bufferCount], bufferPoint);
		switch (saveCommand) {
		case CEQUAL:
			if (*bufferPoint == *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CNOTEQ:
			if (*bufferPoint != *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CMORE:
			if (*bufferPoint > *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CLESS:
			if (*bufferPoint < *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CMOREEQ:
			if (*bufferPoint >= *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CLESSEQ:
			if (*bufferPoint <= *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		}
		break;
	}
	case CPRINT:
		if (command.var == NULL)
			error("syntax error");
		point = command.var;
		printf("%d", *point);
		break;
	case CPRINTN: {
		int *point = (int*)malloc(sizeof(int));
		*point = CDriver(commands[--bufferCount], point);
		printf("%d", *point);
		break;
	}
	case CPRINTS: 
		printf("%s", command.var);
		break;
	case CSCAN:
		point = command.var;
		scanf("%d", point);
		break;
	case CLOAD:
		point = (int*)malloc(sizeof(int));
		*point = CDriver(commands[--bufferCount], point);
		bufferPoint = command.var;
		bufferCount = cLaunch;
		switch (commands[++bufferCount].command) {
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
		}
		break;
	}
	if (var == NULL)
		return 0;
	return *var;
}

void a(void) {
	int count = 0;
	while (commands[count].command != CEND) {
		switch (commands[count].command) {
		case CNONE:
			printf("%d cnone - %d\n", cLaunch, commands[count].var);
			break;
		case CNONEP:
			printf("%d cnonep - %d\n", cLaunch, commands[count].var);
			break;
		case CLOAD:
			printf("%d cload - %d\n", cLaunch, commands[count].var);
			break;
		case CMULT:
			printf("%d cmult\n", cLaunch);
			break;
		case CPLUS:
			printf("%d cplus\n", cLaunch);
			break;
		case CPRINT:
			printf("%d cprint - %d\n", cLaunch, commands[count].var);
			break;
		case CSCAN:
			printf("%d cscan\n", cLaunch);
			break;
		case CMORE:
			printf("%d cmore\n", cLaunch);
			break;
		case CLESS:
			printf("%d cless\n", cLaunch);
			break;
		case CNOTEQ:
			printf("%d cnoteq\n", cLaunch);
			break;
		case CEQUAL:
			printf("%d cequal\n", cLaunch);
			break;
		case CLESSEQ:
			printf("%d clesseq\n", cLaunch);
			break;
		case CMOREEQ:
			printf("%d cmoreeq\n", cLaunch);
			break;
		case CDIV:
			printf("%d cdiv\n", cLaunch);
			break;
		case CMOD:
			printf("%d cmod\n", cLaunch);
			break;
		case CMODA:
			printf("%d cmoda\n", cLaunch);
			break;
		case CPLUSA:
			printf("%d cplusa\n", cLaunch);
			break;
		case CMINUSA:
			printf("%d cminusa\n", cLaunch);
			break;
		case CDIVA:
			printf("%d cdiva\n", cLaunch);
			break;
		case CMULTA:
			printf("%d cmulta\n", cLaunch);
			break;
		case CIF:
			printf("%d cif - %d\n", cLaunch, commands[count].var);
			break;
		case CELSE:
			printf("%d celse - %d\n", cLaunch, commands[count].var);
			break;
		case CWHILE:
			printf("%d cwhile - %d\n", cLaunch, commands[count].var);
			break;
		case CJUMP: {
			int *point = commands[count].var;
			printf("%d cjump - %d\n", cLaunch, *point);
			break;
		}
		case CASSIGN:
			printf("%d cassign\n", cLaunch);
			break;
		case CSTOP:
			printf("%d cstop\n", cLaunch);
			break;
		}
		count++;
		cLaunch++;
	}
	b++;
	cLaunch = 0;
}

void launching(void) {
	//if (b == 0) a();
	while (commands[cLaunch].command != CSTOP) {
		bufferCount = cLaunch;
		CDriver(commands[cLaunch], NULL);
		cLaunch++;
	}
}