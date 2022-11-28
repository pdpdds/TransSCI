/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2021.
 *
 *	This is the header file for main.cpp
 *	
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <commctrl.h>

#include "maindefs.h"
#include "language.h"
#include "utils.h"
#include "list.h"
#include "message.h"


///////////////////////////////////////////
 /* Stuff missing from the Windows headers */
 
#define LVS_EX_LABELTIP 0x00004000
#define LVS_EX_ONECLICKACTIVATE 0x00000040
#define LVS_EX_FULLROWSELECT 0x00000020
//#define LVN_ITEMACTIVATE (LVN_FIRST-14)
//#define LVM_SETEXTENDEDLISTVIEWSTYLE 0x00001036
//#define LVS_EX_BORDERSELECT 0x00008000
//#define LVS_EX_GRIDLINES 0x1

/*
typedef struct tagNMCUSTOMDRAWINFO {
    NMHDR hdr;
    DWORD dwDrawStage;
    HDC hdc;
    RECT rc;
    DWORD dwItemSpec;
    UINT uItemState;
    LPARAM lItemlParam;
} NMCUSTOMDRAW, *LPNMCUSTOMDRAW;

typedef struct tagNMLVCUSTOMDRAW {
    NMCUSTOMDRAW nmcd;
    COLORREF clrText;
    COLORREF clrTextBk;
#if (_WIN32_IE >= 0x0400)
    int iSubItem;
#endif
#if (_WIN32_IE >= 0x560)
    DWORD dwItemType;
    // Item Custom Draw	
    COLORREF clrFace;
    int iIconEffect;
    int iIconPhase;
    int iPartId;
    int iStateId:
    // Group Custom Draw
    RECT rcText;
    UINT uAlign;
#endif
} NMLVCUSTOMDRAW, *LPNMLVCUSTOMDRAW;
*/

//#define CDRF_NOTIFYITEMDRAW 0x20
//#define CDIS_FOCUS 16 
//#define CDIS_SELECTED 1

//////////////////////////////////////////



 /***** Function Prototypes *****/


 /* Declare Windows procedures */
 LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
 LRESULT CALLBACK WindowBarProcedure(HWND, UINT, WPARAM, LPARAM);
 LRESULT CALLBACK WindowTextProcedure(HWND, UINT, WPARAM, LPARAM);
 LRESULT CALLBACK WinStatusBarProcedure(HWND, UINT, WPARAM, LPARAM);
 WNDPROC g_pfnWndProcOrig;

 BOOL DoFileOpen(HWND, char *, bool);
 BOOL DoFileSave(HWND);
 BOOL DoFileSaveAs(HWND);
 BOOL DoFileExport(HWND);

 LRESULT CALLBACK SearchStatus(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
 LRESULT CALLBACK AboutBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


 /* Program specific procedures */ 
 extern bool LoadFile(HWND, LPSTR, MsgData **);
 extern bool SaveFile(HWND, LPSTR, bool);
 extern bool ExportFile(HWND, LPSTR);
 extern bool ImportTextFile(HWND hwnd, MsgData *mdo);
 
 void versionString(char *);
 void updateStrIndex(HWND, unsigned short);

 bool exit_proc(HWND);  //called on exit for confirmation dialog


 //extern BOOL ExtractFromVolume(HWND hwnd, char *mapname, unsigned char type, char *exten, char *filename);
 
 extern void DrawNounGraph(HWND hwnd, int sorting, bool colors);
 
 extern bool AddExCloneToAudMAP(HWND hwnd, SortedMsg *cloneID, SortedMsg *origID, ProgramAudMap *);
 extern bool ExportAudMAP(HWND hwnd, ProgramAudMap *padmap, char *filename);
 extern ProgramAudMap *ImportAudMAP(HWND hwnd, char **filename);
 extern void VerifyAudMap(HWND hwnd, MsgData *cmpObj, char *filename);
 extern bool ValidateExternalMAP(HWND hwnd, ProgramAudMap *intmap, char *filename);
 extern void RemoveZeroCloneFromAudMAP(HWND hwnd, SortedMsg *cloneID, SortedMsg *origID, ProgramAudMap *map);

 extern int addWordToWL(char *lookupfile, char *originalStr, char *translatedStr);
 extern bool appendWordToWL(char *lookupfile, char *originalStr, char *translatedStr);
 extern void doWordsLookup(HWND hwnd, char *lookupfile);
 
 extern int LoadNounsFromHEAP(HWND hwnd);
 
 extern bool ImportLabels(HWND hwnd, char *importfile);
 extern void ExportLabels(HWND hwnd, char type, char *exportfile); 
 
#endif //MAIN_H
