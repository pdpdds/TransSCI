/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	These are prototypes for functions described in utils.c
 * 
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>


 /* I/O helpers functions prototypes*/
 char *my_fgets (char *, int, FILE *);
 int my_fputs (const char *, FILE *);
 
 bool findText(char *, int &, char *, bool);
 


#endif //UTILS_H
