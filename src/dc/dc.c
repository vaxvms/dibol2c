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
 * dc is the dibol compiler 
 */

/*
 * Enable this if you want debug output 
 */
// #define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct
{
  char *name;
  int compiled;
}
SourceFile;

#if 0
/*
 * not used yet 
 */
/*
 * Add an include directive in a header file 
 */
void
addinclude (char *headername, char *includename)
{
  char *line = NULL;
  int linesize = 0;
  FILE *header;
  if (!(header = fopen (headername, "rw")))
    {
      return;
    }
  while (getline (&line, &linesize, header) != -1)
    {
      fprintf (header, "%s", line);
      if (strstr (line, "DEFINE ") != NULL)
	{
	  fprintf ("#include \"%s\"\n", includename);
	}
      free (line);
      line = NULL;
      linesize = 0;
    }
  fclose (header);
}
#endif


int
compareSourceFile (gconstpointer a, gconstpointer b)
{
  char *filename1 = ((SourceFile *) a)->name;
  char *filename2 = (char *) b;
  return strcmp (filename1, filename2);
}

int
compareTarget (gconstpointer a, gconstpointer b)
{
  char *filename1 = ((SourceFile *) ((GSList *) a)->data)->name;
  char *filename2 = ((SourceFile *) b)->name;
  return strcmp (filename1, filename2);
}

/*
 * return a pointer to a newly allocated string containing the filename with
 * extension changed to ext 
 */
char *
changeExt (char *filename, char *ext)
{
  int i;
  char *newfilename;
  i = strlen (filename) - 1;
  while (i >= 0 && filename[i] != '.')
    i--;
  if (filename[i] == '.')
    {
      int extlen = strlen (ext);
      newfilename = (char *) malloc ((i + extlen + 1) * sizeof (char));
      memcpy (newfilename, filename, i + 1);
      memcpy (newfilename + i + 1, ext, extlen);
      newfilename[i + extlen + 1] = '\0';
      return newfilename;
    }
  else
    return g_strconcat (filename, ext, NULL);
}

/*
 * Allocate a new structure to deal with file compilation
 */
SourceFile *
newFile (char *name)
{
  SourceFile *file = (SourceFile *) malloc (sizeof (SourceFile));
  file->name = name;
  file->compiled = 0;
  return file;
}

/*
 * Read and parse a rules file * You get back 2 lists : *  - One with the
 * file you'll have to translate and compile *  - One with the rules to apply 
 * to the file. * * the file list is a list of File structure. * the rules
 * list contains lists describing a rule. * A rule is a list of element
 * pointing to a File structure in the file list. * The first element is the
 * file you want to compile, the others are * the dependencies * *  this will 
 * be eventually replaced by a LEX/YACC parser 
 */
GSList *
parse_file (char *filename, GSList ** files)
{
  FILE *prj;
  char *line;
  int len = 0;
  char *fichier;
  char *depend;
  GSList *rules = NULL;
  GSList *rule = NULL;
  GSList *tmp;
  SourceFile *file;

  prj = fopen (filename, "r");
  while ((len = getline (&line, &len, prj)) != -1)
    {
      char *token;
      if (sscanf (line, "%a[a-zA-Z0-9.]%*[^:]:%a[^\n]", &fichier, &depend))
	{
	  if (
	      (tmp =
	       g_slist_find_custom (*files, fichier,
				    compareSourceFile)) == NULL)
	    {
	      file = newFile (fichier);
	      *files = g_slist_prepend (*files, file);
	    }
	  else
	    file = tmp->data;
	  rule = g_slist_append (rule, file);
	  token = strtok (depend, " ");
	  do
	    {
	      if (
		  (tmp =
		   g_slist_find_custom (*files, token,
					compareSourceFile)) == NULL)
		{
		  file = newFile (g_strdup (token));
		  *files = g_slist_append (*files, file);
		}
	      else
		file = tmp->data;
	      rule = g_slist_append (rule, file);
	    }
	  while (token = strtok (NULL, " "));
	  free (depend);
	  rules = g_slist_append (rules, rule);
	  rule = NULL;
	}
      free (line);
      line = NULL;
      len = 0;
    }
  free (line);
  fclose (prj);
  return rules;
}

/*
 * Display a File structure
 */
void
printFile (gpointer data, gpointer user_data)
{
  SourceFile *file = (SourceFile *) data;
  printf ("%s : compiled=%d\n", file->name, file->compiled);
}

/*
 * Run the various command needed to compile a file
 * 
 * return 0 if ok x if there is a problem to access the file 1000+y if there
 * is a problem during pre-processing 2000+z if there is a problem during
 * translation 3000+t if there is a problem during indentation 
 */
int
realcompil (char *filename)
{
  char *filename1;
  char *command;
  int result;

  /*
   * Pre-processor 
   */
  filename1 = changeExt (filename, "DPP");
  asprintf (&command, "dpp ../%s %s", filename, filename1);
  printf ("%s\n", command);
  result = system (command);
  free (command);
  if (result != 0)
    return (1000 + result);

  /*
   * Translation 
   */
  asprintf (&command, "d2c %s", filename1);
  printf ("%s\n", command);
  result = system (command);
  free (command);
  free (filename1);
  if (result != 0)
    return (2000 + result);

  /*
   * Indentation 
   */
  /*
   * The return code for indent are not given in the man page * to find it, 
   * use the force and look at indent.h 
   */
  filename1 = changeExt (filename, "c");
  asprintf (&command, "indent %s", filename1);
  printf ("%s\n", command);
  result = system (command);
  free (command);
  free (filename1);
  if (result != 0)
    return (3000 + result);

  return 0;
}

/*
 * Compile a file and his dependencies
 */
int
compileFile (GSList * rules, SourceFile * file)
{
  GSList *tmp;
  GSList *tmp2;
  int result;

  if (file->compiled)
    {
      printf ("%s is already compiled\n", file->name);
      return;
    }

  if ((tmp = g_slist_find_custom (rules, file, compareTarget)) == NULL)
    {
      printf ("No depends for %s, we compile it.\n", file->name);

      /*
       * here, the real compilation took place 
       */
      if ((result = realcompil (file->name)) == 0)
	{
	  file->compiled = 1;
	  return 0;
	}
      else
	return result;
    }
  else
    {
      printf ("Depends for %s, we process them.\n", file->name);
      tmp2 = tmp->data;
      while ((tmp2 = g_slist_next (tmp2)) != NULL)
	if (compileFile (rules, tmp2->data) == 0)
	  return 0;
      printf ("Now, it's ok for %s. let's go\n", file->name);
      if ((result = realcompil (file->name)) == 0)
	{
	  file->compiled = 1;
	  return 0;
	}
      else
	return result;
    }
}

/*
 * Run all the rules
 */
void
run_compilation (GSList * rules, GSList * files)
{
  GSList *tmp;
  GSList *tmp2;
  SourceFile *file;
  int result;
  char *compilecommand;
  char *binname;
  unsigned int i;

  /*
   * Create the directory for temporary file 
   */
  mkdir ("c", S_IRWXU);
  chdir ("c");
  printf ("Running compilation...\n");
  tmp = rules;
  while (tmp != NULL)
    {
      tmp2 = (GSList *) tmp->data;
      file = (SourceFile *) tmp2->data;
      printf ("Compilation of %s\n", file->name);
      while (tmp2 != NULL)
	{
	  if (!file->compiled)
	    if ((result = compileFile (rules, file)) != 0)
	      {
		fprintf (stderr, "* Error while processing %s\n", file->name);
		switch (result / 1000)
		  {
		  case 0:
		    fprintf (stderr, "Error while accessing the file\n");
		    break;
		  case 1:
		    fprintf (stderr, "Error while pre-processing\n");
		    break;
		  case 2:
		    fprintf (stderr, "Error while translating to C\n");
		    break;
		  case 3:
		    fprintf (stderr, "Error while indenting.\n");
		    break;
		  default:
		    fprintf (stderr, "Error at unknown stage !!\n");
		    break;
		  }
		fprintf (stderr, "Program return error code : %d\n",
			 result % 1000);
		return;
	      }
	  tmp2 = g_slist_next (tmp2);
	}
      tmp = g_slist_next (tmp);
    }
  printf ("Cleaning temporary file\n");
  system ("rm -f *~");
  system ("rm -f *.DPP");
  printf ("Translation in C is available in the \"c\" subdir\n");
  printf ("Compile everything\n");

  /* Compilation of the whole thing 
   * FIXME : Produce a Makefile instead of running gcc 
   * The name of the output is the name of the first 
   * rule without the extension 
   */
  binname = ((SourceFile *) (files->data))->name;
  i = strlen (binname);
  while (i >= 0 && binname[i] != '.')
    i--;
  if (i != 0)
    binname[i] = 0;
  asprintf (&compilecommand,
	    "gcc -o %s *.c `dibol-config --cflags` `dibol-config --libs`",
	    binname);
  printf ("%s\n", compilecommand);
  system (compilecommand);
  free (compilecommand);
  chdir ("..");
}

int
main (int argc, char *argv[])
{
  GSList *rules;
  GSList *tmp;
  GSList *files = NULL;
  if (argc != 2)
    {
      fprintf (stderr, "dc, dibol compiler\nusage : dc projectfile\n");
      exit (-1);
    }

  /*
   * Read the project file 
   */
  rules = parse_file (argv[1], &files);

#ifdef DEBUG
  /*
   * print the files state 
   */
  printf ("List of files in the project :\n");
  g_slist_foreach (files, printFile, NULL);

  /*
   * print the rules 
   */
  printf ("project rules :\n");
  tmp = rules;
  while (tmp != NULL)
    {
      printf ("New rule :\n");
      g_slist_foreach (tmp->data, printFile, NULL);
      tmp = g_slist_next (tmp);
    }
#endif

  /*
   * Launch the compilation 
   */
  run_compilation (rules, files);

#ifdef DEBUG
  /*
   * print the files state 
   */
  g_slist_foreach (files, printFile, NULL);
#endif
  return 0;
}
