/**************************************************************************
 * (c) 2009 Nicolas DUPEUX                      nicolas@dupeux.net
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

/* Grammar file for DIBOL language. */

%{

#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <glib.h>
#include <getopt.h>
#include "dpplexer.h"

GSList *fields=NULL;
extern GSList *include_dirs;
extern int output;
extern int in_record;

/* This function open include file. If the filename is a VMS filename,
we convert it to a unix filename. Then the directory search order is :
- Current directory
- include dir specified by -I command line option
*/
FILE* open_include_file(char *filename) {
	GSList *iter = NULL;
	char unixfilename[PATH_MAX];
	char *include_filename;
	FILE *ifile;

	if (is_vms_name(filename)) {
		make_unix_filespec(unixfilename,filename);
	} else {
		strcpy(unixfilename,filename);
	}

	for(iter=include_dirs;iter;iter=iter->next) {
		include_filename=g_strconcat((char*)iter->data,"/",unixfilename,NULL);
		if ( (ifile=fopen(include_filename,"r"))!=NULL )
			return ifile;
		free(include_filename);
	}
}

%}

%union {
	long long num;
	char *str;
}


%token ifdef
%token ifndef
%token endc
%token include
%token title
%token id
%token fid
%token qstring
%token subroutine
%token proc
%token record
%token common

%start stmts

%type <str> qstring
%type <str> id
%type <str> fid

%%

stmts:
	stmt
	| stmts stmt
	;

stmt:
	ifdef id {
		if (g_slist_find_custom(fields,$2,(GCompareFunc)strcmp)!=NULL && output==1)
			output=1;
		else
			output=0;
	}
	| ifndef id {
		if (g_slist_find_custom(fields,$2,(GCompareFunc)strcmp)==NULL)
			output=1;
		else
			output=0;
	}
	| endc {
	}
	| include qstring {
		FILE *ifile;
		char *include_filename;
		GSList *iter = NULL;
		YY_BUFFER_STATE yybs;
		/* Remove first and last character */
		/* as string came with delimiters */
		$2++;
		$2[strlen($2)-1]='\0';
		ifile=open_include_file($2);
		if (ifile==NULL) {
			fprintf(stderr,"Can't find include file %s\n",$2);
			exit(EXIT_FAILURE);
		}
		yybs=yy_create_buffer(ifile,YY_BUF_SIZE);
		yypush_buffer_state(yybs);
	}
	| id {
		if (output) printf("%s",yylval.str);
	}
	| fid {
		if (output) {
			printf("%s",yylval.str);
			if (in_record) {
				fields=g_slist_prepend(fields,$1);
			}
		}
	}
	| qstring {
		if (output) printf("%s",yylval.str);
	}
	| record
	| record id {
		if (output) {
			printf("%s",yylval.str);
			fields=g_slist_prepend(fields,$2);
		}
	}
	| common
	| proc
	| subroutine
	| title
	| title qstring
	;

%%

