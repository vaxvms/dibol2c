/**************************************************************************
 * reads.c from libdibol
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

#include "reads.h"
#include "filefonc.h"
#include <stdlib.h>
#include <string.h>

/*
	inputs the next available record in sequence from a file
	3-55
*/
int dbl_reads(variable* ch,variable* record)
{
	Channel* canal=findchannel(decimal2integer(ch));

	if (canal==NULL) error(11);

	/* Terminal */
	if (canal->terminal==1) {
		int key=0;
		char* buffer;
		int index=0;
		buffer=(char*)malloc(getsize(record)*sizeof(char));
		memset(buffer,32,getsize(record));
		do {
			key=getchar();
			if ((key==127) && (index!=0)) {
				buffer[index]=32;
				index--;
				putchar('\b');
				putchar(32);
				putchar('\b');
			}
			else if ((key!=127) && (key!='\n') && (index<getsize(record))) {
				buffer[index]=key;
				index++;
				putchar(key);
			} else if (key!='\n')
				putchar(7); /* Bell */
		} while (key!='\n');
		putchar('\n');
		memcpy(getdata(record),buffer,getsize(record));
		free(buffer);
		return 1;
	}

	/* Sequential file */
	if (canal->submode==SEQUENTIAL) {
		char* buffer=NULL;
		int len;
		int n=0;
		len=getline(&buffer,&n,canal->fh);
		if (len==-1) return 1;
		if (len-1>getsize(record)) {
			free(buffer);
			error(23);
		}
		memset(getdata(record),32,getsize(record));
		memcpy(getdata(record),buffer,len-1);
		free(buffer);
		return 0;
	}

	/* Relative file */
	if (canal->submode==RELATIVE) {
		char* query;
		MYSQL_RES *result;
		MYSQL_ROW row;
		unsigned long *lengths;
		/* SELECT MIN(t1.id+1) FROM %s AS t1 LEFT JOIN TEST AS t2 ON t1.id+1=t2.id WHERE t2.id IS NULL AND t1.id> %s */
		asprintf(&query,"SELECT * FROM %s WHERE id>%d LIMIT 1",canal->filespec,canal->lastid);
		if (mysql_query(dibol->dbh, query) != 0)
			error(22);
		free(query);
		result=mysql_store_result(dibol->dbh);
		if (mysql_num_rows(result) == 0) {
			mysql_free_result(result);
			return 1; /* no record from query, so end of file */
		}
		row = mysql_fetch_row(result);
		lengths = mysql_fetch_lengths(result);
		memcpy(getdata(record), row[1], lengths[1]); /* FIXME we don't check for record size */
		canal->lastid=strtol(row[0],NULL,10);
		mysql_free_result(result);
		return 0;
	}

	/* Indexed file */
	if (canal->submode==INDEXED) {
		char* query;
		MYSQL_RES *result;
		MYSQL_ROW row;
		char* buffer;
		MYSQL_FIELD *field2;
		int i=0;
		int len;
		unsigned long *lengths;
		asprintf(&query,"SELECT * FROM %s WHERE %s > %s LIMIT 1",canal->filespec,canal->lastkeyfld,canal->lastkey);
		mysql_query(dibol->dbh, query);
		free(query);
		result=mysql_store_result(dibol->dbh);
		if (mysql_num_rows(result) == 0) {
			mysql_free_result(result);
			return 1; /* no record from query, so end of file */
		}
        	/* Copy the record */
		row=mysql_fetch_row(result);
		lengths = mysql_fetch_lengths(result);
		buffer = (char *) malloc(sizeof(char));
		while (field2 = mysql_fetch_field(result)) {
			if (strcmp(field2->name,"id")==0) {
				canal->lastid=strtol(row[i],NULL,10);
				i++;
				continue;
			}
			if (strcmp(field2->name,canal->lastkeyfld)==0) {
				if (canal->lastkey!=NULL)
					free(canal->lastkey);
				canal->lastkey=strdup(row[i]);
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
		return 0;
	}
}

