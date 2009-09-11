/**************************************************************************
 * wait.c from libdibolues
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

#include "wait.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

void pWAIT(variable* seconds, variable* parameters, variable* event)
{
	// FIXME
	// Wait only for terminal input or timeout
	fd_set *set=NULL;
	struct timeval *timeout=NULL;
	int64 iparameters=decimal2integer(parameters);
	int message,terminator,input,time;
	int result;

	/* Decode paramters */
	message=(iparameters/1000)!=0;
	iparameters%=1000;
	terminator=(iparameters/100)!=0;
	iparameters%=100;
	input=(iparameters/10)!=0;
	iparameters%=10;
	time=iparameters!=0;

	if (input) {
		set=(fd_set*)malloc(sizeof(fd_set));
		FD_ZERO(set);
		FD_SET(STDIN_FILENO, set);
	}

	if (time) {
		timeout=(struct timeval*)malloc(sizeof(struct timeval));
		timeout->tv_sec = decimal2integer(seconds);
		timeout->tv_usec = 0;
	}

	result=select(FD_SETSIZE,set,NULL,NULL,timeout);
	switch (result) {
		case 0: integer2decimal(event,1);
			break;
		case 1: integer2decimal(event,2);
			break;
	}
}
