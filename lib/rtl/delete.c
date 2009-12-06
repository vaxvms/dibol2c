/**************************************************************************
 * delete.c from libdibol
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

#include "delete.h"
#include "filefonc.h"

void dbl_delete(variable *ch, variable *keyfld)
{
	Channel *canal = findchannel(decimal2integer(ch));
	if (canal->submode == INDEXED) {
		/* You can note that keyfld is useless */
		char* query;
		asprintf(&query,"DELETE FROM %s WHERE id=%d",canal->filespec,canal->lastid);
		mysql_query(dibol->dbh,query);
		free(query);
	}
}
