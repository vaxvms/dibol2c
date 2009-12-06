/**************************************************************************
 * writes.c from libdibol
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

#include "dibol.h"
#include "filefonc.h"
#include <stdlib.h>

void dbl_writes (variable* ch, variable* record)
{
	Channel* canal=findchannel(decimal2integer(ch));
	if (canal==NULL) error(11);
	if (canal->terminal==1) {
		fwrite(getdata(record),getsize(record),1,stdout);
		printf("\n");	
		return;
	}
	if (canal->submode == SEQUENTIAL) {
		fwrite(getdata(record),getsize(record),1,canal->fh);
		fprintf(canal->fh,"\n");
		return;
	}
	if (canal->submode == RELATIVE ) {
		int seqnum;
		char *query;
		MYSQL_RES *result;
		MYSQL_ROW row;
		char *data = alpha2char (record);
		/* Compute the next free id */
		/* We iterate through the id while we found a free place */
		seqnum=(canal->lastid)+1;
		asprintf(&query,"SELECT id FROM %s WHERE id>%d ORDER BY id",canal->filespec,canal->lastid);
		mysql_query(dibol->dbh,query);
		free(query);
		result=mysql_use_result(dibol->dbh);
		while ( (row=mysql_fetch_row(result))) {
			if (seqnum!=strtol(row[0],NULL,10))
				break;
			seqnum++;
		}
		mysql_free_result(result);

		/* write the record */
		asprintf(&query,"INSERT INTO %s (id,data) VALUES (%d,\"%s\")",canal->filespec,seqnum,data);
		mysql_query(dibol->dbh,query);
		free(query);
		free(data);
		
		canal->lastid=seqnum;

		return;
	}
}
