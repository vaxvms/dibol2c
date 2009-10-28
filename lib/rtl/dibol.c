/**************************************************************************
 * dibol.c from libdibol
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

void freeDibol();

/* Initialise the run-time environnement */
void initDibol(char *programname)
{
    struct termios tattr;
    struct sockaddr_un name;
    size_t size;

    dibol = (Dibol *) malloc(sizeof(Dibol));

    atexit(freeDibol);

    /* init the terminal */
    if (!isatty(STDIN_FILENO)) {
	fprintf(stderr, "Not a terminal.\n");
	exit(EXIT_FAILURE);
    }

    tcgetattr(STDIN_FILENO, &(dibol->saved_attributes));

    tcgetattr(STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON | ECHO);	/* Clear ICANON and ECHO. */
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);

    /* open connection to the SQL server */
    dibol->dbh = mysql_init(NULL);
    if (!mysql_real_connect
	(dibol->dbh, NULL, NULL, NULL, "dibol", 0, NULL, 0)) {
	fprintf(stderr, "Failed to connect to database: Error: %s\n",
		mysql_error(dibol->dbh));
    }

    /* run-time parameters at startup */
    dibol->flags = 0;

return;

    /* Open a socket unix local socket for
       communication between dibol programs */
    dibol->sock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (dibol->sock < 0) {
	perror("socket");
	exit(EXIT_FAILURE);
    }
    name.sun_family = AF_LOCAL;
    strncpy(name.sun_path, programname, sizeof(name.sun_path));
    size = SUN_LEN(&name);
    if (bind(dibol->sock, (struct sockaddr *) &name, size) < 0) {
	perror("bind");
	// exit(EXIT_FAILURE);
    }
}

/*
   free the run-time environnement
   this function is called automatically at exit
*/
void freeDibol()
{
    /* restore the terminal parameters */
    tcsetattr(STDIN_FILENO, TCSANOW, &(dibol->saved_attributes));

    /* Close the database connection */
    mysql_close(dibol->dbh);

    /* Close the socket */
    close(dibol->sock);

    /* Close open channels */
    /* TODO */

    free(dibol);
}
