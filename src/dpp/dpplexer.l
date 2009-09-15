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

/* lexical scanner for DIBOL 83 pre processing */

%option case-insensitive
%option header-file="dpplexer.h"
/*%option debug*/

%{
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "dppparser.tab.h"

char* quote(char* str)
{
	char* quotestr;
	char* oldquotestr;
	char* oldstr=str;
	int quotelength=1;
	while (*str!='\0') {
		if (*str=='"') quotelength+=2;
		else quotelength++;
		str++;
	}
	quotestr=(char*)malloc(quotelength*sizeof(char));
	oldquotestr=quotestr;
	str=oldstr;
	do {
		if (*str=='"') {
			*quotestr='\\';
			quotestr++;
		}
		if (*str=='\n') {
			str+=2;
		}
		*quotestr=*str;
		str++;
		quotestr++;
	} while (*str!='\0');
	quotestr--;
	*quotestr='\0';
	return oldquotestr;
}

void trim(char *str) {
	char *begin=str;
	while (isspace(*str)) str++;
	while (*str) {
		*begin=*str;
		begin++;
		str++;
	}
	*begin='\0';
}

int output=1;
int ifdefcount=0;
int in_record=0;

%}

digit		[0-9]
integer		{digit}+
id		[a-zA-Z][a-zA-Z0-9$_]*
comments	";".*
comma		","

%%

".IFDEF"	{
		ifdefcount++;
		return(ifdef);
		}

".IFNDEF"	{
		ifdefcount++;
		return(ifndef);
		}

".ENDC"		{
		ifdefcount--;
		if (ifdefcount==0) output=1;
		return(endc);
		}

".INCLUDE"	{
		return(include);
		}

".LIST"		{
		/* .LIST statement is ignored */
		}

".NOLIST"	{
		/* .NOLIST statement is ignored */
		}

".PAGE"		{
		/* .PAGE statement is ignored */
		}

".TITLE"	{
		return(title);
		}

{comments}	{
		if (output) printf("%s",yytext);
		}

\'[^\']*\'	{
		int c = input();
		if (c != '\'') {
			yylval.str=strdup(yytext);
			unput(c);
			return qstring;
		} else {
			unput(c);
			yymore();
		}
		}

\"[^"]*\"	{
		int c = input();
		if (c!='"') {
			yylval.str=strdup(yytext);
			unput(c);
			return qstring;
		} else {
			unput(c);
			yymore();
		}
		}

"subroutine"	{
		if (output) printf("%s",yytext);
		in_record=0;
		return(subroutine);
		}

"proc"		{
		if (output) printf("%s",yytext);
		in_record=0;
		return(proc);
		}

"record"	{
		if (output) printf("%s",yytext);
		in_record=1;
		return(record);
		}

"common"	{
		if (output) printf("%s",yytext);
		in_record=1;
		return(common);
		}

{id}		{
		yylval.str=g_strdup(yytext);
		/* Put the identifier in uppercase */
		g_strup(yylval.str);
		return(id);
		}

^[ \t]*{id}	{
		yylval.str=g_strdup(yytext);
		trim(yylval.str);
		g_strup(yylval.str);
		return(fid);
		}

.		{
		if (output) printf("%s",yytext);
		}

<<EOF>>		{
		yypop_buffer_state();
     
		if ( !YY_CURRENT_BUFFER ) {
			yyterminate();
		}
		}
