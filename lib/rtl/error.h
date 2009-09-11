#ifndef ERROR_H
#define ERROR_H

#include <setjmp.h>

typedef struct {
	int code;
	char* file;
	int line;
	jmp_buf ctx;
	int active;
	} Errorhandle;

#define ONERROR(LABEL) \
	dibol->errorhandle.active=1; \
	if (setjmp(dibol->errorhandle.ctx)!=0) goto LABEL

#define OFFERROR \
	dibol->errorhandle.active=0;

void error(int code);

#endif
