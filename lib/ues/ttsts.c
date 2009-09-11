/**************************************************************************
 * ttsts.c from libdibolues
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

#include "ttsts.h"
#include "filefonc.h"

void pTTSTS(variable* nfield,variable* ch)
{
	// FIXME You must provide a channel number
	Channel *canal=findchannel(decimal2integer(ch));
	int nbcar;
	if ((!canal->terminal) || (canal==NULL)) {
		integer2decimal(nfield,0);
		return;
	}
	nbcar=read(fileno(canal->fh),NULL,0);
	integer2decimal(nfield,nbcar);
}
