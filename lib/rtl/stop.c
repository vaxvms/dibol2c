/**************************************************************************
 * stop.c from libdibol
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

#include "stop.h"
#include "filename.h"
#include <stdlib.h>
#include <unistd.h>

void dbl_stop(variable* filespec)
{
	char* vmsprogram;
	char* unixprogram;
	if (filespec==NULL)
		exit(0);

	/* chain to a new program */
	vmsprogram=(char*)malloc((getsize(filespec)+1)*sizeof(char));
	memcpy(vmsprogram,getdata(filespec),getsize(filespec));
	if (is_vms_name(vmsprogram)) {
		unixprogram=(char*)malloc((getsize(filespec)+1)*sizeof(char));
		make_unix_filespec(unixprogram,vmsprogram);
	} else
		unixprogram=vmsprogram;
	if ( access(unixprogram,X_OK | F_OK ) !=0)
		error(18);
	freeDibol();
	execv(unixprogram,NULL);
}

