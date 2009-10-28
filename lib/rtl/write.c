/**************************************************************************
 * write.c from libdibol
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
#include <string.h>

void
dbl_write (variable * ch, variable * record, variable * exp)
{
  Channel *canal = findchannel (decimal2integer (ch));
  if (canal == NULL)
    error (11);
  if (canal->submode == RELATIVE)
    {
      char *query;
      char *data = alpha2char (record);
      int seqnum = decimal2integer (exp);
      asprintf (&query, "REPLACE INTO %s (id,data) VALUES (%d,\"%s\")",
		canal->filespec, seqnum, data);
      if (mysql_query (dibol->dbh, query)!=0) {
      	free (data);
      	free (query);
	error(22);
      }
      canal->lastid=seqnum;
      free (data);
      free (query);
    }
  if (canal->submode == INDEXED)
    {
	char* query;
	char** dataquery;
	char** tmp;
	int length;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char* dataquerystr;
	int offset=0;
	int keyfldoffset=getdata(exp)-getdata(record);

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
			canal->lastkey=(char*)malloc((getsize(exp)+1)*sizeof(char));
			memcpy(canal->lastkey,getdata(exp),getsize(exp));
		}
		tmp++;
	}
	*tmp=NULL;
	dataquerystr=g_strjoinv(",",dataquery);
	    asprintf(&query,"UPDATE %s SET %s WHERE id=%d",canal->filespec,dataquerystr,canal->lastid);
	    mysql_query(dibol->dbh, query);
	    free(query);
    }
}
