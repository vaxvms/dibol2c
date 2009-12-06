INCLUDES=-I../../lib/common

bin_PROGRAMS = d2c
d2c_SOURCES = dibolparser.y dibollexer.l symbols.c d2c.c gencode.c
noinst_HEADERS = d2c.h symbols.h gencode.h dibolparser.h
d2c_LDADD = @LEXLIB@ $(top_srcdir)/lib/common/libdibolcommon.la

## YACC = @YACC@ -d --debug -v
YACC = @YACC@ -d -t

## Dibol is case-independant so -i
LEX = @LEX@ -i
