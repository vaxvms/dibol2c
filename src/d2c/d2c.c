/**************************************************************************
 * (c) 2001 Nicolas DUPEUX                      nicolas@dupeux.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **************************************************************************/

/* main routine, argument decoding */

#define TOTO 1
#include "d2c.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>
#include <getopt.h>

int lineno=1;
extern int yydebug;

char* toSymbol(char* str)
{
	char* symbol;
	char* result;
	result=symbol=strdup(str);
	while (*symbol) {
		switch (*symbol) {
		case '.' : *symbol='_';
		default : *symbol=toupper(*symbol);
		}
		symbol++;
	}
	return result;
}

/* Report an error */
int yyerror(const char *msg) {
	fprintf(stderr,"** line %d: %s at '%s'\n", lineno, msg, yytext);
}

/*
        return a pointer to a newly allocated string
	containing the filename with extension
        changed to ext
*/
char* changeExt(char* filename,char *ext)
{
        int i;
        char* newfilename;
        i = strlen(filename) - 1;
        while (i >= 0 && filename[i] != '.')
		i--;
	if (filename[i] == '.') {
		int extlen=strlen(ext);
		newfilename=(char*)malloc((i+extlen+2)*sizeof(char));
		memcpy(newfilename,filename,i+1);
		memcpy(newfilename+i+1,ext,extlen);
		newfilename[i+extlen+1]='\0';
		return newfilename;
	}
	else
		return g_strconcat(filename, ext, NULL);
}

/****************************************/
int main(int argc, char **argv)
{
	FILE *src;
	char *destname;
	int result;
	int c;

	while ((c = getopt (argc, argv, "d")) != -1)
		switch (c) {
		case 'd':
			yydebug=1;
			break;
		default:
			exit(EXIT_FAILURE);
		}

	if (optind >= argc) {
		fprintf(stderr,"%s: No input file\n",argv[0]);
		exit(1);
	}

	if (! (src = fopen(argv[optind], "r")))
	{
		fprintf(stderr,"could not open %s\n",argv[1]);
		exit(2);
	}

	yyin = src;
	srcfilename=argv[1];

	destname=changeExt(argv[optind],"c");
	if (!(csrc=fopen(destname,"w")))
	{
		fprintf(stderr,"could not write file %s\n",destname);
		free(destname);
		exit(3);
	}
	
	fprintf(csrc,"/*\n\tdibol program translated to C by d2c\n");
	fprintf(csrc,"\td2c is (c) Nicolas DUPEUX <nicolas@dupeux.net>\n");
	fprintf(csrc,"*/\n");
	fprintf(csrc,"\n#include \"dibol.h\"\n\n");
	result=yyparse();

	free(destname);
	fclose(csrc);
	fclose(src);
	return result;
}

