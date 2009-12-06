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
#include "dppparser.h"

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

".DEFINE"	{
		return(define);
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

"\n&"	{
		/* Join splitted lines together. It's easier for the parser */
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

{comma}		{
		return(comma);
		}

{id}		{
		yylval.str=g_strdup(yytext);
		/* Put the identifier in uppercase */
		g_strup(yylval.str);
		return(id);
		}

{integer}	{
		yylval.str=g_strdup(yytext);
		return(NUMBER);
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

