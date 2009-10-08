#ifndef OPEN_H
#define OPEN_H

#include <glib.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include "dibol.h"

// open mode definition
#define INPUT	1
#define OUTPUT	2
#define APPEND	3
#define UPDATE	4

// open submode definition
#define SEQUENTIAL	8
#define PRINT		16
#define RELATIVE	32
#define INDEXED		64

// describe a channel
typedef struct {
	int ch;
	int mode;
	int submode;
	char* filespec;
	int alloc;
	int bktsiz;
	int blksiz;
	int bufsiz;
	int recsiz;
	
	FILE* fh;	// if the file is a regular one
	int lastid;	// id of the last fetched record (for indexed or relative file)
	char* lastkeyfld; // name of the last used key field
	char* lastkey;	// value of the last key
	int terminal;	// if the file is the terminal
} Channel;

void dbl_open(variable* ch,int mode, int submode,variable* filespec,variable* alloc,
     variable* bktsiz, variable* blksiz, variable* bufsiz, variable* recsiz);


#endif
