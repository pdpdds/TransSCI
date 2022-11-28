/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	This code header is used to define common headers include
 *	
 */

#ifndef MAINCOMMON_H
#define MAINCOMMON_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "utils.h"
#include "language.h"
#include "message.h"
//#include "maps.hpp"

#ifdef _MSC_VER 
 //not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

 /* That's the object which stores all MSG data in main.h */
 
 extern MsgData *msgObj;
 extern HWND hwndListV;
 typedef BOOL (*Func)(HWND, char *, unsigned char, char *, char *);
 extern Func ExtractFromVolume;
 
 //NOTE: this is a cheap solution of course, but the MsgData class
 //      could as well be reused in a new program which allows multiple windows
 //      (MsgData has no dependencies from the interface).

 /* other stuff from main*/
 
 extern char szAppDir[MAX_PATH], szFileName[MAX_PATH];
 extern void updateStrIndex(HWND hwnd, unsigned short value);
 extern void versionString(char *vers); 
 extern void UpdateListLabels(int which, char *with, int type);  

#endif //MAINCOMMON_H
