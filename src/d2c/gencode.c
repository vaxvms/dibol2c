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

/*
	gencode.c

	This file contains functions called by the parser to
	generate code
*/

#include "gencode.h"
#include "d2c.h"
#include <string.h>
#include <stdlib.h>

int nbtmp=0;

/*
 *         concat several GSList together
 */
GSList *
g_slist_multiconcat (GSList * list1, ...)
{
  GSList *concat = list1;
  va_list arg;
  int i;
  GSList *list;
  GSList *tmp = list1;

  va_start (arg, list1);

  while (tmp->next != NULL)
    tmp = g_slist_next (tmp);

  while ((list = va_arg (arg, GSList *)) != NULL)
    {
      tmp->next = list;
      while (tmp->next != NULL)
	tmp = g_slist_next (tmp);
    }

  va_end (arg);

  return concat;
}

/*
 *         Copy a text and put it in a list element
 */
GSList *
txt2list (char *text)
{
  GSList *list = NULL;
  list = g_slist_append (list, g_strdup (text));
  return list;
}

/* Generate code for a common definition
 */
GSList *
gencode_common_def (char *commonname, int redef, GSList * fields)
{
  GSList *gencode = NULL;
  FieldDef *def;
  char *line;

 if (commonname != NULL)
    {
      asprintf (&line, "COMMON variable* v%s;", commonname);
      gencode = g_slist_prepend (gencode, line);
    }

  fields = g_slist_reverse (fields);
  while (fields != NULL) {
   def = g_slist_nth_data (fields, 0);
   fields = g_slist_remove (fields, def);
   if (def->name != NULL) {
      asprintf(&line, "COMMON variable* v%s;",
               def->name);
   } else {
      asprintf (&line, "COMMON variable* gtmp%d;", nbtmp);
      nbtmp++;
   }
   gencode = g_slist_prepend (gencode, line);
   free (def);
  }
  
  gencode = g_slist_reverse (gencode);
  return gencode;
}

/* Generation initialisation code for a common definition
 * XXX : this function is (almost) a copy of gencode_fields_def
 */
GSList *
gencode_common_init_def (char *commonname, int redef, GSList * fields)
{
  GSList *gencode = NULL;
  FieldDef *def;
  char *line;
  char *str;
  int size = 0;
  GSList *tmp = fields;

  /* Compute the size of the record */
  while (tmp != NULL)
    {
      size += (*(FieldDef *) (tmp->data)).size;
      tmp = g_slist_next (tmp);
    }

  /* Fields definition */
  tmp = fields;
  while (tmp != NULL)
    {
      def = g_slist_nth_data (tmp, 0);
      tmp = g_slist_next (tmp);
      if (def->type == tDECIMAL)
	asprintf (&str, "tDECIMAL");
      if (def->type == tALPHA)
	asprintf (&str, "tALPHA");
      if (def->type == tINTEGER)
	asprintf (&str, "tINTEGER");
      if (def->name != NULL)
	{
	  char *data;
	  if (!def->data)
	    data = strdup ("NULL");
	  else
	    asprintf (&data, "\"%s\"", def->data);
	  asprintf (&line, "v%s=newvar(vh,%s,%d,%d,%d,%d,%s);",
		    def->name, str, def->size, def->nbelts, redef, def->name == NULL,
		    data);
	}
      else
	{
	  char *data;
	  if (!def->data)
	    data = strdup ("NULL");
	  else
	    asprintf (&data, "\"%s\"", def->data);
	  nbtmp++;
	  asprintf (&line, "gtmp%d=newvar(vh,%s,%d,%d,%d,%d,%s);", nbtmp, str,
		    def->size, def->nbelts, redef, def->name == NULL, data);
	}
      gencode = g_slist_prepend (gencode, line);
      free (str);
    }

  /* Common definition */
 if (commonname != NULL)
    {
      asprintf (&line, "v%s=newvar(vh,tRECORD,%d,1,%d,0,NULL);", commonname,
		size, redef);
      gencode = g_slist_prepend (gencode, line);
    }
  if (redef)
    {
      asprintf (&line, "vh->offset=vh->offset-%d;", size);
      gencode = g_slist_prepend (gencode, line);
    }
 

  //gencode = g_slist_reverse (gencode);
  return gencode;
}

/* Generate code for a record definition
 * -----
 * recordname : name of the record, NULL if the record has no name
 * redef : true if this is a redefinition
 * fields : list of fields in the record
 * ---
 * return a list containing the generated code
 */
GSList *
gencode_fields_def (char *recordname, int redef, GSList * fields)
{
  GSList *gencode = NULL;
  FieldDef *def;
  char *line;
  char *str;
  int size = 0;
  GSList *tmp = fields;

  /* Compute the size of the record */
  while (tmp != NULL)
    {
      size += (*(FieldDef *) (tmp->data)).size;
      tmp = g_slist_next (tmp);
    }

  /* Record definition */
  if (redef)
    {
      asprintf (&line, "vh->offset=vh->offset-%d;", size);
      gencode = g_slist_prepend (gencode, line);
    }
  if (recordname != NULL)
    {
      asprintf (&line, "variable* v%s=newvar(vh,tRECORD,%d,1,0,0,NULL);", recordname,
		size);
      gencode = g_slist_prepend (gencode, line);
    }

  /* Fields definition */
  fields = g_slist_reverse (fields);
  while (fields != NULL)
    {
      def = g_slist_nth_data (fields, 0);
      fields = g_slist_remove (fields, def);
      if (def->type == tDECIMAL)
	asprintf (&str, "tDECIMAL");
      if (def->type == tALPHA)
	asprintf (&str, "tALPHA");
      if (def->type == tINTEGER)
	asprintf (&str, "tINTEGER");
      if (def->name != NULL)
	{
	  char *data;
	  if (!def->data)
	    data = strdup ("NULL");
	  else
	    asprintf (&data, "\"%s\"", def->data);
	  asprintf (&line, "variable* v%s=newvar(vh,%s,%d,%d,0,%d,%s);",
		    def->name, str, def->size, def->nbelts, def->name == NULL,
		    data);
	  free (data);
	}
      else
	{
	  char *data;
	  if (!def->data)
	    data = strdup ("NULL");
	  else
	    asprintf (&data, "\"%s\"", def->data);
	  nbtmp++;
	  asprintf (&line, "variable* tmp%d=newvar(vh,%s,%d,%d,0,%d,%s);", nbtmp, str,
		    def->size, def->nbelts, def->name == NULL, data);
	  free(data);
	}
      gencode = g_slist_prepend (gencode, line);
      free (def);
      free (str);
    }

  gencode = g_slist_reverse (gencode);
  return gencode;
}

/* Allocate a struct describing a field
 * -----
 * name : name of the field
 * type : text describing the type of the field
 * data : default data for the field, NULL if no data
 * ---
 * return a struct fielddef describing the field
 */
FieldDef *
newFielddef (const char *name, const char *type, char *data)
{
  FieldDef *def = (FieldDef *) malloc (sizeof (FieldDef));

  /* Store the name in upper case */
  def->name = g_strdup (name);
  if (def->name != NULL)
    g_strup (def->name);

  /* Parse the type (type and length) */
  switch (*type)
    {
    case 'A':
      def->type = tALPHA;
      break;
    case 'D':
      def->type = tDECIMAL;
      break;
    case 'I':
      def->type = tINTEGER;
      break;
    }
  type++;
  def->size = atoi (type);
  type--;

  /* data */
  if (data != NULL)
    def->data = g_strdup (data);
  else
    def->data = NULL;
  def->nbelts = 1;

  return def;
}

/*
 *         Generate the decoration for source code
 */
GSList *
gencode_function (char *prototype, GSList * vardecl, GSList * commondecl, GSList * commoninit, GSList * stmts)
{
  GSList *code = NULL;
  FILE *headerfile;
  char *headername;
  char *symbolname;
  char *init = "\n";

  if (commondecl == NULL) 
     commondecl = txt2list ("/* No common declarations */");

  if (commoninit == NULL) 
     commoninit = txt2list ("/* No common initialisation */");

  if (prototype == NULL)
    {
      commondecl = g_slist_prepend(commondecl,"#define COMMON");
      prototype = "int main(int argc, char **argv)";
      init = "initDibol(argv[0]);";
    } else {
      commondecl = g_slist_prepend(commondecl,"#define COMMON extern");
      commoninit = txt2list ("/* No common initialisation */");
    }
  if (vardecl == NULL)
    vardecl = txt2list ("/* No variables declarations */");
  if (stmts == NULL)
    stmts = txt2list ("/* No statements */");
  code = g_slist_multiconcat (commondecl,
                              txt2list (prototype),
			      txt2list ("{"),
			      txt2list
			      ("Varhandle* vh = newVarhandle(1000);"),
			      txt2list ("Stack* stack;"),
			      txt2list ("variable* var1;"),
			      txt2list ("variable* var2;"),
                              commoninit,
			      vardecl, txt2list (init),
			      stmts, txt2list ("freeVarhandle(vh);"),
			      txt2list ("}"), NULL);

  /* Produce an empty header file */
  headername = changeExt (srcfilename, "h");
  symbolname = toSymbol (headername);
  headerfile = fopen (headername, "w");
  fprintf (headerfile, "#ifndef %s\n#define %s\n\n\n#endif\n", symbolname,
	   symbolname);
  fclose (headerfile);
  free (headername);

  return (code);
}

/*
 * 	Generate the code for the FOR instruction
 */
GSList *
gencode_for (GSList * exstart, GSList * exstop, GSList * exby,
	     GSList * exstmts, char *idname)
{
  GSList *code = NULL;
  char *tmp1 =
    g_strconcat ("assign(v",idname,",pop(&stack));",
		 NULL);
  char *tmp2 =
    g_strconcat ("push(&stack,v", idname, ");", NULL);
  char *tmp3 = g_strconcat ("assign (v", idname,
			    ", add(pop(&stack),pop(&stack)));", NULL);
  if (exby == NULL)
    exby = txt2list ("push(&stack, imm (tDECIMAL, 1, \"1\"));");
  code = g_slist_multiconcat (exstart,
			      g_slist_append (NULL, tmp1),
			      txt2list ("while (1) {"),
			      exstop,
			      g_slist_append (NULL, tmp2),
			      txt2list
			      ("push(&stack,le(pop(&stack),pop(&stack)));"),
			      txt2list ("if (!isTrue(pop(&stack))) {"),
			      exstmts, txt2list ("} else break;"),
			      txt2list(tmp2), exby,
			      g_slist_append (NULL, tmp3), txt2list ("}"),
			      NULL);

  return code;
}
