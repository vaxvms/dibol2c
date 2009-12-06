/**************************************************************************
 * forms.c from libdibol
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

#include "forms.h"
#include "filefonc.h"
#include <stdlib.h>

/*
	FORMS function
	3-31
*/
void dbl_forms(variable* ch, variable* code)
{
	Channel* canal=findchannel(decimal2integer(ch));
	int64 controlcode=decimal2integer(code);
	char tmp[3];
	FILE* fh;

	if (canal->terminal==1)
		fh=stdout;
	else
		fh=canal->fh;

	if (controlcode==0) {
		tmp[0]=12;
		tmp[1]=0;
	} else if ( (controlcode >= 1) && (controlcode<=255)) {
		int i;
		tmp[0]=10;
		tmp[1]=13;
		tmp[2]=0;
		for (i=1; i<controlcode;i++)
			fprintf(fh,tmp);
		return;
	} else if (controlcode==-1) {
		tmp[0]=11;
		tmp[1]=0;
	} else if (controlcode==-3) {
		tmp[0]=13;
		tmp[1]=0;
	} else
		error(104);

	fprintf(fh,tmp);
}

