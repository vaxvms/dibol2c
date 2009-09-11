/**************************************************************************
 * stack.c from libdibolcommon
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

/* function to manipulate stack */
/* stack are implemented with a simple linked list */

#include "stack.h"

void push(Stack** stack, void* element)
{
	*stack=g_slist_prepend(*stack,element);
}

void* pop(Stack** stack)
{
	void* element;
	element=g_slist_nth_data(*stack,0);
	*stack=g_slist_remove(*stack,element);
	return element;
}

void* top(Stack** stack)
{
	void* element;
	element=g_slist_nth_data(*stack,0);
	return element;
}
