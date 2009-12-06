/**************************************************************************
 * open.c from libdibol
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
#include <stdlib.h>

/*
	OPEN
	3-44
*/
void dbl_open(variable* ch,int mode,int submode,variable* filespec,variable* alloc,
     variable* bktsiz, variable* blksiz, variable* bufsiz, variable* recsiz)
{
	Channel* canal=(Channel*)malloc(sizeof(Channel));

	/* canal number */
	canal->ch=decimal2integer(ch);

	canal->mode=mode;
	canal->submode=submode;

	/* file name */
	canal->filespec=alpha2char(filespec);

	/* terminal */
	if (strcmp(canal->filespec,"TT:")==0) {
		canal->terminal = 1;
		goto fin;
	}

	/* sequential file */
	if ((canal->submode==SEQUENTIAL) || (canal->submode==PRINT)) {
		if (canal->mode==INPUT)
			canal->fh=fopen(canal->filespec,"r");
		if (canal->mode==OUTPUT) {
			canal->fh=fopen(canal->filespec,"w");
			if (canal->fh==NULL)
				error(18);
		}
	}

	/* relative file */
	if (canal->submode==RELATIVE) {
		/* Create the file if needed */
		if (canal->mode==OUTPUT) {
			char* query;
			asprintf(&query,"DROP TABLE %s",canal->filespec);
			mysql_query(dibol->dbh,query);
			free(query);
			asprintf(&query,"CREATE TABLE %s (id int(11) not null primary key,data blob)",canal->filespec);
			mysql_query(dibol->dbh,query);
			free(query);
		}
		if ((canal->mode==INPUT) || (canal->mode==UPDATE)) {
			MYSQL_RES* result;
			result=mysql_list_tables(dibol->dbh,canal->filespec);
			if (mysql_num_rows(result)==0)
				error(18);
			mysql_free_result(result);
		}
	}

	/* indexed file */
	if (canal->submode==INDEXED) {
		if (canal->mode==OUTPUT) {
			/* in dibol 83, you can't create an indexed file */
			error(18);
		}
		if (canal->mode==INPUT) {
			MYSQL_RES* result;
			result=mysql_list_tables(dibol->dbh,canal->filespec);
			if (mysql_num_rows(result)==0)
				error(18);
			mysql_free_result(result);
		}
	}
fin:
	dibol->channels=g_slist_append(dibol->channels,canal);
}
