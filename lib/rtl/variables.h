#ifndef VARIABLES_H
#define VARIABLES_H

#define tRECORD		0
#define tALPHA		1
#define tDECIMAL	2
#define tINTEGER	3

#include <stdio.h>

/* This structure contains all the neccessary things to
   handle the declared variables */
typedef struct {
	char* data;
	int offset;
} Varhandle;

/* description of a variable */
typedef struct {
	int type;
	int freetype;
	size_t size;
	size_t nbelts;
	char* data;
} variable;

Varhandle* newVarhandle(size_t size);
void freeVarhandle(Varhandle* vh);

variable* newvar (Varhandle* vh, int type, size_t size, size_t nbelts, int redef, int noname, char* defval);
variable* imm (int type, size_t size, char* data);
void freevar(variable* var);

variable* indexvar(Varhandle* vh, variable* var, int index);
variable* substring(variable* var, variable* start, variable* end);

int gettype(variable* var);
int getsize(variable* var);
char* getdata(variable* var);

int isTrue(variable* var);

#endif
