/**************************************************************************
 * locase.c from libdibol
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
#include "locase.h"
#include <ctype.h>

/*
	convert uppercase characters to lowercase charactecs
	according to DIBOL convention
	3-39
*/
void dbl_locase(variable* afield)
{
	int size=getsize(afield);
	char* data=getdata(afield);
	int i;

	for (i=0;i<size;i++) {
		if ((*data>=64) && (*data<=94))
			*data+=32;
		data++;
	}
}

