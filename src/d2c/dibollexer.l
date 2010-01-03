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

/* lexical scanner for DIBOL 83 (and parts of 85) */

%option nounistd
%option stack
/* %option debug */
%option case-insensitive

%{
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "d2c.h"
#include "dibolparser.h"

#define SINGLEQUOTE '\''
#define DOUBLEQUOTE '"'

const char dec2octal[] = "01234567";

/* This function convert a dibol string to a C string :
 *  - double quote are escaped,
 *  - two following quotes are replaced by a single quote if there are of 
 *    the same type as surrounding quote,
 *  - non printable characters are replaced by their hexadecimal representation
 *  - backslash is escaped
 */
char* quote(char* str, char quote)
{
	char* quotestr;
	char* oldquotestr;
	char* oldstr=str;
	int quotelength=1;
	int i=0;
	/* Compute length of the resulting string */
	while (*str!='\0') {
		if (*str=='"' && quote==SINGLEQUOTE) quotelength+=2;
		else if (*str=='"' && *(str+1)=='"' && quote==DOUBLEQUOTE) {
			quotelength+=2;
			str++;
		}
		else if (*str=='\\') quotelength+=2;
		else if (*str < 32) quotelength+=4;
		else quotelength++;
		str++;
	}
	quotestr=(char*)malloc(quotelength*sizeof(char));

	/* Build the resulting string */
	oldquotestr=quotestr;
	str=oldstr;
	do {
		if (*str==DOUBLEQUOTE && quote==SINGLEQUOTE) {
			*quotestr='\\';
			quotestr++;
			*quotestr=*str;
			str++;
			quotestr++;
		}
		else if (*str=='"' && *(str+1)=='"' && quote==DOUBLEQUOTE) {
			*quotestr='\\';
			*quotestr++;
			str++;
			*quotestr=*str;
			str++;
			quotestr++;
		}
		else if (*str=='\'' && *(str+1)=='\'' && quote==SINGLEQUOTE) {
			str++;
			*quotestr=*str;
			str++;
			quotestr++;
		}
		else if (*str=='\\') {
			*quotestr='\\';
			quotestr++;
			*quotestr=*str;
			str++;
			quotestr++;
		}
		else if (*str < 32) {
			*quotestr='\\';
			quotestr++;
			*quotestr='0';
			quotestr++;
			*quotestr=dec2octal[(*str) / 8];
			quotestr++;
			*quotestr=dec2octal[(*str) % 8];
			quotestr++;
			str++;
		}
		else if (*str=='\n') {
			str+=2;
			*quotestr=*str;
			str++;
			quotestr++;
		}
		else {
			*quotestr=*str;
			str++;
			quotestr++;
		}
	} while (*str!='\0');
	*quotestr='\0';
	return oldquotestr;
}

%}

digit		[0-9]
integer		{digit}+
id		[a-zA-Z][a-zA-Z0-9$_]*
comments	";".*
comma		","

%x      OPENMODE
%x	OPENSUBMODE
%x	OPENOPTIONS

%x	LPQUEOPTIONS

%x	STMTS


%%


<OPENMODE>"O"	{
		return(output);
		}

<OPENMODE>"I"	{
		return(inputD);
		}

<OPENMODE>"U"	{
		return(update);
		}

<OPENMODE>"SI"	{
		return(inputindexed);
		}

<OPENMODE>"SU"	{
		return(updateindexed);
		}

<OPENMODE>"A"	{
		return(append);
		}

<OPENMODE>":"	{
		BEGIN OPENSUBMODE;
		return(colonD);
		}

<OPENMODE>","	{
		BEGIN STMTS;
		return comma;
		}

<OPENSUBMODE>"S"	{
		BEGIN STMTS;
		specialstate=0;
		return(sequential);
		}

<OPENSUBMODE>"I"	{
		BEGIN STMTS;
		specialstate=0;
		return(indexed);
		}

<OPENSUBMODE>"P"	{
		BEGIN STMTS;
		specialstate=0;
		return(print);
		}

<OPENSUBMODE>"R"	{
		BEGIN STMTS;
		specialstate=0;
		return(relative);
		}

<OPENOPTIONS>"ALLOC"	{
		return(openalloc);
		}

<OPENOPTIONS>"BKTSIZ"	{
		return(openbktsiz);
		}

<OPENOPTIONS>"BLKSIZ"	{
		return(openblksiz);
		}

<OPENOPTIONS>"BUFSIZ"	{
		return(openbufsiz);
		}

<OPENOPTIONS>"RECSIZ"	{
		return(openrecsiz);
		}

<OPENOPTIONS>":"	{
		BEGIN STMTS;
		specialstate=0;
		return(colonD);
		}

<LPQUEOPTIONS>"LPNUM"	{
		return(lpquelpnum);
		}

<LPQUEOPTIONS>"COPIES"	{
		return(lpquecopies);
		}

<LPQUEOPTIONS>"FORM"	{
		return(lpqueform);
		}

<LPQUEOPTIONS>"DELETE"	{
		return(lpquedelete);
		}

<LPQUEOPTIONS>":"	{
		specialstate=0;
		return(colonD);
		}

<*>{comments}	{ /* We ignore the comments */ }

<*>[ \t]		{ }

<*>"\n&"		{
		/* When you get \n and &, the instruction continue on the
		   next line. So we don't return anything */
		lineno++;
		}

"\n"		{
		lineno++;
		return(newline);
		}

<STMTS>"\n"		{
		lineno++;
		return(newline);
		}

<*>{integer}	{
		yylval.str=g_strdup(yytext);
		return(NUMBER);
		}

{comma}		{
		return(comma);
		}

<STMTS>{comma}		{
		if (specialstate==OPENMODESTATE)
			BEGIN OPENMODE;
		if (specialstate==OPENOPTIONSTATE)
			BEGIN OPENOPTIONS;
		if (specialstate==LPQUEOPTIONSTATE)
			BEGIN LPQUEOPTIONS;
		return(comma);
		}

<STMTS>"("		{
		return(openparenth);
		}

<STMTS>")"		{
		return(closeparenth);
		}

<STMTS>"["	{
		return(openbracket);
		} 

<STMTS>"]"	{
		return(closebracket);
		}

<STMTS>"%"		{
		return(percent);
		}

<*>\'[^\']*\'	{
		int c = input();
		if (c != '\'') {
			//yylval.str[yyleng-1] = '\0';
			yytext[yyleng-1]='\0';
			yylval.str=quote(yytext+1,SINGLEQUOTE);
			//yylval.str[yyleng-2] = '\0';
			unput(c);
			return qstring;
		} else {
			unput(c);
			yymore();
		}
		}

<*>\"[^"]*\"	{
		int c = input();
		if (c!='"') {
			yytext[yyleng-1]='\0';
			yylval.str=quote(yytext+1,DOUBLEQUOTE);
			//yylval.str[yyleng-2] = '\0';
			unput(c);
			return qstring;
		} else {
			unput(c);
			yymore();
		}
		}

<STMTS>"="		{
		return(equal);
		}

"subroutine"	{
		return(subroutine);
		}

"record"	{
		return(record);
		}

"common"	{
		return(common);
		}

"group"		{
		return(group);
		}

"endgroup"	{
		return(endgroup);
		}

<STMTS>"+"		{
		return(plus);
		}

<STMTS>"-"		{
		return(minus);
		}

<STMTS>"#"		{
		return(rounding);
		}

<STMTS>"*"		{
		return(multiply);
		}

<STMTS>"/"		{
		return(divide);
		}

<STMTS>".eq."		{
		return(eq);
		}

<STMTS>".ne."		{
		return(ne);
		}

<STMTS>".gt."		{
		return(gt);
		}

<STMTS>".lt."		{
		return(lt);
		}

<STMTS>".ge."		{
		return(ge);
		}

<STMTS>".le."		{
		return(le);
		}

<STMTS>".not."		{
		return(not);
		}

<STMTS>".and."		{
		return(and);
		}

<STMTS>".or."		{
		return(or);
		}

<STMTS>".xor."		{
		return(xor);
		}

<STMTS>"accept"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(accept);
		}

<STMTS>"begin"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(begin);
		}

<STMTS>"end"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(end);
		}

<STMTS>"call"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(call);
		}

<STMTS>"clear"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(clearA);
		}

<STMTS>"close"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(close);
		}

<STMTS>"decr"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(decr);
		}

<STMTS>"delete"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(delete);
		}

<STMTS>"detach"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(detach);
		}

<STMTS>"display"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(display);
		}

<STMTS>"do"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(doA);
		}

<STMTS>"until"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(until);
		}

<STMTS>"exitloop"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(exitloop);
		}

<STMTS>"for"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(forA);
		}

<STMTS>"from"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(from);
		}

<STMTS>"thru"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(thru);
		}

<STMTS>"by"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(by);
		}

<STMTS>"forms"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(forms);
		}

<STMTS>"goto"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(gotoA);
		}

<STMTS>"go to"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(gotoA);
		}

<STMTS>"if"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(ifA);
		}

<STMTS>"then"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(then);
		}

<STMTS>"else"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(elseA);
		}

"external"	{
		return(external);
		}

"function"	{
		return(functionA);
		}

<STMTS>"incr"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(incr);
		}

<STMTS>"locase"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(locase);
		}

<STMTS>"lpque"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(lpque);
		}

<STMTS>"nextloop"	{
		return(nextloop);
		}

<STMTS>"offerror"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(offerror);
		}

<STMTS>"off error"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(offerror);
		}

<STMTS>"onerror"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(onerror);
		}

<STMTS>"on error"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(onerror);
		}

<STMTS>"open"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(open);
		}

"proc"		{
		BEGIN STMTS;
		return(proc);
		}

<STMTS>"read"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(read);
		}

<STMTS>"reads"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(reads);
		}

<STMTS>"recv"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(recv);
		}

<STMTS>"return"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(returnA);
		}

<STMTS>"send"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(send);
		}

<STMTS>"sleep"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(sleep);
		}

<STMTS>"stop"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(stop);
		}

<STMTS>"store"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(store);
		}

<STMTS>"unlock"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(unlock);
		}

<STMTS>"upcase"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(upcase);
		}

<STMTS>"using"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(using);
		}

<STMTS>"select"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(selectA);
		}

<STMTS>"endusing"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(endusing);
		}

<STMTS>"while"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(whileA);
		}

<STMTS>"write"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(write);
		}

<STMTS>"writes"	{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(writes);
		}

<STMTS>"xcall"		{
		yylval.str=g_strdup(yytext);
		g_strup(yylval.str);
		return(xcall);
		}

<STMTS>"."	{
		return(dot);
		}

<*>{id}		{
		yylval.str=g_strdup(yytext);
		/* Put the identifier in uppercase */
		g_strup(yylval.str);
		return(id);
		}

.		{
		//fprintf(stderr,"Unrecognized character: \"%s\"\n", yytext );
		}
