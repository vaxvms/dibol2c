bin_PROGRAMS=dpp
dpp_SOURCES=dpp.c dpplexer.l dppparser.y
BUILT_SOURCES= dpplexer.h dppparser.h
dpp_LDADD = @LEXLIB@ $(top_srcdir)/lib/common/libdibolcommon.la

YACC = @YACC@ -d --debug -v
LEX= @LEX@ -i

CLEANFILES=dpplexer.h
dpplexer.h: dpplexer.l dppparser.c
	$(LEX) --header-file=$@ -o /dev/null $<

