/**************************************************************************
 * call.c from libdibol
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
#include "call.h"
#include <stdlib.h>

/* management of the call stack */
/* FIXME : we have function to deal with stack, we should use them */

void push_call()
{
	call_stack *layer;
	layer = (call_stack*)malloc(sizeof(call_stack));
	layer->super=_call;
	_call=layer;
	_call->xcall=0;
}

void pop_call()
{
	call_stack* super;
	jmp_buf buf;
	int xcall=_call->xcall;
	memcpy(buf,_call->ctx,sizeof(jmp_buf));
	super=_call->super;
	free(_call);
	_call=super;
	if (xcall==0)
		longjmp(buf,1);
}
