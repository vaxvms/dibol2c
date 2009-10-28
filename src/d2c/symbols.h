#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <glib.h>

#define LABEL		1
#define FUNCTION	2
#define VARIABLE	3
#define PARAMETER	4

typedef struct
{
  char *name;
  int type;

  int length;
}
Symbol;

typedef GSList *Symbols;

/* Initialisation of the symbols table */
Symbols initSymbols ();

/* Helpers to create new symbols */
Symbol *createLabel (char *name);
Symbol *createFunction (char *name);
Symbol *createVariable (char *name, int length);
Symbol *createParameter (char *name);

/* Add a symbol to the table */
void addSymbol (Symbols * symbols, Symbol * symbol);

/* find a symbol in the table by name and type */
Symbol *findSymbol (Symbols * symbols, char *name, int type);

/* Freeing of the symbol table */
void freeSymbols (Symbols * symbols);


#endif
