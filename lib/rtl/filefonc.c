/**************************************************************************
 * filefonc.c from libdibol
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
#include "filefonc.h"

static gint comparechannel(gconstpointer data,gconstpointer userdata)
{
	if ( ((Channel*)data)->ch==*(int*)userdata ) return 0;
	if ( ((Channel*)data)->ch>*(int*)userdata ) return 1;
	if ( ((Channel*)data)->ch<*(int*)userdata ) return -1;
}

Channel* findchannel(int noch)
{
	GSList* element=g_slist_find_custom(dibol->channels,&noch,&comparechannel);
	if (element==NULL) return NULL;
	else return element->data;
}
