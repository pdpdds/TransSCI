/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	This code header is used to define Win controls constants
 *	
 */

#ifndef MAINDEFS_H
#define MAINDEFS_H



#define WIN_WIDTH 780
#define WIN_HEIGHT 570

 
 /* main window buttons */
#define BTNBACK_ID      1
#define BTNNEXT_ID      2
#define SCRLBAR_ID      5
#define BTNGO_ID       10
#define BTNFIND_ID     20
#define BTNREPLACE_ID  22
#define BTNPHRASE_ID   25
#define BTNEDIT_ID     26
#define BTNCASE_ID     28
//#define BTNFILES_ID    30 //TODO???

#define BTNUNCLONE_ID  40

#define BTNLISTV_ID 60

#define IDD_SEARCHDIALOG   130
#define IDD_UNCLONEDIALOG   150
#define IDD_CLONEDIALOG     151
#define IDC_CLONEFIELD      1510
#define IDD_ABOUTBOX        1000

#define IDC_LOADMSG         1010
#define IDC_LOADRES         1011
#define IDC_MANUAL          1012
#define IDC_CLOSEDLG        1013

/* menu bar stuff */
#define IDM_FILEOPEN       100
#define IDM_FILEOPENVOL    110
#define IDM_FILESAVE       101
#define IDM_FILESAVEAS     102
#define IDM_IMPORTFILE     103
#define IDM_IMPORTTEXT     104
#define IDM_EXPORTTEXT     105
#define IDM_COMPAREWITH     106

#define IDM_FILEABOUT      107

#define IDM_FILEEXIT       108

#define IDM_NOUNSFROMHEAP  180
#define IDM_LOADLABELSTXT  182

#define IDM_EXPORTNOUNS    183
#define IDM_EXPORTVERBS    184
#define IDM_EXPORTCASES    185
#define IDM_EXPORTTALKERS  186

#define IDM_PROPERTIES     120
#define IDM_CLONE          150

  /*
#define IDM_EDITUNDO       200     //NOTE: disabled because they're not working as expected
#define IDM_EDITCUT        201
#define IDM_EDITCOPY       202
#define IDM_EDITPASTE      203
                              

#define IDM_CHOOSEFONT     302  
*/
#define IDM_FIRST          301
#define IDM_LEFT           302
#define IDM_RIGHT          303
#define IDM_LAST           304


#define IDM_FIND           321
#define IDM_REPLACE        322


//#define IDM_SHOWUNKBYTES   328

#define IDM_ADDTOWL        330
#define IDM_DOWORDSLOOKUP  331 
#define IDM_SHOWWL         332

#define IDM_FONT           400
#define IDM_FONT14         414
#define IDM_FONT16         416
#define IDM_FONT18         418
#define IDM_FONT20         420
#define IDM_FONT22         422
#define IDM_FONT24         424
#define IDM_FONT26         426


    
#define IDM_DRAWGF         507
#define IDM_DRAWGFBW         508

    
#define IDM_GRAPH           600
#define IDM_DRAWNOUNGF      601
#define IDM_DRAWCASEGF      602
#define IDM_DRAWVERBGF      603
#define IDM_DRAWTALKERGF    604
#define IDM_DRAWINDEXGF     605

#define IDM_LOADMAP         701
#define IDM_EXPORTMAP       702
#define IDM_VERIFYMAP       705

#define IDM_PLAY            720
#define IDM_LOADAUDRES      722
#define IDM_AUTOPLAY        721

#define IDM_HELP            900

#define IDM_NEXTCOMMENT     312

#define IDD_MSGSEL         140
#define IDC_VOLLIST        141
  

#endif //MAINDEFS_H
