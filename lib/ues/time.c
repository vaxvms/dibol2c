/**************************************************************************
 * time.c from libdibolues
 *
 * (c) 2009 Anu				info@aksuda.com
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
#include <time.h>

void pTIME(variable* afield)
{
	time_t curtime;
	char* str=(char*)malloc(9*sizeof(char));
	int i;
	curtime=time(NULL);
	strftime(str,9,"%H:%M:%S",localtime(&curtime));
	memcpy(getdata(afield),str,getsize(afield));
	free(str);
}

