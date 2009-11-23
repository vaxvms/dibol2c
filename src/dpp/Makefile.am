bin_PROGRAMS=dpp
dpp_SOURCES=dpp.c dpplexer.l dppparser.y
noinst_HEADERS = dpplexer.h dppparser.h
dpp_LDADD = @LEXLIB@ $(top_srcdir)/lib/common/libdibolcommon.la

YACC = @YACC@ -d --debug -v
LEX= @LEX@ -i
