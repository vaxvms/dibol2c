/**************************************************************************
 * variable.c from libdibol
 *
 * (c) 2001 Nicolas DUPEUX                      nicolas@dupeux.net
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 **************************************************************************/

#include "variables.h"
#include "operators.h"
#include <stdlib.h>

/*
 * Create a new Varhandle
 */
Varhandle *newVarhandle(size_t size)
{
    Varhandle *vh = (Varhandle *) malloc(sizeof(Varhandle));

    vh->offset = 0;
    vh->data = (char *) malloc(size*sizeof(char));

    return vh;
}

/*
 * Free a Varhandle
 */
void freeVarhandle(Varhandle *vh)
{
    free(vh->data);
    free(vh);
}

/*
 * Declare a new variable in a Varhandle
 */
variable* newvar(Varhandle * vh, int type, size_t size, size_t nbelts, int redef,
	   int noname, char *defval)
{
    variable *var;
    var = (variable *) malloc(sizeof(variable));
    var->type = type;
    var->size = size;
    var->nbelts = nbelts;
    var->data = vh->data + vh->offset;
    var->freetype=0;

    if (defval != NULL)
// XXX : bug here. i must take the min between size and strlen
	memcpy(var->data, defval, MIN(size * nbelts, strlen(defval)));
    else if (!redef)
	dbl_clear(var);
    if ((!redef) && (type != tRECORD))
	vh->offset = vh->offset + size;
    return var;
}

variable* indexvar(Varhandle* vh, variable* var, int index)
{
	variable* var2=(variable*)malloc(sizeof(variable));
	*var2=*var;
	var2->data=var2->data+(--index)*var2->size;
	return var2;
}

void freevar(variable* var)
{
	if (var->freetype==1)
		free(var->data);
	free(var);
}

/*
 * return a pointeur to a portion of a variable
 */
variable *substring(variable * var, variable * start, variable * end)
{
    variable *var2 = (variable *) malloc(sizeof(variable));
    *var2 = *var;
    var2->data += decimal2integer(start) - 1;
    var2->size = decimal2integer(end) - decimal2integer(start) + 1;
    return var2;
}

/*
 * Create a variable with an immediate value
 */
variable *imm(int type, size_t size, char *data)
{
    variable *var = (variable *) malloc(sizeof(variable));
    var->type = type;
    var->size = size;
    var->nbelts = 1;
    var->data=(char*)malloc(size*sizeof(char));
    memcpy(var->data,data,size);
    var->freetype=1;
    return (var);
}

/*
 * return the type of a variable
 */
int gettype(variable * var)
{
    return (var->type);
}

/*
 * return the declared size of a variable
 */
int getsize(variable * var)
{
    return (var->size);
}

/*
 * return a pointer to the data of a variable
 */
char *getdata(variable * var)
{
    return (var->data);
}

/*
 * return true if the variable value is true
 */
int isTrue(variable * var)
{
    int64 result;
    result = decimal2integer(var);
    return (result != 0);
}
