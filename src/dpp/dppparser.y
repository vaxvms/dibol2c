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
GHashTable *defines=NULL;
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
	return NULL;
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
%token define
%token comma
%token NUMBER

%start stmts

%type <str> qstring
%type <str> NUMBER
%type <str> id
%type <str> fid

%%

stmts:
	stmt
	| stmts stmt
	;

stmt:
	ifdef id {
		char* key;
		char* value;
		if (g_hash_table_lookup_extended(defines,$2,&key,&value) && output==1)
			output=1;
		else
			output=0;
	}
	| ifndef id {
		char* key;
		char* value;
		if (!g_hash_table_lookup_extended(defines,$2,&key,&value) && output==1)
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
		char* value;
		if (output) {
			value=g_hash_table_lookup(defines,$1);
			printf("%s",value==NULL ? yylval.str : value);
		}
	}
	| fid {
		if (output) {
			printf("%s",yylval.str);
			if (in_record) {
				g_hash_table_insert(defines,$1,NULL);
			}
		}
	}
	| qstring {
		if (output) printf("%s",yylval.str);
	}
	| NUMBER {
		if (output) printf("%s",yylval.str);
	}
	| record
	| record id {
		if (output) {
			printf("%s",yylval.str);
			g_hash_table_insert(defines,$2,NULL);
		}
	}
	| define id {
		g_hash_table_insert(defines,$2,NULL);
	}
	| define id comma qstring {
		g_hash_table_insert(defines,$2,$4);
	}
	| define id comma NUMBER {
		g_hash_table_insert(defines,$2,$4);
	}
	| common
	| comma {
		if (output) printf(",");
	}
	| proc
	| subroutine
	| title
	| title qstring
	;

%%

