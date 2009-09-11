bin_PROGRAMS=dpp
dpp_SOURCES=dpp.c dppparser.y dpplexer.l
noinst_HEADERS = dpplexer.h dppparser.tab.h
dpp_LDADD = @LEXLIB@ $(top_srcdir)/lib/common/libdibolcommon.la

## YACC = @YACC@ -d --debug -v
YACC = @YACC@ -d -t

