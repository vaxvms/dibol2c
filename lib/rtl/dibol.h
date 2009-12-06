#ifndef DIBOL_H
#define DIBOL_H

#define _GNU_SOURCE 1

#include "variables.h"
#include "open.h"
#include "operators.h"
#include "stack.h"
#include "error.h"

#include "accept.h"
#include "call.h"
#include "clear.h"
#include "close.h"
#include "decr.h"
#include "delete.h"
#include "display.h"
#include "forms.h"
#include "incr.h"
#include "locase.h"
#include "lpque.h"
#include "read.h"
#include "reads.h"
#include "send.h"
#include "sleep.h"
#include "stop.h"
#include "store.h"
#include "write.h"
#include "writes.h"

#include <termio.h>
#include <unistd.h>
#include <mysql/mysql.h>


/*
	In this structure, we put the information
	about the runtime context.
	All that must be global
*/
typedef struct {
	GSList* channels;			/* List of open channels */
	Errorhandle errorhandle;		/* error handler */
	struct termios saved_attributes;	/* Terminal setting */
	int sock;				/* socket descriptor for communication */
	MYSQL* dbh;				/* connection to the mysql server */
	int flags;				/* run-time system parameters */
} Dibol;

Dibol* dibol;

/* Initialise the runtime environnement */
void initDibol(char* programname);
void freeDibol();

#endif
