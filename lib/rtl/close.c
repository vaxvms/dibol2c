/**************************************************************************
 * close.c from libdibol
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

#include "close.h"
#include "filefonc.h"
#include <stdlib.h>

/*
	Close an open channel
	3-18
*/
void
dbl_close (variable * ch)
{
  Channel *canal = findchannel (decimal2integer (ch));

  /* According to the documentation, no error
     is generated if the channel is not opened */
  if (canal == NULL)
    return;

  if ((canal->submode == SEQUENTIAL) && (!canal->terminal))
    {
      fclose (canal->fh);
    }
  if ((canal->submode == RELATIVE) || (canal->submode == INDEXED))
    {
    }

  dibol->channels = g_slist_remove (dibol->channels, canal);
}
