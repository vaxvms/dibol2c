/**************************************************************************
 * store.c from libdibol
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

#include "store.h"
#include "filefonc.h"
#include <string.h>

void dbl_store(variable *ch, variable *record, variable *keyfld)
{
	Channel *canal = findchannel (decimal2integer (ch));
	if (canal == NULL)
		error(11);
	if (canal->submode == INDEXED) {
		char* query;
		char** dataquery;
		char* dataquerystr;
		char** tmp;
		MYSQL_RES *result;
		MYSQL_ROW row;
		int length;
		int offset=0;
		int keyfldoffset=getdata(keyfld)-getdata(record);

		/* build query according to data field */
		asprintf(&query,"SHOW COLUMNS FROM %s",canal->filespec);
		mysql_query(dibol->dbh,query);
		free(query);
		result=mysql_store_result(dibol->dbh);
		dataquery=(char**)malloc((mysql_num_rows(result))*sizeof(char*));
		memset(dataquery,0,mysql_num_rows(result));
		mysql_fetch_row(result); /* Pass id field */
		tmp=dataquery;
		while ( (row=mysql_fetch_row(result) )) {
			sscanf(row[1], "char(%d)", &length);
			*tmp=(char*)malloc((strlen(row[0])+length+4)*sizeof(char));
			memset(*tmp,0,strlen(row[0])+length+4);
			strcpy(*tmp,row[0]);
			strcpy(*tmp+strlen(row[0]),"=\"");
			memcpy(*tmp+strlen(row[0])+2,getdata(record)+offset,length);
			strcpy(*tmp+strlen(row[0])+length+2,"\"");
			offset+=length;
			if (offset==keyfldoffset) {
				/* Copy the last keyfield and keydata for reads */
				if (canal->lastkeyfld!=NULL)
					free(canal->lastkeyfld);
				canal->lastkeyfld=strdup(row[0]);
				if (canal->lastkey!=NULL)
					free(canal->lastkey);
				canal->lastkey=(char*)malloc((getsize(keyfld)+1)*sizeof(char));
				memcpy(canal->lastkey,getdata(keyfld),getsize(keyfld));
			}
			tmp++;
		}
		*tmp=NULL;

		mysql_free_result(result);

		/* Add data */
		dataquerystr=g_strjoinv(",",dataquery);
		asprintf(&query,"INSERT INTO %s SET %s",canal->filespec,dataquerystr);
		free(dataquerystr);
		mysql_query(dibol->dbh,query);
		free(query);
	}
}
