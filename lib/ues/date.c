/**************************************************************************
 * date.c from libdibolues
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
#include "ues.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

void pDATE(variable* afield)
{
	time_t curtime;
	char* str=(char*)malloc(10*sizeof(char));
	int i;
	curtime = time (NULL);
	strftime(str,10,"%d-%b-%y",localtime(&curtime));
	for (i=0;i<10;i++)
		str[i]=toupper(str[i]);
	memcpy(getdata(afield),str,getsize(afield));
	free(str);
}

