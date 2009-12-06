/**************************************************************************
 * display.c from libdibol
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
#include "display.h"
#include "filefonc.h"
#include <stdlib.h>

void dbl_display(variable* ch, int count, ...)
{
	Channel* canal=findchannel(decimal2integer(ch));
	va_list arg;
	int i;
	variable* field;
	FILE* fh;

	if (canal->terminal==1)
		fh=stdout;
	else
		fh=canal->fh;

	va_start(arg,count);

	for (i = 0; i < count; i++) {
		field=va_arg(arg,variable*);
		if (gettype(field)<=tALPHA)
			fwrite(getdata(field),getsize(field),1,fh);
		if (gettype(field)==tDECIMAL) {
			int64 code;
			code=decimal2integer(field);
			code%=256;
			fprintf(fh,"%c",code);
		}
	}

	fflush(fh);

	va_end(arg);
}
