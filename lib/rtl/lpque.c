/**************************************************************************
 * lpque.c from libdibol
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

#include "lpque.h"
#include <stdlib.h>
#include <unistd.h>

/*
	LPQUE
	3-40
*/
void dbl_lpque(variable* filespec,variable* lpnum,variable* copies)
{
	char* commandline;
	int nbcopies=1;
	int lpid=0;
	char* filename;

	filename=(char*)calloc(0,(getsize(filespec)+1)*sizeof(char));
	memcpy(filename,getdata(filespec),getsize(filespec));
	if ( access(filename,R_OK | F_OK ) !=0)
		error(18);

	if (lpnum!=NULL)
		lpid=decimal2integer(lpnum);
	if (copies!=NULL)
		nbcopies=decimal2integer(copies);

	asprintf(&commandline,"lpr -#%d -Plp%d %s",nbcopies,lpid,filename);

	if ( system(commandline)!=0 )
		error(120);

	free(commandline);
	free(filename);
}

