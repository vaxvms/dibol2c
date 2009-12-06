/*
	This file has been lamed from fal/task.cc and libdap/connection.cc
	of dnprogs-2.07.
	As a result, credits should go to P.J. Caulfield

	Nicolas DUPEUX
*/

/******************************************************************************
    (c) 1998-2000 P.J. Caulfield               patrick@tykepenguin.cix.co.uk
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 ******************************************************************************
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <syslog.h>
#include <pwd.h>
#include <grp.h>
#include <glob.h>
#include <regex.h>
#include <string.h>

#include "filename.h"

void makeupper(char *s)
{
	unsigned int i;
	for (i=0; i<strlen(s); i++) s[i] = toupper(s[i]);
}

void makelower(char *s)
{
	unsigned int i;
	for (i=0; i<strlen(s); i++) s[i] = tolower(s[i]);
}



// A test to see if the file is a VMS-type filespec or a Unix one
bool is_vms_name(char *name)
{
    if ( (strchr(name, '[') && strchr(name, ']')) ||
	  strchr(name, ';') || strchr(name, ':') )
	return true;
    else
	return false;
}

// Splits a filename up into volume, directory and file parts.
// The volume and directory are just for display purposes (they get sent back
// to the client). file is the (possibly) wildcard filespec to use for 
// searching for files. 
//
// Unix filenames are just returned as-is.
//
// volume and directory are assumed to be long enough for PATH_MAX. file
// also contains the input filespec.
//
void split_filespec(char *volume, char *directory, char *file)
{
    if (is_vms_name(file))
    {
	// This converts the VMS name to a Unix name and back again. This
	// involves calling parse_vms_filespec twice and ourself once more.
	// YES THIS IS RIGHT! We need to make sense of the input file name
	// in our own terms and then send back our re-interpretation of
	// the input filename. This is what any sensible operating
	// system would do. (VMS certainly does!)

	// Convert it to a Unix filespec
	char unixname[PATH_MAX];
	char vmsname[PATH_MAX];

	make_unix_filespec(unixname, file);

	// Parse that Unix name
	strcpy(file, unixname);
	split_filespec(volume, directory, file);

	if (verbose > 1)
	    fprintf(stderr, "Unix filespec is %s\n", unixname);

	// Then convert it back to VMS
	make_vms_filespec(unixname, vmsname, true);

	if (verbose > 1)
	    fprintf(stderr, "VMS filespec is %s\n", vmsname);

	// Split up the VMS spec for returning in bits
	parse_vms_filespec(volume, directory, vmsname);

	// Make sure we set this back to true after we called ourself with a
	// Unix filespec
	return;
    }

    // We don't fill in the volume for unix filespecs
    volume[0] = '\0';
    directory[0] = '\0';
}

// Convert a Unix-style filename to a VMS-style name
// No return code because this routine cannot fail :-)
void make_vms_filespec(const char *unixname, char *vmsname, bool full)
{
    char        fullname[PATH_MAX];
    int         i;
    char       *lastslash;
    struct stat st;
    int slashes = 0;
    char *slash2;
    
    // Resolve all relative bits and symbolic links
    realpath(unixname, fullname);

    // Find the last slash in the name
    lastslash = fullname + strlen(fullname);
    while (*(--lastslash) != '/') ;

    // If the filename has no extension then add one. VMS seems to
    // expect one as does dapfs.
    if (!strchr(lastslash, '.'))
        strcat(fullname, ".");
    
    // If it's a directory then add .DIR;1
    if (lstat(unixname, &st)==0 && S_ISDIR(st.st_mode))
    {
        // Take care of dots embedded in directory names (/etc/rc.d)
        if (fullname[strlen(fullname)-1] != '.')
	    strcat(fullname, ".");
	
        strcat(fullname, "DIR;1"); // last dot has already been added
    }
    else // else just add a version number unless the file already has one
    {
	if (!strchr(fullname, ';'))
	    strcat(fullname, ";1");
    }

    // If we were only asked for the short name then return that bit now
    if (!full)
    {
	i=strlen(fullname);
	while (fullname[i--] != '/') ;
	strcpy(vmsname, &fullname[i+2]);

	// Make it all uppercase
	makeupper(vmsname);
	return;
    }

    // Count the slashes. If there is one slash we emit a filename like:
    // SYSDISK:[000000]filename
    // For two we use:
    // SYSDISK:[DIR]filename
    // for three or more we use:
    // DIR:[DIR1]filename
    // and so on...
    
    // Oh, also make it all upper case for VMS's benefit.
    for (i=0; i<(int)strlen(fullname); i++)
    {
	if (islower(fullname[i])) fullname[i] = toupper(fullname[i]);
	if (fullname[i] == '/')
	{
	    slashes++;
	}
    }
    
    // File is in the root directory
    if (slashes == 1)
    {
	sprintf(vmsname, "%s:[000000]%s", sysdisk_name, fullname+1);
	return;
    }

    // File is in a directory immediately below the root directory
    if (slashes == 2)
    {
	char *second_slash = strchr(fullname+1, '/');

	*second_slash = '\0';

	strcpy(vmsname, sysdisk_name);
	strcat(vmsname, ":[");
	strcat(vmsname, fullname+1);
	strcat(vmsname, "]");
	strcat(vmsname, second_slash+1);

	return;
    }

    // Most user filenames end up here
    slash2 = strchr(fullname+1, '/');

    *slash2 = '\0';
    strcpy(vmsname, fullname+1);
    strcat(vmsname, ":[");

    // Do the directory depth
    lastslash = slash2;

    for (i=1; i<slashes-1; i++)
    {
	char *slash = strchr(lastslash+1, '/');
	if (slash)
	{
	    *slash = '\0';
	    strcat(vmsname, lastslash+1);
	    strcat(vmsname, ".");
	    lastslash = slash;
	}
    }
    vmsname[strlen(vmsname)-1] = ']';
    strcat(vmsname, lastslash+1);
}

// Split out the volume, directory and file portions of a VMS file spec
// We assume that the VMS name is (quite) well formed.
void parse_vms_filespec(char *volume, char *directory, char *file)
{
    char *colon = strchr(file, ':');
    char *ptr = file;
    char *enddir;

    volume[0] = '\0';
    directory[0] = '\0';

    if (colon) // We have a volume name
    {
	char saved = *(colon+1);
	*(colon+1) = '\0';
	strcpy(volume, file);
	ptr = colon+1;
	*ptr = saved;
    }
    
    enddir = strchr(ptr, ']');
    if (*ptr == '[' && enddir) // we have a directory
    {
	char saved = *(enddir+1);

	*(enddir+1) = '\0';
	strcpy(directory, ptr);
	ptr = enddir+1;
	*ptr = saved;
    }
    
    // Copy the rest of the filename using memmove 'cos it might overlap
    if (ptr != file)
	memmove(file, ptr, strlen(ptr)+1);
    
}

// Convert a VMS filespec into a Unix filespec
// volume names are turned into directories in the root directory
// (unless they are SYSDISK which is our pseudo name)
void make_unix_filespec(char *unixname, char *vmsname)
{
    char volume[PATH_MAX];
    char dir[PATH_MAX];
    char file[PATH_MAX];
    int  ptr;
    int  i;
    char *semi;
    char *basedir;

    strcpy(file, vmsname);

    // Remove the trailing version number
    semi = strchr(file, ';');
    if (semi) *semi = '\0';

    // If the filename has a trailing dot them remove that too
    if (file[strlen(file)-1] == '.')
        file[strlen(file)-1] = '\0';
    
    unixname[0] = '\0';

    /* We add the environement variable DIBOLBASEDIR
     * it at the beginning of the unix filespec */
    if ((basedir=getenv("DIBOLBASEDIR"))!=NULL)
        strcat(unixname,basedir);

    // Split it into its component parts
    parse_vms_filespec(volume, dir, file);

    // Remove the trailing colon from the volume name
    if (volume[strlen(volume)-1] == ':')
	volume[strlen(volume)-1] = '\0';
    
    // If the filename has the dummy SYSDISK volume then start from the
    // filesystem root
    if (strcasecmp(volume, sysdisk_name) == 0)
    {
	strcat(unixname, "/");
    }
    else
    {
	if (volume[0] != '\0')
	{
	    strcat(unixname, "/");
	    strcat(unixname, volume);
	}
    }
    ptr = strlen(unixname);
    
    // Copy the directory
    for (i=0; i< (int)strlen(dir); i++)
    {
	// If the directory name starts [. then it is relative to the
	// user's home directory and we lose the starting slash
	// If there is also a volume name present then it all falls
	// to bits but then it's pretty dodgy on VMS too.
	if (dir[i] == '[' &&
	    dir[i+1] == '.')
	{
	    i++;
	    ptr = 0;
	    continue;
	}

	if (dir[i] == '[' ||
	    dir[i] == ']' ||
	    dir[i] == '.')
	{
	    unixname[ptr++] = '/';
	}
	else
	{
	    // Skip root directory specs
	    if (dir[i] == '0' && strncmp(&dir[i], "000000", 6))
	    {
		i += 5;
		continue;
	    }
	    if (dir[i] == '0' && strncmp(&dir[i], "0,0", 3))
	    {
		i += 2;
		continue;
	    }
	    unixname[ptr++] = dir[i];
	}
    }
    unixname[ptr++] = '\0'; // so that strcat will work properly

    // A special case (ugh!), if VMS sent us '*.*' (maybe as part of *.*;*)
    // then change it to just '*' so we get all the files.
    if (strcmp(file, "*.*") == 0) strcpy(file, "*");

    strcat(unixname, "/");
    strcat(unixname, file);

    // Finally convert it all to lower case. This is not the greatest way to
    // cope with it but because VMS will upper-case everything anyway we
    // can't really distinguish case. I know samba does fancy stuff with
    // matching various combinations of case but I really can't be bothered.
    makelower(unixname);

    // If the name ends in .dir and there is a directory of that name without
    // the .dir then remove it (the .dir, not the directory!)
    if (strstr(unixname, ".dir") == unixname+strlen(unixname)-4)
    {
	char dirname[strlen(unixname)+1];
	struct stat st;
	char *ext;

	strcpy(dirname, unixname);
	ext = strstr(dirname, ".dir");
	if (ext) *ext = '\0';
	if (stat(dirname, &st) == 0 &&
	    S_ISDIR(st.st_mode))
	{
	    char *ext = strstr(unixname, ".dir");
	    if (ext) *ext = '\0';
	}
    }
}

// Convert VMS wildcards to Unix wildcards
// In fact all this does is substitute ? for % in the string.
// elipses are NOT catered for because there is no Unix equivalent.
// This is NOT done in the normal VMS->Unix conversion for all sorts of
// complicated reasons.
void convert_vms_wildcards(char *filespec)
{
    unsigned int i = strlen(filespec);

    while (i--)
    {
	if (filespec[i] == '%') filespec[i] = '?';
    }
}

