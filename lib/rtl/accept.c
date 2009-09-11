/**************************************************************************
 * accept.c from libdibol
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

#include "accept.h"
#include "filefonc.h"
#include <stdlib.h>

/*
	ACCEPT inputs a character from a terminal
	3-12
*/
int dbl_accept(variable* ch, variable* field)
{
	int key;
	Channel* canal=findchannel(decimal2integer(ch));

	if (canal==NULL) error(11);
	if (canal->terminal!=1) error(10);

	key=getchar();
	putchar(key);
	/* If you press ^D return 1 for goto */
	if ((key==4) && (gettype(field)<=tALPHA)) return 1;

	if (gettype(field)<=tALPHA)
		assign(field,imm(tALPHA,1,(char*)&key));
	if (gettype(field)==tDECIMAL)
		integer2decimal(field,key);

	return 0;
}
