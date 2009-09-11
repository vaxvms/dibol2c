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

char* quote(char* str)
{
	char* quotestr;
	char* oldquotestr;
	char* oldstr=str;
	int quotelength=1;
	int i=0;
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
		specialstate=0;
		return(sequential);
		}

<OPENSUBMODE>"I"	{
		specialstate=0;
		return(indexed);
		}

<OPENSUBMODE>"P"	{
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

<STMTS>"%"		{
		return(percent);
		}

<*>\'[^\']*\'	{
		int c = input();
		if (c != '\'') {
			yylval.str=quote(yytext+1);
			yylval.str[yyleng-2] = '\0';
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
			yylval.str=quote(yytext+1);
			yylval.str[yyleng-2] = '\0';
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
		return(accept);
		}

<STMTS>"begin"		{
		return(begin);
		}

<STMTS>"end"		{
		return(end);
		}

<STMTS>"call"		{
		return(call);
		}

<STMTS>"clear"		{
		return(clearA);
		}

<STMTS>"close"		{
		return(close);
		}

<STMTS>"decr"		{
		return(decr);
		}

<STMTS>"delete"	{
		return(delete);
		}

<STMTS>"detach"	{
		return(detach);
		}

<STMTS>"display"	{
		return(display);
		}

<STMTS>"do"		{
		return(doA);
		}

<STMTS>"until"		{
		return(until);
		}

<STMTS>"exitloop"	{
		return(exitloop);
		}

<STMTS>"for"		{
		return(forA);
		}

<STMTS>"from"		{
		return(from);
		}

<STMTS>"thru"		{
		return(thru);
		}

<STMTS>"by"		{
		return(by);
		}

<STMTS>"forms"		{
		return(forms);
		}

<STMTS>"goto"		{
		return(gotoA);
		}

<STMTS>"go to"		{
		return(gotoA);
		}

<STMTS>"if"		{
		return(ifA);
		}

<STMTS>"then"		{
		return(then);
		}

<STMTS>"else"		{
		return(elseA);
		}

"external"	{
		return(external);
		}

"function"	{
		return(functionA);
		}

<STMTS>"incr"		{
		return(incr);
		}

<STMTS>"locase"	{
		return(locase);
		}

<STMTS>"lpque"		{
		return(lpque);
		}

<STMTS>"offerror"	{
		return(offerror);
		}

<STMTS>"off error"	{
		return(offerror);
		}

<STMTS>"onerror"	{
		return(onerror);
		}

<STMTS>"on error"	{
		return(onerror);
		}

<STMTS>"open"		{
		return(open);
		}

"proc"		{
		BEGIN STMTS;
		return(proc);
		}

<STMTS>"read"		{
		return(read);
		}

<STMTS>"reads"		{
		return(reads);
		}

<STMTS>"recv"		{
		return(recv);
		}

<STMTS>"return"	{
		return(returnA);
		}

<STMTS>"send"		{
		return(send);
		}

<STMTS>"sleep"		{
		return(sleep);
		}

<STMTS>"stop"		{
		return(stop);
		}

<STMTS>"store"		{
		return(store);
		}

<STMTS>"unlock"	{
		return(unlock);
		}

<STMTS>"upcase"	{
		return(upcase);
		}

<STMTS>"using"		{
		return(using);
		}

<STMTS>"select"	{
		return(selectA);
		}

<STMTS>"endusing"	{
		return(endusing);
		}

<STMTS>"while"		{
		return(whileA);
		}

<STMTS>"write"		{
		return(write);
		}

<STMTS>"writes"	{
		return(writes);
		}

<STMTS>"xcall"		{
		return(xcall);
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
