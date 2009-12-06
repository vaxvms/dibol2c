#ifndef GENCODE_H
#define GENCODE_H

#include <glib.h>
#include "d2c.h"

GSList *g_slist_multiconcat (GSList * list1, ...);
GSList *txt2list(char *text);
GSList* gencode_fields_def(char* recordname, int redef, GSList* fields);
FieldDef* newFielddef(const char* name, const char* type, char* data);
GSList* gencode_common_init_def(char* commonname, int redef, GSList* fields);
GSList* gencode_common_def(char* commonname, int redef, GSList* fields);
GSList *gencode_function(char *prototype, GSList * vardecl, GSList * commondecl, GSList * commoninit, GSList * stmts);
GSList *gencode_for(GSList * exstart, GSList * exstop, GSList * exby, GSList * exstmts, char *idname);

#endif
