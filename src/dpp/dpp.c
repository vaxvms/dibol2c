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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <glib.h>

int lineno = 1;
extern char* yytext;	/* Text of the lastest token */
extern FILE* yyin;	/* input file */
extern int yydebug;
GSList* include_dirs=NULL;

int yyerror(const char *msg) {
        fprintf(stderr,"** line %d: %s at '%s'\n", lineno, msg, yytext);
}

int main(int argc, char **argv)
{
  int c;
  int index;
  FILE *src;

  while ((c = getopt (argc, argv, "dI:")) != -1)
    switch (c)
      {
      case 'I':
        include_dirs=g_slist_append(include_dirs,strdup(optarg));
	break;
      case 'd':
        yydebug=1;
        break;
      default:
	exit (EXIT_FAILURE);
      }

  /* take the first non option argument as the source filename */
  if (optind >= argc) {
    fprintf(stderr,"%s: No input file\n",argv[0]);
    exit(1);
  }
  if (! (src = fopen(argv[optind],"r"))) {
    fprintf(stderr,"could not open %s\n",argv[optind]);
    exit(2);
  }

  /* Add current dir to the top of the include dir list */
  include_dirs=g_slist_prepend(include_dirs,strdup("."));

  yyin = src;
  return yyparse();
}
