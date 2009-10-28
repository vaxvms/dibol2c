#ifndef D2C_H
#define D2C_H

#include <stdio.h>

#define OPENMODESTATE		1
#define OPENOPTIONSTATE		2
#define LPQUEOPTIONSTATE	3

#define false 0
#define true 1

#define tRECORD		0
#define tALPHA		1
#define tDECIMAL	2
#define tINTEGER	3	// Not supported yet, just in case...

typedef struct {
	char* name;
	int type;
	int size;
	int nbelts;
	char* data;
} FieldDef;

extern int specialstate;

/* External variable defined in the lexer */
#ifndef TOTO
extern int lineno;          /* Number of the line we are looking at */
#endif
extern char* yytext;          /* Text of the lastest token */
extern FILE* yyin;            /* Input file */
 
FILE* csrc;             /* Handle of the file where we'll write the output */
 
char* srcfilename;      /* Name of the file we are processing */

char* changeExt(char* filename,char *ext);
char* toSymbol(char* str);

#endif
