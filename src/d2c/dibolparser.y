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

/* Grammar file for DIBOL language. */

%{

#define YYDEBUG 1

#include <stdio.h>
#include "d2c.h"
#include <glib.h>
#include <stdlib.h>
#include "symbols.h"
#include <ctype.h>
#include <string.h>
#include "gencode.h"
#include "stack.h"


GSList* vardecl= NULL;
GSList* commondecl= NULL;
GSList* commoninit= NULL;
char* prototype;
int specialstate=0;
Symbols symbols;
int nbparams=0;
int nbusing=0;
Stack* usingstack=NULL;

%}

%union {
	long long num;
	char *str;
	GSList* code;
	FieldDef* fielddef;
	GSList* fieldsdef;
	GSList* ids;
}


%token <str> NUMBER
%token <str> equal
%token <str> id
%token <str> comma
%token <str> record
%token <str> common
%token <str> qstring
%token <str> openparenth
%token <str> closeparenth
%token <str> openbracket
%token <str> closebracket
%token <str> percent
%token <str> newline
%token <str> subroutine
%token <str> rounding
%token <str> not
%token <str> xor
%token <str> accept
%token <str> begin
%token <str> end
%token <str> call
%token <str> clearA
%token <str> close
%token <str> decr
%token <str> delete
%token <str> detach
%token <str> display
%token <str> doA
%token <str> until
%token <str> external
%token <str> functionA
%token <str> exitloop
%token <str> forA
%token <str> from
%token <str> thru
%token <str> forms
%token <str> gotoA
%token <str> ifA
%token <str> then
%token <str> elseA
%token <str> incr
%token <str> locase
%token <str> lpque
%token <str> offerror
%token <str> onerror
%token <str> open
%token <str> proc
%token <str> read
%token <str> reads
%token <str> recv
%token <str> returnA
%token <str> send
%token <str> sleep
%token <str> stop
%token <str> store
%token <str> unlock
%token <str> upcase
%token <str> using
%token <str> selectA
%token <str> endusing
%token <str> whileA
%token <str> write
%token <str> writes
%token <str> xcall

%token <str> inputD
%token <str> output
%token <str> update
%token <str> append
%token <str> sequential
%token <str> print
%token <str> relative
%token <str> indexed
%token <str> inputindexed
%token <str> updateindexed
%token <str> openalloc
%token <code> openbktsiz
%token <code> openblksiz
%token <code> openbufsiz
%token <code> openrecsiz
%token <str> colonD
%token <code> lpquelpnum
%token <code> lpquecopies
%token <code> lpqueform
%token <code> lpquedelete
%token <str> by


%left or xor
%left and
%left not
%left multiply divide
%left minus plus
%left eq ne lt gt ge le
%left openparenthesis closeparenthesis
%nonassoc UMINUS
%nonassoc elseA
%nonassoc then

%start prog

%type <code> labelA
%type <code> stmt
%type <code> stmts
%type <code> stmtrecord
%type <code> stmtcommon
%type <code> compexpr
%type <code> expr
%type <code> stmtaffect
%type <code> parameter
%type <code> parameters
%type <code> stmtaccept
%type <code> stmtcall
%type <code> stmtclear
%type <code> stmtclose
%type <code> stmtdecr
%type <code> stmtdelete
%type <code> stmtdetach
%type <code> stmtdisplay
%type <code> stmtdo
%type <code> stmtexitloop
%type <code> stmtfor
%type <code> stmtforms
%type <code> stmtgoto
%type <code> stmtif
%type <code> stmtincr
%type <code> stmtlocase
%type <code> stmtlpque
%type <code> stmtofferror
%type <code> stmtonerror
%type <code> stmtopen
%type <code> stmtproc
%type <code> stmtread
%type <code> stmtreads
%type <code> stmtrecv
%type <code> stmtreturn
%type <code> stmtsend
%type <code> stmtsleep
%type <code> stmtstop
%type <code> stmtstore
%type <code> stmtunlock
%type <code> stmtupcase
%type <code> stmtwhile
%type <code> stmtwrite
%type <code> stmtwrites
%type <code> stmtxcall
%type <code> stmtexternal
%type <code> stmtnewline
%type <code> begin_end
%type <code> defrecord
//%type <str> common
%type <fieldsdef> fieldsdefinition
%type <fielddef> fielddefinition
%type <fieldsdef> subroutineargs
%type <fielddef> subroutinearg
%type <str> fieldtype
//%type <str> subroutine
%type <code> openallocopt
%type <code> openbktsizopt
%type <code> openblksizopt
%type <code> openbufsizopt
%type <code> openrecsizopt
%type <str> openmainmode
%type <str> opensubmode
%type <str> openmode
%type <code> openoption
%type <code> openoptions
%type <code> lpnumopt
%type <code> copiesopt
%type <code> formopt
%type <code> deleteopt
%type <code> lpqueoptions
%type <str> fieldvalue
%type <str> fieldvalues
%type <str> iobuf
%type <str> optnewline
%type <str> externaldef
%type <str> externaldefs
%type <ids> ids
%type <code> stmtusing
%type <code> choices
%type <code> choice
%type <code> mexps
%type <code> mexp
%type <str> ident
%type <code> datadivision
%type <code> datasdivision

%%

prog:
	stmtnewlines subroutine id newline stmtnewlines stmtproc {
		char* headername;
		FILE* headerfile;
		prototype=g_strconcat("void p",$3," ()",NULL);
		GSList * code=gencode_function(prototype,vardecl,commondecl,commoninit,$6);
		/* When we have parsed a procedure, nothing else matter
		as there is only one proc per file with dibol */
		while (code!=NULL) {
			fprintf(csrc,"%s\n",(char*)code->data);
			//free(code->data);
			code=g_slist_next(code);
		}
		g_slist_free(code);
	}
	| stmtnewlines subroutine id newline stmtnewlines datasdivision stmtproc {
		char* headername;
		FILE* headerfile;
		prototype=g_strconcat("void p",$3," ()",NULL);
		GSList * code=gencode_function(prototype,vardecl,commondecl,commoninit,$7);
		/* When we have parsed a procedure, nothing else matter
		as there is only one proc per file with dibol */
		while (code!=NULL) {
			fprintf(csrc,"%s\n",(char*)code->data);
			//free(code->data);
			code=g_slist_next(code);
		}
		g_slist_free(code);
	}
	| stmtnewlines subroutine id newline stmtnewlines subroutineargs datasdivision stmtproc {
		char* params=NULL;
		GSList* tmp;
		GSList* fieldslist;
		char* headername;
		FILE* headerfile;
		fieldslist=g_slist_reverse($6);
		tmp=fieldslist;
		while (tmp != NULL) {
			if (params==NULL)
				params=g_strdup("");
			else
				params=g_strconcat(params,",",NULL);
			asprintf(&params,"%s variable * v%s",params,(*(FieldDef*)(tmp->data)).name);
			addSymbol(&symbols,createParameter((*(FieldDef*)(tmp->data)).name));
			tmp = g_slist_next(tmp);
		}
		g_slist_free(fieldslist);
		asprintf(&prototype,"void p%s (%s)",$3,params);
		/* When we have parsed a procedure, nothing else matter
		as there is only one proc per file with dibol */
		GSList * code=gencode_function(prototype,vardecl,commondecl,commoninit,$8);
		while (code!=NULL) {
			fprintf(csrc,"%s\n",(char*)code->data);
			//free(code->data);
			code=g_slist_next(code);
		}
		g_slist_free(code);
		}
	| stmtnewlines datasdivision stmtproc {
		/* When we have parsed a procedure, nothing else matter
		as there is only one proc per file with dibol */
		GSList * code=gencode_function(prototype,vardecl,commondecl,commoninit,$3);
		while (code!=NULL) {
			fprintf(csrc,"%s\n",(char*)code->data);
			//free(code->data);
			code=g_slist_next(code);
		}
		g_slist_free(code);
		}
	| stmtnewlines stmtproc {
		GSList * code=gencode_function(prototype,vardecl,commondecl,commoninit,$2);
		while (code!=NULL) {
			fprintf(csrc,"%s\n",(char*)code->data);
			//free(code->data);
			code=g_slist_next(code);
		}
		g_slist_free(code);
	}
	
stmtnewlines:
	stmtnewline
	| stmtnewline stmtnewlines
	| ;

datasdivision:
	datadivision
	| datasdivision datadivision

datadivision:
	stmtrecord {
		vardecl=g_slist_concat(vardecl,$1);
	}
	| stmtcommon {
		commondecl=g_slist_concat(commondecl,$1);
		}
	;

stmts:
	stmt {
		$$ = $1;
		}
	| stmts stmt {
		$$ = g_slist_concat($1,$2);
		}
	;

stmt:
	labelA
	| stmtaffect
	| stmtaccept
	| stmtcall
	| stmtclear
	| stmtclose
	| stmtdecr
	| stmtdelete
	| stmtdetach
	| stmtdisplay
	| stmtdo
	| stmtfor
	| stmtforms
	| stmtgoto
	| stmtif
	| stmtincr
	| stmtlocase
	| stmtlpque
	| stmtofferror
	| stmtonerror
	| stmtopen
	| stmtread
	| stmtreads
	| stmtrecv
	| stmtreturn
	| stmtsend
	| stmtsleep
	| stmtstop
	| stmtstore
	| stmtunlock
	| stmtupcase
	| stmtusing
	| stmtwhile
	| stmtwrite
	| stmtwrites
	| stmtxcall
	| stmtexternal
	| stmtexitloop
	| stmtnewline
	| begin_end
	;

labelA:
	ident comma {
		$$= g_slist_append(NULL,g_strconcat("l",$1,": ;",NULL));
		free($1);
		};

stmtnewline:
	newline { $$=txt2list(""); }

stmtrecord:
	record defrecord {
		$$ = $2;
		};

stmtcommon:
	common newline fieldsdefinition {
		commoninit=g_slist_concat(commoninit,gencode_common_init_def(NULL,false,$3));
		$$=gencode_common_def(NULL,false,$3);
	}
	| common id newline fieldsdefinition {
		commoninit=g_slist_concat(commoninit,gencode_common_init_def($2,false,$4));
		$$=gencode_common_def($2,false,$4);
	}
	| common comma id newline fieldsdefinition {
		commoninit=g_slist_concat(commoninit,gencode_common_init_def(NULL,true,$5));
		$$=gencode_common_def(NULL,true,$5);
	}
	| common id comma id newline fieldsdefinition {
		commoninit=g_slist_concat(commoninit,gencode_common_init_def($2,true,$6));
		$$=gencode_common_def($2,true,$6);
	};

defrecord:
	newline fieldsdefinition {
		$$=gencode_fields_def(NULL,false,$2);
	}
	| id newline fieldsdefinition {
		$$=gencode_fields_def($1,false,$3);
	}
	| comma id newline fieldsdefinition {
		$$=gencode_fields_def(NULL,true,$4);
	}
	| id comma id newline fieldsdefinition {
		$$=gencode_fields_def($1,true,$5);
	};

fieldsdefinition:
	fielddefinition { if ($1!=NULL) $$ = g_slist_prepend(NULL,$1); }
	| fieldsdefinition fielddefinition {
		if ($2!=NULL) $$ = g_slist_prepend($1,$2);
	};

fieldtype:
	id { $$=$1; }
	| NUMBER id { $$=$2; }
	| NUMBER id NUMBER { $$=$2; };

fieldvalues:
	fieldvalue
	| fieldvalues comma fieldvalue {
	};

fieldvalue:
	NUMBER
	| minus NUMBER { }
	| qstring {
		}

fielddefinition:
	id comma fieldtype comma fieldvalues newline {
		$$=newFielddef($1,$3,$5);
		}
	| id comma fieldtype newline {
		$$=newFielddef($1,$3,NULL);
		}
	| comma fieldtype comma fieldvalues newline {
		$$=newFielddef(NULL,$3,$5);
		}
	| comma fieldtype newline {
		$$=newFielddef(NULL,$2,NULL);
		}
	| newline { $$ = NULL; }
	;

iobuf:
	{ $$ = g_strdup(" "); }
	| openparenth NUMBER closeparenth {
		$$ = g_strdup(" ");
		}

/*stmtfunction:
	functionA id newline*/

stmtproc:
	proc iobuf newline stmts {
		$$ = $4;
		}
	| proc iobuf newline stmts end stmtnewlines {
		$$ = $4;
		};

subroutineargs:
	subroutinearg { if ($1!=NULL) $$ = g_slist_prepend(NULL,$1); }
	/*| newline subroutinearg*/
	| subroutineargs subroutinearg {
		if ($2!=NULL) $$ = g_slist_prepend($1,$2);
	};

subroutinearg:
	ident comma id newline {
		$$=newFielddef($1,$3,NULL);
	}
	| newline { $$ = NULL; }

stmtaffect:
	expr equal expr newline {
		$$ = g_slist_multiconcat($1,$3,txt2list("assign(pop(&stack),pop(&stack));"),NULL);
	}
	| expr equal newline {
		$$ = g_slist_concat($1,txt2list("dbl_clear(pop(&stack));"));
	}
	| expr equal expr comma expr newline {
		$$ = g_slist_multiconcat($1,$3,$5,
			txt2list("push(&stack,formatD(pop(&stack),pop(&stack)));"),
			txt2list("assign(pop(&stack),pop(&stack));"),NULL);
	};

compexpr:
	eq expr {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,eq(var1,var2));"),
			NULL);
		}
	| ne expr {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,ne(var1,var2));"),
			NULL);
		}
	| ge expr {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,ge(var1,var2));"),
			NULL);
		}
	| le expr {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,le(var1,var2));"),
			NULL);
		}
	| gt expr {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,gt(var1,var2));"),
			NULL);
		}
	| lt expr {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,lt(var1,var2));"),
			NULL);
		};

expr:
	expr plus expr {
		$$=g_slist_multiconcat(
			$1,
			$3,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,add(var1,var2));"),NULL);
	}
	| expr minus expr {
		GSList* code;
		code=g_slist_concat($1,$3);
		code=g_slist_concat(code,txt2list("var2=pop(&stack);"));
		code=g_slist_concat(code,txt2list("var1=pop(&stack);"));
		code=g_slist_concat(code,txt2list("push(&stack,sub(var1,var2));"));
		$$ = code;
	}
	| expr multiply expr {
		$$ = g_slist_multiconcat(
			$1,
			$3,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,mul(var1,var2));"),
			NULL);
		}
	| expr divide expr {
		$$ = g_slist_multiconcat(
			$1,
			$3,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,divD(var1,var2));"),
			NULL);
		}
	| expr compexpr {
		$$ = g_slist_concat($1,$2);
		}
	| expr and expr {
		$$ = g_slist_multiconcat(
			$1,
			$3,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,and(var1,var2));"),
			NULL);
		}
	| expr or expr {
		$$ = g_slist_multiconcat(
			$1,
			$3,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,or(var1,var2));"),
			NULL);
		}
	| openparenth expr closeparenth {
			$$ = $2;
		}
	| expr rounding expr {
		$$ = g_slist_multiconcat(
			$1,
			$3,
			txt2list("var2=pop(&stack);"),
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,rounding(var1,var2));"),
			NULL);
	}
	| minus expr %prec UMINUS {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,minus(var1));"),
			NULL);
		}
	| not expr {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("var1=pop(&stack);"),
			txt2list("push(&stack,not(var1));"),
			NULL);
		}
	| percent id openparenth parameters closeparenth {
		$$ = txt2list("// Not implemented yet.");
		}
	| expr openbracket expr closebracket {
		$$ = txt2list("// Not implemented yet.");
	}
	| NUMBER {
		char* tmp;
		asprintf(&tmp,"push(&stack,imm(tDECIMAL,%d,\"%s\"));",strlen($1),$1);
		$$ = g_slist_append(NULL,tmp);
		}
	| ident {
	//| id {
		//if (findSymbol(&symbols,$1,PARAMETER)!=0) 
		$$ = g_slist_append(NULL,g_strconcat("push(&stack,v",$1,");",NULL));
		}
	| qstring {
		char* tmp=NULL;
		asprintf(&tmp,"push(&stack,imm(tALPHA,%d,\"%s\"));",strlen($1),$1);
		$$ = g_slist_append(NULL,tmp);
		}
	| expr openparenth expr closeparenth {
		char* tmp=g_strconcat("push(&stack,indexvar(vh,v",$1,",decimal2integer(pop(&stack))));",NULL);
		$$ = g_slist_concat($3,g_slist_append(NULL,tmp));
		}
	| expr openparenth expr comma expr closeparenth {
		$$ = g_slist_multiconcat(
			$1,
			$3,
			$5,
			txt2list("push(&stack,substring(pop(&stack),pop(&stack),pop(&stack)));"),
			NULL);
		};

parameter:
	expr { $$ = $1; }
	;

parameters:
	parameter {
			nbparams=1;
			$$ = $1;
		}
	| parameter comma parameters {
			nbparams++;
			$$ = g_slist_concat($1,$3);
		}
	;

stmtaccept:
	accept openparenth parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			txt2list("dbl_accept(pop(&stack),pop(&stack));"),
			NULL);
		}
	| accept openparenth parameter comma parameter comma id closeparenth newline {
		char* tmp=g_strconcat("goto l",$7,";",NULL);
		$$ = g_slist_multiconcat(
			$3,
			$5,
			txt2list("if (dbl_accept(pop(&stack),pop(&stack)))"),
			g_slist_append(NULL,tmp),
			NULL);
		};

begin_end:
	begin stmts end newline {
		$$=g_slist_multiconcat(
			txt2list("{"),
			$2,
			txt2list("}"),
			NULL);
		}
	;

stmtcall:
	call ident newline {
		$$ = g_slist_append(NULL,g_strconcat("CALL(l",$2,");",NULL));
		};

stmtclear:
	clearA parameters newline {
		GSList *code = $2;
		int i;
		for (i=0;i<nbparams;i++)
			code = g_slist_concat(code,txt2list("dbl_clear(pop(&stack));"));
		$$ = code;
		};

stmtclose:
	close parameter newline {
		$$ = g_slist_concat($2,txt2list("dbl_close(pop(&stack));"));
		};

stmtdecr:
	decr expr newline {
		$$ = g_slist_concat($2,txt2list("dbl_decr(pop(&stack));"));
		};


stmtdelete:
	delete openparenth parameter closeparenth newline {
		$$ = g_slist_concat($3,txt2list("dbl_delete(pop(&stack),NULL);"));
		}
	| delete openparenth parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			txt2list("dbl_delete(pop(&stack),pop(&stack));"),
			NULL);
		};

stmtdetach:
	detach newline {
		$$ = txt2list("/* DETACH is useless in multi-tasking environnement */");
		};

stmtdisplay:
	display openparenth parameter comma parameters closeparenth newline {
		GSList* tmp=$5;
		char* instruct;
		char* params=NULL;
		char* paramscalc=NULL;
		int i=0;
		for (i=0;i<nbparams;i++)
			params=g_strconcat(",pop(&stack)",params,NULL);
		asprintf(&instruct,"dbl_display(pop(&stack),%d%s);",nbparams,params);
		$$ = g_slist_multiconcat($3,$5,txt2list(instruct),NULL);
		free(instruct);
	};

optnewline:
	{ }
	| newline { }

stmtdo:
	doA optnewline stmt optnewline until expr {
		$$ = g_slist_multiconcat(
			txt2list("do {"),
			$3,
			$6,
			txt2list("} while ( ! isTrue(pop(&stack)));"),
			NULL);
		};

stmtfor:
	forA id from expr thru expr optnewline stmt {
		$$=gencode_for($4,$6,NULL,$8,$2);
		}
	| forA id from expr thru expr by expr optnewline stmt {
		$$=gencode_for($4,$6,$8,$10,$2);
		};

stmtforms:
	forms openparenth parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			txt2list("dbl_forms(pop(&stack),pop(&stack));")
			,NULL);
		}
	;

ident:
	id | ifA | then | elseA | call | display | clearA | read | open | delete | begin | end | selectA | send; 

ids:
	ident {
		$$ = g_slist_append(NULL,$1);
		}
	|
	ident comma ids {
		$$=g_slist_append($3,$1);
		};

stmtgoto:
	gotoA ident newline {
			$$ = g_slist_append(NULL,g_strconcat("goto l",$2,";",NULL));
		}
	| gotoA openparenth ids closeparenth comma expr newline {
		char* newcase;
		int i=1;
		GSList* tmp = $3;
		GSList* code=NULL;
		code=g_slist_prepend(code,g_strdup("switch (decimal2integer(pop(&stack))) {"));
		while (tmp!=NULL) {
			asprintf(&newcase,"case %d : goto l%s;\n",i,tmp->data);
			code = g_slist_prepend(code,newcase);
			tmp = g_slist_next(tmp);
			i++;
		}
		code=g_slist_prepend(code,g_strdup("}"));
		code=g_slist_reverse(code);
		code=g_slist_multiconcat($6,code,NULL);
		$$=code;
		}
	;

stmtif:
	ifA expr optnewline stmt {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("if (isTrue(pop(&stack))) {"),
			$4,
			txt2list("}"),
			NULL);
		}
	| ifA expr optnewline then optnewline stmt optnewline elseA optnewline stmt {
		$$ = g_slist_multiconcat(
			$2,
			txt2list("if (isTrue(pop(&stack))) {"),
			$6,
			txt2list("} else {"),
			$10,
			txt2list("}"),
			NULL);
		}
	;

stmtincr:
	incr expr newline {
		$$ = g_slist_concat($2,txt2list("dbl_incr(pop(&stack));"));
		};

stmtlocase:
	locase expr newline {
		$$ = g_slist_concat($2,txt2list("dbl_locase(pop(&stack));"));
		};

lpnumopt:
	comma lpquelpnum colonD {specialstate=LPQUEOPTIONSTATE;} expr { $$=$5; }
	| { $$ = txt2list("push(&stack,NULL);"); specialstate=0; };

copiesopt:
	comma lpquecopies colonD {specialstate=LPQUEOPTIONSTATE;} expr { $$=$5; }
	| { $$ = txt2list("push(&stack,NULL);"); specialstate=0; };

formopt:
	comma lpqueform colonD {specialstate=LPQUEOPTIONSTATE;} expr { $$=$5; }
	| { $$ = txt2list("push(&stack,NULL);"); specialstate=0; };

deleteopt:
	comma lpquedelete {$$ = txt2list("push(&stack,1);"); }
	| { $$ = txt2list("push(&stack,NULL);"); specialstate=0; };

lpqueoptions:
	lpnumopt copiesopt formopt deleteopt {
		$$ = g_slist_multiconcat($1,$2,$3,$4,NULL);
		};

stmtlpque:
	lpque {specialstate=LPQUEOPTIONSTATE;} openparenth parameter lpqueoptions closeparenth newline {
		$$ = g_slist_multiconcat($4,$5,txt2list("dbl_lpque(pop(&stack),pop(&stack),pop(&stack),pop(&stack),pop(&stack));"),NULL);
		};

stmtofferror:
	offerror newline {
		$$ = txt2list("OFFERROR;");
		};

stmtonerror:
	onerror id newline {
		$$ = g_slist_append(NULL,g_strconcat("ONERROR(l",$2,");",NULL));
		};

openallocopt:
	comma openalloc colonD { specialstate=OPENOPTIONSTATE; } expr { $$ = $5; }

openbktsizopt:
	comma openbktsiz colonD { specialstate=OPENOPTIONSTATE; } expr { $$ = $5; }

openblksizopt:
	comma openblksiz colonD { specialstate=OPENOPTIONSTATE; } expr { $$ = $5; }

openbufsizopt:
	comma openbufsiz colonD { specialstate=OPENOPTIONSTATE; } expr { $$ = $5; }

openrecsizopt:
	comma openrecsiz colonD expr { $$ = $4; }

openoption:
	openallocopt
	| openbktsizopt
	| openblksizopt
	| openbufsizopt
	| openrecsizopt
	| { $$ = txt2list("push(&stack,NULL);"); specialstate=0; }

openoptions:
	openoption openoption openoption openoption openoption { 
		$$ = g_slist_multiconcat($1,$2,$3,$4,$5,NULL);
		};

stmtopen:
	open openparenth { specialstate=OPENMODESTATE; } parameter comma openmode comma { specialstate=OPENOPTIONSTATE; } parameter openoptions closeparenth newline {
		$$ = g_slist_multiconcat($4,$9,$10,
			g_slist_append(NULL,g_strconcat("dbl_open(pop(&stack),",$6,",pop(&stack),pop(&stack),pop(&stack),pop(&stack),pop(&stack),pop(&stack));",NULL)),
			NULL);
		};

stmtread:
	read openparenth parameter comma parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			$7,
			txt2list("dbl_read(pop(&stack),pop(&stack),pop(&stack));"),
			NULL);
		};

stmtreads:
	reads openparenth parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			txt2list("dbl_reads(pop(&stack),pop(&stack));"),
			NULL);
		}
	| reads openparenth parameter comma parameter comma id closeparenth newline {
		$$ = g_slist_multiconcat($3,$5,
			g_slist_prepend(NULL,g_strconcat("if (dbl_reads(pop(&stack),pop(&stack))) goto l",$7,";",NULL)),
			NULL);
		};

stmtrecv:
	recv openparenth parameter comma id closeparenth newline {
		char* tmp1=g_strconcat("if (dbl_recv(pop(&stack),NULL)) goto l",$5,";",NULL);
		$$ = g_slist_concat(
			$3,
			g_slist_append(NULL,tmp1));
		}
	| recv openparenth parameter comma id comma parameter closeparenth newline {
		char* tmp1=g_strconcat("if (dbl_recv(pop(&stack),pop(&stack))) goto l",$5,";",NULL);
		$$ = g_slist_multiconcat(
			$3,
			$7,
			g_slist_append(NULL,tmp1),
			NULL);
		};

stmtreturn:
	returnA newline {
		$$ = txt2list("RETURN;");
		}

stmtsend:
	send openparenth parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			txt2list("dbl_send(pop(&stack),pop(&stack),NULL);"),
			NULL);
		}
	| send openparenth parameter comma parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			$7,
			txt2list("dbl_send(pop(&stack),pop(&stack),pop(&stack));"),
			NULL);
		};

stmtsleep:
	sleep parameter newline {
		$$ = g_slist_concat($2,txt2list("dbl_sleep(pop(&stack));"));
		};

stmtstop:
	stop parameter newline {
		$$ = g_slist_concat($2,txt2list("dbl_stop(pop(&stack));"));
		}
	| stop newline {
		$$ = txt2list("dbl_stop(NULL);");
		};

stmtstore:
	store openparenth parameter comma parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			$7,
			txt2list("dbl_store(pop(&stack),pop(&stack),pop(&stack));"),
			NULL);
		};

stmtunlock:
	unlock parameter newline {
		$$ = g_slist_concat($2,txt2list("dbl_unlock(pop(&stack));"));
		};

stmtupcase:
	upcase parameter newline {
		$$ = g_slist_concat($2,txt2list("dbl_upcase(pop(&stack));"));
		};

mexp:
	compexpr { $$ = g_slist_concat($1,txt2list("push(&stack,eq(pop(&stack),var1));")); }
	| expr { $$ = g_slist_concat($1,txt2list("push(&stack,eq(pop(&stack),var1));")); }
	| expr thru expr {
		$$ = g_slist_multiconcat($1,
			txt2list("push(&stack,ge(var1,pop(&stack)));"),
			$3,
			txt2list("push(&stack,le(var1,pop(&stack)));"),
			txt2list("push(&stack,and(pop(&stack),pop(&stack)));"),
			NULL);
	}

mexps:
	mexp { $$ = $1; }
	| mexps comma mexp {
		$$ = g_slist_multiconcat($1,
			$3,
			txt2list("push(&stack,or(pop(&stack),pop(&stack)));"),
			NULL);
	}

choice:
	openparenth mexps closeparenth comma optnewline stmt optnewline {
		char* tmp;
		asprintf(&tmp,"goto ENDUSING%d;",top(&usingstack));
		$$ = g_slist_multiconcat($2,
			txt2list("if (isTrue(pop(&stack))) {"),
			$6,
			g_slist_append(NULL,tmp),
			txt2list("}"),
			NULL);
		}
	| openparenth closeparenth comma optnewline stmt optnewline {
			$$ = $5;
		}
	| newline {
		$$ = txt2list("\n");
	}

choices:
	choice { $$ = $1; }
	| choices choice {
		$$ = g_slist_concat($1,$2);
		}

stmtusing:
	using { nbusing++; push(&usingstack,nbusing); } expr selectA newline choices endusing {
		char* tmp;
		asprintf(&tmp,"ENDUSING%d: ;",top(&usingstack));
		$$ = g_slist_multiconcat($3,
			txt2list("var1=pop(&stack);"),
			$6,
			g_slist_append(NULL,tmp),
			NULL);
		pop(&usingstack);
		};

stmtwhile:
	whileA expr optnewline stmt {
		$$ = g_slist_multiconcat(
			txt2list("while (1) {"),
			$2,
			txt2list("if (isTrue(pop(&stack))) {"),
			$4,
			txt2list("} else break;}"),
			NULL);
		};

stmtwrite:
	write openparenth parameter comma parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			$7,
			txt2list("dbl_write(pop(&stack),pop(&stack),pop(&stack));"),
			NULL);
		};

stmtwrites:
	writes openparenth parameter comma parameter closeparenth newline {
		$$ = g_slist_multiconcat(
			$3,
			$5,
			txt2list("dbl_writes(pop(&stack),pop(&stack));"),
			NULL);
	};

stmtxcall:
	xcall ident openparenth parameters closeparenth newline {
		GSList* tmp = $4;
		char* instruct;
		char* params=NULL;
		char* paramscalc=NULL;
		int nbparams=g_slist_length($4);
		int i;
		params=(char*)malloc(nbparams*12*sizeof(char));
		for (i=0;i<nbparams;i++) {
			memcpy(params+i*12,"pop(&stack),",12);
		}
		params[nbparams*12-1]='\0';
		while (tmp != NULL) {
			paramscalc=g_strconcat((char*)tmp->data,paramscalc,NULL);
			tmp = g_slist_next(tmp);
		}
		instruct=g_strconcat("XCALL p",$2," (",params,");",NULL);
		$$ = g_slist_multiconcat($4,txt2list(instruct),NULL);
		free(instruct);
		}
	| xcall id newline {
		$$ = g_slist_append(NULL,g_strconcat("XCALL p",$2,"();",NULL));
		};

openmode:
	openmainmode {
		$$ = g_strconcat($1,",SEQUENTIAL",NULL);
		}
	| openmainmode colonD opensubmode {
		$$ = g_strconcat($1,",",$3,NULL);
		}
	| inputindexed {
		$$ = g_strdup("INPUT,INDEXED");
		}
	| updateindexed {
		$$ = g_strdup("UPDATE,INDEXED");
		};

openmainmode:
	inputD { $$ = g_strdup("INPUT"); }
	| output { $$ = g_strdup("OUTPUT"); }
	| update { $$ = g_strdup("UPDATE"); }
	| append { $$ = g_strdup("APPEND"); }
	;

opensubmode:
	sequential { $$ = g_strdup("SEQUENTIAL"); }
	| print { $$ = g_strdup("PRINT"); }
	| relative { $$ = g_strdup("RELATIVE"); }
	| indexed { $$ = g_strdup("INDEXED"); }
	;

externaldef:
	id comma id newline

externaldefs:
	externaldef
	| externaldefs externaldef {}

stmtexternal:
	external functionA newline externaldefs {
		$$ = txt2list("// external : Not implemented yet.");
		}

stmtexitloop:
	exitloop newline {
		$$ = txt2list("break;");
		}

%%
