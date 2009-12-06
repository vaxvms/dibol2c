/*
	This file contain function to convert VMS filespec
	to UNIX one. They have been stolen from dnprogs-2.07
	by P.J. Caulfield.

*/
#ifndef FILENAME_H
#define FILENAME_H

// In order to compile with as less change as possible
#define bool int
#define true 1
#define false 0
static int verbose=5;
static const char *sysdisk_name="SYSDISK";
// ***************************************************

bool is_vms_name(char *name);
void split_filespec(char *volume, char *directory, char *file);
void make_vms_filespec(const char *unixname, char *vmsname, bool full);
void parse_vms_filespec(char *volume, char *directory, char *file);
void make_unix_filespec(char *unixname, char *vmsname);
void convert_vms_wildcards(char *filespec);

#endif
