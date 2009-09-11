/**************************************************************************
 * read.c from libdibol
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

#include "read.h"
#include "filefonc.h"
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>

/*
	input a record from a relative or a indexed file
	3-51 and 3-53
*/
void dbl_read(variable * ch, variable * record, variable * field)
{
    Channel *canal = findchannel(decimal2integer(ch));
    if (canal->submode == RELATIVE) {
	char *query;
	int64 id = decimal2integer(field);
	MYSQL_ROW row;
	MYSQL_RES *result;
	unsigned long *lengths;
	asprintf(&query, "SELECT data FROM %s WHERE id=%Ld",
		 canal->filespec, id);
	if (mysql_query(dibol->dbh, query) != 0)
	    error(22);
	free(query);
	result = mysql_store_result(dibol->dbh);
	if (mysql_num_rows(result) != 1)
	    error(64);
	row = mysql_fetch_row(result);
	lengths = mysql_fetch_lengths(result);
	memcpy(getdata(record), row[0], lengths[0]); /* FIXME we don't check for record size */
	mysql_free_result(result);
	canal->lastid=id;
	return;
    }
    if (canal->submode == INDEXED) {
	int wantedoffset = getdata(field) - getdata(record);
	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field2;
	char *query;
	unsigned long *lengths;
	int tmp = 0;
	int keyfield = 1;
	int offset = 0;
	int num_fields = 0;
	char *buffer;
	int i = 0;
	int len = 0;

	/* Look for the field to use as the key */
	query = g_strconcat("SHOW COLUMNS from ", canal->filespec, NULL);
	mysql_query(dibol->dbh, query);
	result = mysql_use_result(dibol->dbh);
	free(query);
	mysql_fetch_row(result); /* pass id field */
	while ((row = mysql_fetch_row(result)) && (offset < wantedoffset)) {
	    lengths = mysql_fetch_lengths(result);
	    sscanf(row[1], "char(%d)", &tmp);
	    offset += tmp;
	    keyfield++;
	}
	/* Can't find it */
	if (offset != wantedoffset) {
	    mysql_free_result(result);
	    error(52);
	}
	/* look for the asked record */
	asprintf(&query, "SELECT * FROM %s WHERE %.*s>=\"%.*s\" LIMIT 1",
		 canal->filespec, lengths[0], row[0], getsize(field), getdata(field));

	/* Copy the last keyfield and keydata for reads */
	if (canal->lastkeyfld!=NULL)
		free(canal->lastkeyfld);
	canal->lastkeyfld=strdup(row[0]);
	if (canal->lastkey!=NULL)
		free(canal->lastkey);
	canal->lastkey=(char*)malloc((getsize(field)+1)*sizeof(char));
	memset(canal->lastkey,0,(getsize(field)+1));
	memcpy(canal->lastkey,getdata(field),getsize(field));

	mysql_free_result(result);
	mysql_query(dibol->dbh, query);
	result = mysql_store_result(dibol->dbh);
	free(query);
	/* Can't find it */
	if (mysql_num_rows(result) == 0) {
	    mysql_free_result(result);
	    error(1);
	}

	row = mysql_fetch_row(result);
	lengths = mysql_fetch_lengths(result);
	/* Is it the right one ? */
	if (memcmp
	    (getdata(field), row[keyfield],
	     getsize(field) <
	     lengths[keyfield] ? getsize(field) : lengths[keyfield]) != 0) {
		/* FIXME According to the dibol manual, we have to return the record with the next higher key */
	    mysql_free_result(result);
	    error(53);
	}
	/* Copy the record */
	buffer = (char *) malloc(sizeof(char));
	while (field2 = mysql_fetch_field(result)) {
		if (strcmp(field2->name,"id")==0) {
			canal->lastid=strtol(row[i],NULL,10);
			i++;
			continue;
		}
	    len += field2->length;
	    buffer = (char *) realloc(buffer, (len + 1) * sizeof(char));
	    memset(buffer + len - field2->length, 32, field2->length);
	    memcpy(buffer + len - field2->length, row[i], lengths[i]);
	    i++;
	}
	memcpy(getdata(record), buffer, getsize(record));
	free(buffer);
	mysql_free_result(result);
    }
}
