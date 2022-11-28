/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	Used to load a MSG from a RESOURCE type archive
 *
 *	TODO: extend to segmented Volumes
 */
 
#include "stdafx.h"
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <windows.h>
 #include <windowsx.h>


 #include <direct.h>

 #include "volume.hpp"
 #include "pkdcl.h"
 #include "lzs.h"
 #include "delzw.h"
 #include "language.h"
 #include "list.h"
 #include "resvol.h"
 #include "maindefs.h"
 
 #define LZW         0x01
 #define LZWMSB      0x02
 #define LZWMSBV256  0x03
 #define LZWMSBP256  0x04
 #define PKDCL       0x12
 #define PKDCL13     0x13
 #define PKDCL14     0x14
 #define LZS         0x20
 
 #define UNKNOWN     0x30
 
 #define MAPNAMESITEMS 5

 
 extern HINSTANCE hInst;
 //extern BOOL DoFileOpen(HWND, char *);

 
 char *mapnames[MAPNAMESITEMS] = {
      "RESOURCE","MESSAGE","ALT","ALTRES","RESMAP"
 };
 
 char *volnames[MAPNAMESITEMS] = {
      "RESOURCE","RESOURCE.MSG","RESOURCE.ALT","ALTRES.000","RESSCI"
 };
 
 
 char szMapFileName[MAX_PATH] = "";
 char mapfileversion = 0;
 List<PRESMAPIDSSCI32> *msglist=0;
 char szSelVolFileName[MAX_PATH] = "";
 long hasversionstamp=false;
 char szResFileName[MAX_PATH] = "";
 char resfileversion = 0;
 char compression=0;



LRESULT CALLBACK MessageSelect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndList; 
    HWND hwndOkButton; 
    long nItem=0;
          
	switch (message)
	{
      
	case WM_INITDIALOG:
        {
            hwndList = GetDlgItem(hDlg, IDC_VOLLIST); 
 
            // Initialize the list box (fill it). 
 
            unsigned short i = 0;
            while (msglist->getItem(i) != 0) 
            { 
                char temps[16];
                sprintf(temps, "%d",(*msglist->getItem(i))->id);
                ListBox_AddString(hwndList, temps);  
                i++;
            } 
            SetFocus(hwndList); 
            return FALSE; 
          
        }
		
	case WM_COMMAND:
		
        switch (LOWORD(wParam))
        {
         case IDC_VOLLIST: 
             switch (HIWORD(wParam)) 
             { 		
               case LBN_SELCHANGE: 
 
                 hwndOkButton = GetDlgItem(hDlg, IDOK); 
                 EnableWindow(hwndOkButton, true);
        
                 return TRUE; 
               case LBN_SELCANCEL: 
 
                 hwndOkButton = GetDlgItem(hDlg, IDOK); 
                 EnableWindow(hwndOkButton, false);
        
                 return TRUE;  
             }                        
             break;                   
         

         case IDOK:
          nItem = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_VOLLIST));             
          ListBox_GetText(GetDlgItem(hDlg, IDC_VOLLIST), nItem, szSelVolFileName); 
          
         case IDCANCEL:

          EndDialog(hDlg, LOWORD(wParam));
          return LOWORD(wParam);
          break; 
         
		}
		break;
	}
	return FALSE;
}

BOOL ExtractFromVolume(HWND hwnd, char *mapname, unsigned char type, char *exten, char *filename)
{   
       unsigned char fnoff, fnexten;
       
       if (!mapname)
       {
          OPENFILENAME ofn;
    
          ZeroMemory(&ofn, sizeof(ofn));
          //szFileName[0] = 0;

          ofn.lStructSize = sizeof(ofn);
          ofn.hwndOwner = hwnd;
          ofn.lpstrFilter =  INTERFACE_OPENVOLUMEMAPFILTER; 
          ofn.lpstrFile =  szMapFileName;
          ofn.nMaxFile = MAX_PATH;
    
          ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOVALIDATE;
    
          if (GetOpenFileName(&ofn)==0)
             return FALSE;
             
          fnoff = ofn.nFileOffset;
          fnexten = ofn.nFileExtension;
       } else {
         strcpy(szMapFileName, mapname);
         int i;
         for (i=strlen(szMapFileName)-1; i>=0; i--)
             if (szMapFileName[i]=='\\')
                break;
         fnoff = i+1;
         for (i=strlen(szMapFileName)-1; i>=0; i--)
             if (szMapFileName[i]=='.')
                break;
         fnexten = i+1;
       }
    
       RESMAPDIRSCI rmdsci;
       
       FILE *rmap = fopen(szMapFileName, "rb");
       if (rmap)
       {
          //////////////check if it is a SCI volume map file
          hasversionstamp = false;
          fread(&rmdsci, RESMAPDIRSCISIZE, 1, rmap);
          fseek(rmap, rmdsci.list -3, SEEK_SET);
          
          fread(&rmdsci, RESMAPDIRSCISIZE, 1, rmap);
          if (rmdsci.type != 0xFF)
          {
             hasversionstamp = true;
             fseek(rmap, -7, SEEK_CUR);
             fread(&rmdsci, RESMAPDIRSCISIZE, 1, rmap);
          }
          if (rmdsci.type == 0xFF)
          {
             fseek(rmap, 0, SEEK_END);
             if (rmdsci.list == ftell(rmap))
             {
                /////////////determine map file version
                fseek(rmap, 0, SEEK_SET);
                long listsize=0;
                mapfileversion=0;
                RESMAPDIRSCI rmdsci2;
                
                fread(&rmdsci, RESMAPDIRSCISIZE, 1, rmap);
                                              
                do
                {
                  
                  fread(&rmdsci2, RESMAPDIRSCISIZE, 1, rmap);
                  listsize=rmdsci2.list;
                  listsize-=rmdsci.list;
                  
                  if (listsize % RESMAPIDSSCI1SIZE == 0)
                     mapfileversion=-1;
                     
                  if (listsize % RESMAPIDSSCI32SIZE == 0)
                     mapfileversion+=1;
                        
                  rmdsci = rmdsci2;
                } while ((mapfileversion==0) && (rmdsci2.type!=0xFF));
                 
                /* char debugstr[256];
                  sprintf(debugstr, "debug   listsize = %d, mapfileversion = %d, rmdsci2.type = %X", listsize, mapfileversion, rmdsci2.type);
                   MessageBox(hwnd, debugstr, ERR_TITLE,
                           MB_OK | MB_ICONEXCLAMATION);   */
                           
                switch (mapfileversion)
                {
                 case 0:
                      MessageBox(hwnd, ERR_CANTFIGUREMAPFORMAT, ERR_TITLE,
                           MB_OK | MB_ICONEXCLAMATION);
                                
                      return FALSE;
                      //TODO rather than aborting, prompt user for a choice
                      break;
                 case -1:
                      mapfileversion=1; //sciold
                      break;
                 case 1:
                      mapfileversion=2; //scinew
                      break;
                }
                ////////////search for messages list
                fseek(rmap, 0, SEEK_SET);
                do
                {
                  fread(&rmdsci, RESMAPDIRSCISIZE, 1, rmap);
                } while(((rmdsci.type | 0x80) != (type | 0x80) ) && (rmdsci.type != 0xFF));
             
                if (rmdsci.type != 0xFF)
                {
                   fread(&rmdsci2, RESMAPDIRSCISIZE, 1, rmap);
                   fseek(rmap, rmdsci.list, SEEK_SET);
                   listsize = (rmdsci2.list - rmdsci.list) / (mapfileversion==1 ?RESMAPIDSSCI1SIZE :RESMAPIDSSCI32SIZE);
                   
                   msglist = new List<PRESMAPIDSSCI32>(listsize);
                   PRESMAPIDSSCI32 rmissci32;
                                                 
                   ///////read messages list in List object        
                   for (unsigned short i=0; i<listsize; i++)
                   {
                     rmissci32 = new RESMAPIDSSCI32;
                     fread(rmissci32, (mapfileversion==1 ?RESMAPIDSSCI1SIZE :RESMAPIDSSCI32SIZE), 1, rmap);  
                     if (mapfileversion==1)
                        rmissci32->offset = (rmissci32->offset &0x00FFFFFF) * 2;
                     
                     msglist->addItem(&rmissci32);
                     
                   }
                   //// prompt user for choice
                   int btn = DialogBox(hInst, (LPCTSTR)IDD_MSGSEL, hwnd, (DLGPROC)MessageSelect);

                   if ((btn == IDOK) && (szSelVolFileName[0]!=0))
                   {
                   
                                           
                      //char format[4];
                      //strcpy(format, szSelVolFileName+(strlen(szSelVolFileName)-3));
                      
                      //char number[32];
                      //strncpy(number, szSelVolFileName, strlen(szSelVolFileName)-4); 
                      //number[strlen(szSelVolFileName)-4]=0;   
                         
                      //long mindex = atol(number);
                      long mindex = atol(szSelVolFileName);
                        
                      unsigned short i=0;
                      while ((i < msglist->getLast()) && ((*msglist->getItem(i))->id != mindex))
                      {
                         i++;
                      }
                      
                      unsigned long location = (*msglist->getItem(i))->offset;
                      
                      ////try to determine resource volume file name
                              
                      szResFileName[0] = 0;
                      for (int j= 0; j<MAPNAMESITEMS; j++)
                          if (!strnicmp(szMapFileName+fnoff, mapnames[j], strlen(szMapFileName+fnoff)-4))
                          {   
                             if (!stricmp(volnames[j], "RESSCI")) 
                             {
                                if (fnexten>0)
                                   sprintf(szResFileName, "%s.%s", volnames[j], szMapFileName+fnexten);    
                             }
                             else if (!stricmp(volnames[j], "RESOURCE"))
                                  sprintf(szResFileName, "%s.%03d", volnames[j], location >> 28);                              
                             else
                                 strcpy(szResFileName, volnames[j]);  
                                                          
                             break; 
                          }
                         
                      if (szResFileName[0] == 0)
                      {
                          MessageBox(hwnd, VOLS_UNKNOWNVOLUMEFILENAME, ERR_TITLE,
                           MB_OK | MB_ICONEXCLAMATION);
                         //TODO prompt user rather than abort
                      }
                      if (szResFileName[0] != 0)
                      {   
                          FILE *rvol = fopen(szResFileName, "rb");
                          if (rvol)
                          {
                          
                             /////find out volume format and compression
                             resfileversion = 0;
                             
                             switch (mapfileversion)
                             {
                               case 1:
                                    resfileversion=1;
                                    compression=PKDCL;
                                    break;
                               case 2:
                                    if (hasversionstamp)
                                    {
                                       resfileversion = 2;
                                       compression = LZS;
                                    } 
                                    else
                                    {
                                       resfileversion = 1;
                                       compression = UNKNOWN;
                                                          
                                    }
                             
                                    break;
                             }
                             
                             ///////search into volume
                             fseek(rvol, location&0x0FFFFFFF, SEEK_SET);
                             
                             PRESVOLENTRYSCI32 rvolentry = new RESVOLENTRYSCI32;
                             PRESVOLENTRYSCI1 rvolentryold = (PRESVOLENTRYSCI1) rvolentry;
                             fread(rvolentry, (resfileversion==2 ?RESVOLENTRYSCI32SIZE : RESVOLENTRYSCI1SIZE), 1, rvol);
                             
                           
                             if (rvolentry->id == mindex)
                             {                          
                                //////file extraction 
                                                                  
                                void *out;
                                unsigned long outlen;
                                void *uncomp = 0;
                                void *comp = new unsigned char[(resfileversion==2 ?rvolentry->paklen:rvolentryold->paklen)];
   
                                fread(comp, (resfileversion==2 ?rvolentry->paklen:rvolentryold->paklen), 1, rvol);
                                if ((resfileversion==2 ?rvolentry->paklen:rvolentryold->paklen)
                                                       <
                                    (resfileversion==2 ?rvolentry->unplen:rvolentryold->unplen))
                                {
                                
     
                           
                                   uncomp = new unsigned char[(resfileversion==2 ?rvolentry->unplen:rvolentryold->unplen)];
                                   
                                   unsigned long ulen=0;
                                   
                                   if (compression == UNKNOWN)
                                      compression = rvolentryold->pakmeth;
                                      
                                   switch (compression)
                                   {
                                   
                                          case LZW:
                                               ulen = lzw_Decompress(comp, uncomp, rvolentryold->unplen, false); 
                                               break;
                                          case LZWMSB:
                                               ulen = lzw_Decompress(comp, uncomp, rvolentryold->unplen, true);
                                              
                                               break; 
                                          case PKDCL:
                                               ulen = dcl_unpack(comp, uncomp);
                                               break;
                                               
                                          case LZS:          
                                               ulen = lzs_unpack(comp, uncomp);
                                               break; 
                                          
                                              
      
                                                      
                                          case PKDCL13:
                                          case PKDCL14:                    
                                          case LZWMSBV256:
                                          case LZWMSBP256:     
                                          default:
                                                {
                                                  char tmes[256];
                                                  sprintf(tmes, VOLS_UNSUPPORTEDCOMPRESSION, compression);
                                                  MessageBox(hwnd, tmes, ERR_TITLE,
                                                                         MB_OK | MB_ICONEXCLAMATION);
                                                  
                                                }
                                                break;
                                   }                  
                                   
                                   if (ulen != (resfileversion==2 ?rvolentry->unplen:rvolentryold->unplen))
                                   {
                                      char unperror[256];
                                      sprintf(unperror, VOLS_ERRORUNPACKING, (resfileversion==2 ?rvolentry->unplen:rvolentryold->unplen), ulen);
                                      MessageBox(hwnd, unperror, ERR_TITLE,
                                               MB_OK | MB_ICONEXCLAMATION);
                                   }
                                   out = uncomp;
                                   delete [] comp;
                                   outlen = ulen;
                                }
                                else
                                {
                                    out = comp;
                                    delete [] uncomp;
                                    outlen = (resfileversion==2 ?rvolentry->paklen:rvolentryold->paklen); 
                                }
                                   
                                _mkdir("extracted");
                                char temp[MAX_PATH] = "extracted\\";
                                strcat(temp, szSelVolFileName);
                                strcpy(szSelVolFileName, temp);
                                strcat(szSelVolFileName, ".");
                                strcat(szSelVolFileName, exten);
                                FILE *outmsg = fopen(szSelVolFileName, "rb");
                                if (outmsg) //checking if file is already existing
                                {
                                   fclose(outmsg);
                                   outmsg = 0;
                                } else
                                  outmsg = fopen(szSelVolFileName, "wb");
                    
                                if (!outmsg) //ie volume locked or file already existing
                                {
                                   OPENFILENAME ofn2;
                                   
                                   ZeroMemory(&ofn2, sizeof(ofn2));

                                   ofn2.lStructSize = sizeof(ofn2);
                                   ofn2.hwndOwner = hwnd;
                                   ofn2.lpstrFilter = INTERFACE_SAVEGENERICFILTER;
                                   ofn2.nFilterIndex = 1;
                                   ofn2.lpstrFile = szSelVolFileName;
                                   ofn2.nMaxFile = MAX_PATH;
                                   ofn2.lpstrDefExt = exten; //"trn";

                                   ofn2.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
                                             OFN_OVERWRITEPROMPT;
         
                                   if (GetSaveFileName(&ofn2))
                                      outmsg = fopen(szSelVolFileName, "wb");
                                
                                }
                                
                                if (outmsg)
                                {   
                                    ///////////writing the buffer to file
                                    //support for strange SCIPATCHes like those from sciresdump
                                    switch(type&0x7f)
									{
									case 1:
										{
											unsigned long patchID = 0x00008181;
											fwrite(&patchID,4,1,outmsg);

											break;
										}
									case 0:
										{	
											unsigned long patchID = 0x008080;
											fwrite(&patchID,4,1,outmsg);
											unsigned short tshort=320;
											fwrite(&tshort,2,1,outmsg);
											tshort=200;
											fwrite(&tshort,2,1,outmsg);
											tshort=5;
											fwrite(&tshort,2,1,outmsg);
											tshort=6;
											fwrite(&tshort,2,1,outmsg);
											tshort=256;
											fwrite(&tshort,2,1,outmsg);		
											tshort=0;

											for (int i=0;i<6;i++)	
												fwrite(&tshort,2,1,outmsg);

										break;
										}
									default:
										fputc(type, outmsg);
										fputc(0, outmsg);
									}
									
									
                                    ////////////////////
                                    fwrite(out, 1, outlen, outmsg);
                        
                                    fclose(outmsg);
                         
                                    delete [] out;
                                    
                                    if (filename) //if the user passed a in/out string in the parameters list
                                       strcpy(filename, szSelVolFileName);    
                                     
                                    fclose(rvol);
                                    
                                    fclose(rmap);
                   
                                    delete msglist;
                   
                                    return TRUE;
                                }
                               
                             }
                             else
                             {
                                 MessageBox(hwnd, VOLS_CANTFINDFILEIDINVOLUME, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION); 
                             } 
                             
                             fclose(rvol); 
                          } 
                          else
                          {
                              char errorvol[256];
                              sprintf(errorvol, ERR_CANTFINDVOL, szResFileName);
                              MessageBox(hwnd, errorvol, ERR_TITLE,
                                               MB_OK | MB_ICONEXCLAMATION); 
                           
                          } 
                      }
                         
                   } 
                                 
                   fclose(rmap);
                   
                   delete msglist;
                   
                   return FALSE;
                } else
                {
                   MessageBox(hwnd, ERR_NOMSGINMAPFILE, ERR_TITLE,
                           MB_OK | MB_ICONEXCLAMATION);
                           
                   fclose(rmap);          
                   return FALSE;
                }
             }
          }
          
          MessageBox(hwnd, ERR_NOTAMAPFILE, ERR_TITLE,
                           MB_OK | MB_ICONEXCLAMATION);      
          fclose(rmap);
       }

    return FALSE;
}
