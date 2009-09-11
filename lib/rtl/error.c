/**************************************************************************
 * error.c from libdibol
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

typedef struct {
	char* message;
	int trappable;
	} ErrorEntry;

/* Array of error code. there is holes in the table as some
   code are not used.
*/
ErrorEntry errors[] = {
{0,0},
{"End of file",1},
{"RETURN but no CALL or XCALL",0},
{0,0},
{0,0},
{0,0},
{"Incorrect number of arguments",0},
{"Subscript error",1},
{"Writing into a literal",0},
{"Not enough memory",1},
{"Illegal channel number",0},		/* 10 */
{"Channel not open",0},
{"Input from write-only device",1},
{0,0},
{0,0},
{"Number too long",1},
{"DIBOL channel in use",0},
{"Bad file specification",1},
{"File not found",1},
{"Device handler not available",1},
{"Bad digit",1},			/* 20 */
{"Bad OPEN",1},
{"I-O error",1},
{"Line too long",1},
{"No space for file",1},
{"Output file full",1},
{"Field or record too long",1},
{0,0},
{"Illegal record number",1},
{0,0},
{"Divide by zero",1},			/* 30 */
{0,0},
{"Cannot supersede existing file",1},
{0,0},
{0,0},
{0,0},
{0,0},
{"Device in use",1},
{"File in use",1},
{"Output to read-only device",1},
{"Record locked",1},			/* 40 */
{0,0},
{0,0},
{"?M-Dir IO error",0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},					/* 50 */
{0,0},
{"Illegal key",1},
{"Key not same",1},
{"No duplicates",1},
{0,0},
{"Not ISAM file",1},
{0,0},
{"Job startup error",1},
{0,0},
{0,0},					/* 60 */
{"No current record",1},
{"Protection violation",1},
{0,0},
{"Record not found",1},
{0,0},
{"R6 Stack overflow",0},
{0,0},
{0,0},
{0,0},
{0,0},					/* 70 */
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{"Argument out of order",1},
{"Argument not defined in the record",1},
{"Incorrect argument count",1},
{"Field not packed",1},					/* 80 */
{0,0},
{0,0},
{0,0},
{"Illegal block I/O record size",1},
{0,0},
{0,0},
{"Argument missing",1},
{0,0},
{0,0},
{0,0},					/* 90 */
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},
{0,0},					/* 100 */
{0,0},
{0,0},
{"Invalid file organization",1},
{"Out of range",0},
{0,0},
{0,0},
{"Device not ready",1},
{"Invalid OPEN mode value",1},
{0,0},
{0,0},					/* 110 */
{0,0},
{"Error during file open",1},
{"Invalid ALLOC value in OPEN",1},
{"Invalid BKTSIZ value in OPEN",1},
{"Invalid BLKSIZ value in OPEN",1},
{"Invalid BUFSIZ value in OPEN",1},
{"Invalid RECSIZ value in OPEN",1},
{"Unable to open message manager mailbox",1},
{0,0},
{"Queue not available or invalid queue name",1},	/* 120 */
};

/* This is the handler for non-trappable errors
   it print an error message, exit the programm and 
   return the error code to the shell

   TODO : execute the action specifies by FATAL (5-8)

*/
void NTerror(int code)
{
	printf("Non-trappable error : %d\n%s\nBye bye blue sky...\n",code,errors[code].message);
	exit(code);
}

/* This function is called when there is an error.
   If this error is untrappable or there no ONERROR,
   it call NTerror.
   Otherwise, it jump to the ONERROR label
*/
void error(int code)
{
	dibol->errorhandle.code=code;
	if (dibol->errorhandle.active==0) {
		NTerror(code);
	}
	longjmp(dibol->errorhandle.ctx,code);
}

