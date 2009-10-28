/*
	Implementation code of the symbols table
	We internaly use a GSList to keep the symbols
*/

#include "symbols.h"
#include <stdlib.h>

/* Initialisation of the symbols table */
Symbols initSymbols()
{
    /* No need of initialisation */
    return NULL;
}

Symbol *createLabel(char *name)
{
    Symbol *symbol;
    symbol = (Symbol *) malloc(sizeof(Symbol));
    symbol->name = name;
    symbol->type = LABEL;
    symbol->length = -1;
    return symbol;
}

Symbol *createFunction(char *name)
{
    Symbol *symbol;
    symbol = (Symbol *) malloc(sizeof(Symbol));
    symbol->name = name;
    symbol->type = FUNCTION;
    symbol->length = -1;
    return symbol;
}

Symbol *createVariable(char *name, int length)
{
    Symbol *symbol;
    symbol = (Symbol *) malloc(sizeof(Symbol));
    symbol->name = name;
    symbol->type = VARIABLE;
    symbol->length = length;
    return symbol;
}

Symbol *createParameter(char *name)
{
    Symbol *symbol;
    symbol = (Symbol *) malloc(sizeof(Symbol));
    symbol->name = name;
    symbol->type = PARAMETER;
    symbol->length = -1;
    return symbol;
}

/* Add a symbol to the table */
void addSymbol(Symbols * symbols, Symbol * symbol)
{
    *symbols = g_slist_append(*symbols, symbol);
}

gint findbyname(gconstpointer a, gconstpointer b)
{
    gint i = !((strcmp(((Symbol *) a)->name, ((Symbol *) b)->name) == 0)
	       && (((Symbol *) a)->type == ((Symbol *) b)->type));
    return i;
}

/* find a symbol by name in the table */
Symbol *findSymbol(Symbols * symbols, char *name, int type)
{
    Symbol symbol;
    GSList *result;
    symbol.name = name;
    symbol.type = type;
    result = g_slist_find_custom(*symbols, &symbol, findbyname);
    if (result == NULL)
	return NULL;
    else
	return (Symbol *) (result->data);
}

/* Freeing of the symbol table */
void freeSymbols(Symbols * symbols)
{
    g_slist_free(*symbols);
}
