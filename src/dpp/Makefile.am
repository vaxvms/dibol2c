bin_PROGRAMS=dpp
dpp_SOURCES=dpp.c dpplexer.c dppparser.tab.c
noinst_HEADERS = dpplexer.h dppparser.tab.h
dpp_LDADD = @LEXLIB@ $(top_srcdir)/lib/common/libdibolcommon.la

YACC = bison

dpplexer.h dpplexer.c: dpplexer.l dppparser.tab.h
	$(LEX) -o dpplexer.c dpplexer.l

dppparser.tab.h dppparser.tab.c: dppparser.y
	$(YACC) dppparser.y -d

## YACC = @YACC@ -d --debug -v

