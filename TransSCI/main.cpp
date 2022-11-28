/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
*  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.                            
*
*	This is the main program file with all the Windoze stuff
*	
*/

//TODO there are MANY constants (particularly Winds and such) which are hard coded.
//     constants definitions might help for future interface upgrading

#include "main.h"

 HINSTANCE hInst;
 
 /* Make the class name into a global variable */
 char szClassName[ ] = "WindowsApp";

 /* Program windows definition */
 HWND hwndMain = 0;
 //HWND hwndTextFrame = 0;
 HWND hwndFrame = 0;
 HWND hwndPhrase = 0;
 HWND hwndEdit = 0;
 HWND hwndComment = 0;
 HWND hwndCommentsTag = 0;
 HWND hwndMessageTag = 0;
 HWND hwndTranslationTag = 0;
  
 HWND hwndBarFrame = 0;
 HWND hwndFindButton = 0;
 HWND hwndReplaceButton = 0;
 HWND hwndFindText = 0;
 HWND hwndReplaceText = 0;
 HWND hwndCaseButton = 0;
  //HWND hwndAllFilesButton = 0;//TODO???
 HWND hwndPhraseButton = 0;
 HWND hwndEditButton = 0;
 HWND hwndScrollBar = 0;
 
 HWND hwndNoun = 0;    
 HWND hwndVerb = 0;
 HWND hwndCase = 0;
 HWND hwndTalker = 0;

 HWND hwndNounLabel = 0;    
 HWND hwndVerbLabel = 0;
 HWND hwndCaseLabel = 0;
 HWND hwndTalkerLabel = 0;
 
  
 HWND hwndNounIndex = 0;    
 HWND hwndVerbIndex = 0;
 HWND hwndCaseIndex = 0;
 HWND hwndTalkerIndex = 0;
 
 HWND hwndListV = 0;
 
 HWND hwndStatusBar = 0;
 
 /* Handle of the menu */
 HMENU menu;              

 /* Menu accelerators table */
 HACCEL accelTable;

 /* Default Program Font for Edit Text fields */
 HFONT hfDefault, hfInterface, hfComments;

 /* Drawing Variables */
 HDC hdc;             /* A device context used for drawing */
 PAINTSTRUCT ps;      /* Also used during window drawing */
 RECT rc;             /* A rectangle used during drawing */
 
 
 long scrolllefttimer = 0, scrollrighttimer = 0;
 
 RECT windowRect, tempwindowRect;


 bool unkbytesdecodinginfo = false;
 
 bool autoplay = false;

 /* Variables used when doing search */
 
 bool searchoriginal = true;
 bool searchcase = false;
 
 /* Variables used for clones */
 
 long actualclone = -1;   
 bool seqclone = false;  
 
 /* File paths of the last loaded file */
 char szFileName[MAX_PATH] = "";
 char szFilePath[MAX_PATH] = "";
 char szTempFileName[MAX_PATH] = "";
 char szImpFileName[MAX_PATH] = "";
 char szCmpFileName[MAX_PATH] = "";
 char szFindFileName[MAX_PATH] = "";
 char szFirstFindFileName[MAX_PATH] = "";
 char szAppDir[MAX_PATH] = "";

 bool datasaved = true;
 
 /* That's the object which stores all MSG data */
 MsgData *msgObj = 0;



bool ImportAudMapStub(HWND hwnd)
{
    if (msgObj->getAudMap() != 0)
    {
          int btn = MessageBox (hwnd, MAPS_REPLACEEXISTING, WARN_ATTENTION, 
                                        MB_APPLMODAL | MB_ICONQUESTION | MB_OKCANCEL);
          switch (btn)
          {
            case IDOK:
              msgObj->setAudMapName("");
              break;
            case IDCANCEL:      
             default:
                 return false;    
          }

     }
     ProgramAudMap *pam = 0;
     char *tempp = (char *) msgObj->getAudMapName();
     char **newaudmapfile = (char **) &(tempp);
     pam = ImportAudMAP(hwnd, newaudmapfile);
     if (pam != 0)
     {
         msgObj->setAudMap(pam);
         char tempString[MAX_PATH];
         long pos;
	     for (pos = strlen(*newaudmapfile)-1; pos>=0; pos--) 
		    if ((*newaudmapfile)[pos] == '\\')
			   break;
         strcpy(tempString, &((*newaudmapfile)[pos+1]));
         msgObj->setAudMapName(tempString);
         datasaved = false;
         EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);
                       
         EnableMenuItem(menu, IDM_EXPORTMAP, MF_ENABLED);
         EnableMenuItem(menu, IDM_VERIFYMAP, MF_ENABLED);

         return true;
      }
  
      return false;  
}

void PlayAudio(HWND hwnd, bool silent)
{
     if (msgObj->getAudMap())
       {         
         MAPSAUD mapsaud;
         mapsaud.noun = msgObj->getNoun(msgObj->currentString());
         mapsaud.verb = msgObj->getVerb(msgObj->currentString());
         mapsaud.ncase = msgObj->getCase(msgObj->currentString());
         mapsaud.seq = msgObj->getSeq(msgObj->currentString());
  
         MAPSAUD *retmap = (msgObj->getAudMap())->getAudEntry(&mapsaud);
         if (retmap)
         {
           long myresaudpos = (msgObj->getAudMap())->getAudDataPosition(retmap); 
           
           char szAudFileName[MAX_PATH] = "RESOURCE.AUD";
                          
           char pfile[MAX_PATH] = "";
           sprintf(pfile, "%s\\RESOURCE_AUD.path", szFilePath); 
           FILE *inifile = fopen(pfile, "r");
           if (inifile)
           {
               fgets(szAudFileName, MAX_PATH, inifile);
               fclose(inifile);
           }
                          
                          
           FILE *cfilebuf = fopen(szAudFileName, "rb");
           if (cfilebuf)
           {
              fseek(cfilebuf, myresaudpos, SEEK_SET); 
              unsigned char header[9];  //only first 9
              fread((void *) header, 9, 1, cfilebuf);
                             
              if (!strncmp((char *)&(header[2]), "SOL", 4))
              {
                 long filesize;
                 fread((void *) &filesize, 4, 1, cfilebuf); 
                 filesize += header[1] +2;
                             
                 fseek(cfilebuf, myresaudpos, SEEK_SET);
                                
                 unsigned char *buffer = (unsigned char *) malloc(filesize);
                 fread((void *) buffer, 1, filesize, cfilebuf);
                                
                 char audfilepath[MAX_PATH];
                 sprintf(audfilepath, "%s\\0.aud", szAppDir);  
                 FILE *wfilebuf = fopen(audfilepath, "wb");
                 if (wfilebuf)
                 {
                   fwrite((void *) buffer, 1, filesize, wfilebuf);
                                  
                   fclose(wfilebuf);
                   free(buffer);
                                
                   //convert and play
                   char audfilepath[MAX_PATH];
                   sprintf(audfilepath, "%s\\Audio0.wav", szAppDir); 
                   remove(audfilepath);
                                                       
                   HANDLE phandle = 0;
                   SHELLEXECUTEINFO sei = { sizeof(sei) };
                   sei.fMask = 0;//SEE_MASK_NOCLOSEPROCESS;//SEE_MASK_NOASYNC;
                   sei.nShow = SW_HIDE; 
                   sei.lpVerb = "open";
                   sei.lpFile = "audio.bat";
                   sei.lpDirectory = szAppDir;
                   sei.hProcess = phandle;
                   ShellExecuteEx(&sei);
             
                   WaitForSingleObject(phandle, 20000);
                   CloseHandle(phandle);
                                  
                   Sleep(400);     //TODO must find a better solution for waiting for the audio file to be finally written by the filesystem
                                  
                   PlaySound(audfilepath, NULL, SND_FILENAME|SND_ASYNC|SND_NODEFAULT);

                 } else if (!silent)
                   MessageBox (hwnd, ERR_CANTSAVE, ERR_TITLE, MB_ICONEXCLAMATION | MB_OK); 
  
              } else if (!silent)
                 MessageBox (hwnd, AUDIO_NOTASOLFILE, ERR_TITLE, MB_ICONEXCLAMATION | MB_OK); 

              fclose(cfilebuf);
           } else if (!silent)
               MessageBox (hwnd, AUDIO_CANTFINDRESAUD, ERR_TITLE, MB_ICONEXCLAMATION | MB_OK); 

         } else if (!silent)
           MessageBox (hwnd, MAPS_CANTFINDENTRY, ERR_TITLE, MB_ICONEXCLAMATION | MB_OK); 

       } else  if (!silent)
         MessageBox (hwnd, AUDIO_MUSTLOADMAPFIRST, ERR_TITLE, MB_ICONINFORMATION | MB_OK); 

}


int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
     if (msgObj->findListIndex(lParam1) <  msgObj->findListIndex(lParam2))
         return -1;
     else
         return 1;

}


void SortMyListView(HWND hWnd, int newsorting)
{
     char colname[20];
     LV_COLUMN lvc;
     lvc.mask =  LVCF_TEXT;
     lvc.pszText = colname;
     lvc.cchTextMax = 20;
         
     for (int i = 0; i<6; i++)
     {      
            ListView_GetColumn(hwndListV, i, &lvc);
            strlwr(colname+1);
            ListView_SetColumn(hwndListV, i, &lvc);
     }
     
     ListView_GetColumn(hwndListV, newsorting, &lvc);
     strupr(colname);
     ListView_SetColumn(hwndListV, newsorting, &lvc);

     msgObj->sortBy(newsorting);
     ListView_SortItems(hwndListV, CompareFunc, 0);
     updateStrIndex(hWnd, msgObj->currentString());  

}


void UpdateListLabels(int which, char *with, int type)
{
           char temp[6];               
           if (with[0] == 0) //user deleted string
           {
              sprintf(temp, "%d", which);
              with = temp;
           }
           
           LV_ITEM lvi;
           lvi.pszText = with;
           lvi.mask = LVIF_TEXT;
                   
           switch (type)
           {
               case 1:
                           for (long i = 0; i<msgObj->getPhrasesCount(); i++)
                           {
                              if (msgObj->getNoun(i) == which)
                              {    
                                  lvi.iItem = i;
                                  lvi.iSubItem = 1;
                                  ListView_SetItem(hwndListV, &lvi);
                              }
                           }
                           break;   
               case 2:
                           for (long i = 0; i<msgObj->getPhrasesCount(); i++)
                           {
                              if (msgObj->getVerb(i) == which)
                              {    
                                  lvi.iItem = i;
                                  lvi.iSubItem = 2;
                                  ListView_SetItem(hwndListV, &lvi);
                              }
                           }
                           break; 
               case 3:
                           for (long i = 0; i<msgObj->getPhrasesCount(); i++)
                           {
                              if (msgObj->getCase(i) == which)
                              {    
                                  lvi.iItem = i;
                                  lvi.iSubItem = 3;
                                  ListView_SetItem(hwndListV, &lvi);
                              }
                           } 
                           break;
               case 5:
                           for (long i = 0; i<msgObj->getPhrasesCount(); i++)
                           {
                              if (msgObj->getTalker(i) == which)
                              {    
                                  lvi.iItem = i;
                                  lvi.iSubItem = 5;
                                  ListView_SetItem(hwndListV, &lvi);
                              }
                           } 
                           break;                          
            
           }                
}


void InsertListViewItem(long i, char *tempStr, char *tempStr2)
{
                char conv[100];
                LV_ITEM lvI;

                // Some code to create the list-view control.
                // Initialize LVITEM members that are common to all items.
                lvI.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
                lvI.state = 0; 
                lvI.stateMask = 0; 

                // Initialize LVITEM members that are different for each item. 

                lvI.iItem = 0;
                lvI.iImage = 0;
                lvI.iSubItem = 0;
   
                lvI.lParam = (LPARAM) i; //ci conserviamo l'INDEXFILE  
                //Value specific to the item. If you use the LVM_SORTITEMS message, the list-view control passes this value to the application-defined comparison function. You can also use the LVM_FINDITEM message to search a list-view control for an item with a specified lParam value.
   
                sprintf(conv, "%d", i+1);
                lvI.pszText = conv;
                ListView_InsertItem(hwndListV, &lvI);
                lvI.mask = LVIF_TEXT | LVIF_STATE; 

                int label = msgObj->getNoun(msgObj->findListIndex(i));
     
                if (msgObj->getNounName(label))
                   sprintf(conv, "%s", msgObj->getNounName(label));
                else
                    sprintf(conv, "%d", label);
     
                lvI.iSubItem = 1;
                ListView_SetItem(hwndListV, &lvI);

                label = msgObj->getVerb(msgObj->findListIndex(i));
     
                if (msgObj->getVerbName(label))
                   sprintf(conv, "%s", msgObj->getVerbName(label));
                else
                    sprintf(conv, "%d", label);
     
                lvI.iSubItem = 2;
                ListView_SetItem(hwndListV, &lvI); 
    
                label = msgObj->getCase(msgObj->findListIndex(i));
     
                if (msgObj->getCaseName(label))
                   sprintf(conv, "%s", msgObj->getCaseName(label));
                else
                    sprintf(conv, "%d", label);
     
                lvI.iSubItem = 3;
                ListView_SetItem(hwndListV, &lvI);
    
                sprintf(conv, "%d", msgObj->getSeq(msgObj->findListIndex(i)));
     
                lvI.iSubItem = 4;
                ListView_SetItem(hwndListV, &lvI);   
    
                label = msgObj->getTalker(msgObj->findListIndex(i));
     
                if (msgObj->getTalkerName(label))
                   sprintf(conv, "%s", msgObj->getTalkerName(label));
                else
                    sprintf(conv, "%d", label);
     
                lvI.iSubItem = 5;
                ListView_SetItem(hwndListV, &lvI);
    
    
                tempStr2[0] = 0;
                if (msgObj->isClone(msgObj->findListIndex(i)))
                {
                  SortedMsg clonesmsg;
                  clonesmsg.noun = msgObj->getCloneNoun(msgObj->findListIndex(i));
                  clonesmsg.verb = msgObj->getCloneVerb(msgObj->findListIndex(i));
                  clonesmsg.ncase = msgObj->getCloneCase(msgObj->findListIndex(i));
                  clonesmsg.seq = 1;
                  
                  sprintf(tempStr,"<C> %s", msgObj->getPhrase(msgObj->findListIndex(clonesmsg)));
                  if (msgObj->getTranslation(msgObj->findListIndex(clonesmsg)))
                     sprintf(tempStr2,"<C> %s", msgObj->getTranslation(msgObj->findListIndex(clonesmsg)));

                }
                else
                {
                    sprintf(tempStr,"%s", msgObj->getPhrase(msgObj->findListIndex(i)));
                    if (msgObj->getTranslation(msgObj->findListIndex(i)))
                       sprintf(tempStr2,"%s", msgObj->getTranslation(msgObj->findListIndex(i)));
                }


                lvI.pszText = tempStr;
                lvI.iSubItem = 6;
                ListView_SetItem(hwndListV, &lvI);
    
                if (tempStr2[0])
                {
                   lvI.pszText = tempStr2;
                   lvI.iSubItem = 7;
                   ListView_SetItem(hwndListV, &lvI);
                }
                
                lvI.pszText = (char *) msgObj->getComment(msgObj->findListIndex(i));
                lvI.iSubItem = 8;
                ListView_SetItem(hwndListV, &lvI);


}

char CreateUnClonedSeq(HWND hwnd)
{
   SortedMsg cloneID, origID;
                                            
   origID.noun = msgObj->getCloneNoun(msgObj->currentString());
   origID.verb = msgObj->getCloneVerb(msgObj->currentString());
   origID.ncase = msgObj->getCloneCase(msgObj->currentString());
   origID.seq = 1;    
   
   cloneID = msgObj->DuplicateSeq(origID);

   return cloneID.noun;
}


LRESULT CALLBACK CloneDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
 
         case IDYES:
          {    
               char enterednumber[10];  
               if (GetDlgItemText(hDlg, IDC_CLONEFIELD, enterednumber, 10))
               {
                  int retnum = atoi(enterednumber);
                  EndDialog(hDlg, retnum);
                  return LOWORD(wParam);
               } 
          }     
          break;
         case IDCANCEL:
         case IDNO:
               EndDialog(hDlg, -1);
               return LOWORD(wParam);       
               
		}
		break;
	}
	return FALSE;
}

void DoCloning(HWND hwnd)
{    
     if (actualclone != -1)
     { //DO UNCLONING
        bool duplicateseq=false;
        if (seqclone) //if this is a clone of a sequence
        {           
            int btn = MessageBox (hwnd, CLONES_DUPLICATESEQ, WARN_ATTENTION, 
                                        MB_APPLMODAL | MB_ICONQUESTION | MB_YESNOCANCEL);
            switch (btn)
            {
               case IDYES:   
                   duplicateseq = true;
                   break;
               
               case IDNO:              
                    break;
               case IDCANCEL:      
               default:
                   return;    
            }
                        
        }
        
        int btn;
        if (!duplicateseq && (msgObj->getCloneNoun(actualclone) + msgObj->getCloneVerb(actualclone) + msgObj->getCloneCase(actualclone)))
           //in case of a hierarchical clone, we don't need to fix the aud MAP, since we get another clone from the uncloning process
           btn = MessageBox (hwnd, CLONES_HIERARCHICAL, INTERFACE_UNCLONINGTITLE, 
                                        MB_APPLMODAL | MB_ICONQUESTION | MB_OKCANCEL);
        else
           btn = DialogBox(hInst, (LPCTSTR)IDD_UNCLONEDIALOG, hwnd, (DLGPROC)SearchStatus);  //handling is the same as SearchStatus
        
        SortedMsg cloneID, origID; 
         
        switch(btn)
        {
           case IDYES:
           {  //CHECK VERIFY MAP when uncloning seq
              //NOTE: when re-cloning, references are kept in the MAP   
             if (msgObj->getAudMap() ==0)
                if (!ImportAudMapStub(hwnd))  //error messages handled inside
                   break;
                             
              cloneID.noun = msgObj->getNoun(msgObj->currentString());
              cloneID.verb = msgObj->getVerb(msgObj->currentString());
              cloneID.ncase = msgObj->getCase(msgObj->currentString());
              cloneID.seq = msgObj->getSeq(msgObj->currentString());
                              
              origID.noun = msgObj->getCloneNoun(msgObj->currentString());
              origID.verb = msgObj->getCloneVerb(msgObj->currentString());
              origID.ncase = msgObj->getCloneCase(msgObj->currentString());
              origID.seq = msgObj->getSeq(msgObj->currentString());  
                
             if (!duplicateseq)
             {    //SINGLE CLONE

                if (!AddExCloneToAudMAP(hwnd, &cloneID, &origID, msgObj->getAudMap()))   //error messages handled inside
                   break;
             }
           }   
           case IDOK:   
           case IDNO:
             if (duplicateseq)
             {
                   unsigned short i = msgObj->getPhrasesCount();           
                   char newnoun = 0;
                   newnoun = CreateUnClonedSeq(hwnd);
                   if (newnoun != 0)
                   {  
                      msgObj->setCloneNoun(msgObj->currentString(), newnoun); 
                     
                      //we must update Windows controls
                      SCROLLINFO sist;
                      sist.cbSize = sizeof(SCROLLINFO);
                      sist.fMask = SIF_PAGE | SIF_RANGE |SIF_POS;
                      sist.nPage = 1;
                      sist.nMin = 0;
                      sist.nMax = msgObj->getPhrasesCount()-1;
                      sist.nPos = msgObj->currentString();
                      SetScrollInfo(hwndScrollBar, SB_CTL, &sist, true);
                      
                      char msgStr[10240], translStr[10240] = "";
                      
                      for (; i<msgObj->getPhrasesCount(); i++) 
                          InsertListViewItem(i, msgStr, translStr); 
                       
                      SortMyListView(hwnd, msgObj->sorting());
                      
                      //now we add to the Audio MAP (differs from the single clone case, but
                      //              we had no noun reference before, so we have to do it now)  
                        
                      origID.seq = 1;    //not really needed, but let's keep failproof
                      
                      cloneID.noun = newnoun;
                      cloneID.verb = origID.verb;
                      cloneID.ncase = origID.ncase;
                      cloneID.seq = 1;    //not really needed, but let's keep failproof
                      
                      unsigned short orid = msgObj->findListIndex(origID);
                      do {
                         if (!msgObj->isClone(orid) && (btn==IDYES))
                          if (!AddExCloneToAudMAP(hwnd, &cloneID, &origID, msgObj->getAudMap()))   //error messages handled inside
                             {} //NOTE: let's continue anyway  // break;  
   
                         origID.seq++;
                         cloneID.seq++;     
                         orid = msgObj->findListIndex(origID);
                      } while(orid <msgObj->getPhrasesCount());
                      
                                                                      
                                      
                   } else
                   {
                      MessageBox(hwnd, CLONES_CANTUNCLONESEQ, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION); 
                      return;
                   } 
                  
                 
             } else //SINGLE CLONE
             {
               //msgObj->setTalker(msgObj->currentString(), msgObj->getTalker(actualclone)); //TALKER //CHECK should be copied from clone??
               msgObj->setCloneNoun(msgObj->currentString(), msgObj->getCloneNoun(actualclone)); //NOTE: for nested/hierarchical/treeded clones
               msgObj->setCloneVerb(msgObj->currentString(), msgObj->getCloneVerb(actualclone)); //NOTE: for nested/hierarchical/treeded clones
               msgObj->setCloneCase(msgObj->currentString(), msgObj->getCloneCase(actualclone)); //NOTE: for nested/hierarchical/treeded clones
                        
               msgObj->setPhrase(msgObj->currentString(), (char *) msgObj->getPhrase(actualclone));
             }
                    
             //TODO remove this message?
             //TODO add control that external map == internal?
             MessageBox(hwnd, CLONES_UNCLONINGSUCCEDED, INTERFACE_UNCLONINGTITLE, MB_OK | MB_ICONINFORMATION);                                                                                                                                                                            
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
             datasaved = false;
             EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);
                              
             updateStrIndex(hwnd, msgObj->currentString());       
        } 
                                                                                                                      
   } else { //DO CLONING
   
        long usvalue = DialogBox(hInst, (LPCTSTR)IDD_CLONEDIALOG, hwnd, (DLGPROC)CloneDialog);  //handling is the same as SearchStatus
    
        if ((usvalue>0) && (usvalue <= msgObj->getPhrasesCount()) && (msgObj->findListIndex(usvalue-1)!=msgObj->currentString()))
        {  //TODO if uncloned from a seq, remove the added phrases when re-cloning?? (or put a note in the docs stating that it won't work as expected) 
            //       NOTE: however, the copies shouldn't do any harm if left in the file
            if (msgObj->getSeq(msgObj->findListIndex(usvalue-1)) > 1)
               MessageBox(hwnd, CLONES_CANTCLONESEQINTERMEDIATE, ERR_TITLE,
                                                MB_OK | MB_ICONEXCLAMATION);

            else {
               msgObj->setCloneNoun(msgObj->currentString(), msgObj->getNoun(msgObj->findListIndex(usvalue-1))); 
               msgObj->setCloneVerb(msgObj->currentString(), msgObj->getVerb(msgObj->findListIndex(usvalue-1))); 
               msgObj->setCloneCase(msgObj->currentString(), msgObj->getCase(msgObj->findListIndex(usvalue-1))); 
 
               msgObj->setPhrase(msgObj->currentString(), "");
               msgObj->setTranslation(msgObj->currentString(), "");
               
               //for now, we only remove MAP entries to stuff uncloned by Sciamano (which is offset=0 however, so it is perfectly reversible)
               if (msgObj->getAudMap())
               {
                  SortedMsg cloneID;
                  
                  cloneID.noun = msgObj->getNoun(msgObj->currentString());
                  cloneID.verb = msgObj->getVerb(msgObj->currentString());
                  cloneID.ncase = msgObj->getCase(msgObj->currentString());
                  cloneID.seq = msgObj->getSeq(msgObj->currentString());
                  
                  RemoveZeroCloneFromAudMAP(hwnd, &cloneID, NULL, msgObj->getAudMap());
                  //NOTE IMPORTANT if we will remove non-clones too, remember to shift offsets in the MAP file!        
               }
               
               
               datasaved = false;
               EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);
        
               updateStrIndex(hwnd, msgObj->currentString());
            }      
        } else if (usvalue >= 0)
          MessageBox(hwnd, CLONES_CANTCLONEWRONGINDEX, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
     }
     
}



void setStringsFont(int fontPoints)
{
    DeleteObject(hfDefault);
                      
    hfDefault = CreateFont(fontPoints, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Courier New");
    
    SetWindowFont(hwndPhrase, hfDefault, true);
    SetWindowFont(hwndEdit, hfDefault, true);
    
    msgObj->fontSize(fontPoints);                  
                                                          
    for (int i=IDM_FONT14; i<=IDM_FONT26; i+=2)
    CheckMenuItem(menu, i, MF_UNCHECKED);
    
    fontPoints += IDM_FONT;
    if ((fontPoints>=IDM_FONT14)&&(fontPoints<=IDM_FONT26))
    CheckMenuItem(menu, fontPoints, MF_CHECKED);

}


BOOL DoNextFile(HWND hwnd)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
    
    if (!szFirstFindFileName[0])
       strcpy(szFirstFindFileName, szFindFileName);   //search has just started    
    

	char *fname=0;

	long pos;
	char fpath[MAX_PATH];
	for (pos = strlen(szFindFileName)-1; pos>=0; pos--) 
		if (szFindFileName[pos] == '\\')
			break;

	fname = (char *)(((unsigned long) szFindFileName) + pos+1);
	strncpy(fpath, szFindFileName,pos+1);
	fpath[pos+1]=0;


	char searchstr[MAX_PATH];
	sprintf(searchstr, "%s*.msg", fpath); 

	hFind = FindFirstFile(searchstr, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		MessageBox(hwnd, INTERFACE_INVALIDSEARCHHANDLE, INTERFACE_SEARCHTITLE,
                  MB_OK | MB_ICONEXCLAMATION);
		
	} 
	else 
	{
		bool retvalue = true;
		bool passed =false;

		if (!stricmp(szFindFileName, fpath))
			passed = true;
		

		do
		{			
			//if (retvalue)
			//{
				if (passed)
				{
					strcat(fpath, FindFileData.cFileName);
					DoFileOpen(hwnd, fpath, true);
					FindClose(hFind); 
                    if (!stricmp(fpath, szFirstFindFileName))  //back to the first file
                    {
                        MessageBox(hwnd, INTERFACE_ENDOFFILESSTR, INTERFACE_SEARCHTITLE,
                                         MB_OK | MB_ICONINFORMATION);
                        return FALSE;
                    } else return TRUE;
				}
				if (!stricmp(FindFileData.cFileName, fname))
					passed = true;
			//}
			retvalue = (FindNextFile(hFind, &FindFileData) != 0);
            if (!retvalue && passed) //try to restart from beginning
            {
               FindClose(hFind);
               sprintf(searchstr, "%s*.msg", fpath); 
	           hFind = FindFirstFile(searchstr, &FindFileData); 
               if (hFind == INVALID_HANDLE_VALUE) 
	           {
		           MessageBox(hwnd, INTERFACE_INVALIDSEARCHHANDLE, INTERFACE_SEARCHTITLE,
                                   MB_OK | MB_ICONEXCLAMATION);

	           } else retvalue = true;
            }
		}
		while (retvalue);
				
	}

	return FALSE;
}


BOOL DoFileOpen(HWND hwnd, char *filename, bool searching)
{
   if (msgObj && !datasaved)
   {

      int btn = MessageBox (hwnd, WARN_UNSAVEDCHANGES, WARN_ATTENTION,
                                  MB_APPLMODAL | MB_ICONQUESTION | MB_YESNOCANCEL);
      switch (btn)
      {
      case IDYES:
           if (!DoFileSave(hwnd))
              return FALSE;
           break;
      case IDNO:
           break;      
      default:
           return FALSE;
      } 
  
   }
   
   if (!searching)
      szFirstFindFileName[0] = 0;
      
   OPENFILENAME ofn;
   bool proceed = false;
   
   if (filename == NULL)
   {
      ZeroMemory(&ofn, sizeof(ofn));
      //szFileName[0] = 0;

      ofn.lStructSize = sizeof(ofn);
      ofn.hwndOwner = hwnd;
      ofn.lpstrFilter = INTERFACE_OPENFILEFILTER; 
      ofn.lpstrFile = szFileName;
      ofn.nMaxFile = MAX_PATH;
      //ofn.lpstrDefExt = "msg";

      ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOVALIDATE;
      proceed = (GetOpenFileName(&ofn)!=0);
   }
   else
   {
      strcpy(szFileName, filename);
      proceed=true;        
   }   
      
   if(proceed)
   {
      /////added 1.2b3 -- automatic temp file loading
      char tfile[MAX_PATH], oldfilename[MAX_PATH];
      strcpy(oldfilename, szFileName);
      sprintf(tfile, "%s.temp", szFileName);
      
      long i=strlen(szFileName);
      while (i>=0 && szFileName[i] != '\\') i--;
      
      strncpy(szFilePath, szFileName, i);
      szFilePath[i] = 0;
      
      datasaved = true;      
      EnableMenuItem(menu, IDM_FILESAVE, MF_GRAYED);

      if (strcmp(tfile, szTempFileName)!=0)
      {
       FILE *tfilebuf = fopen(tfile, "r");
       if (tfilebuf)
       {
         fclose(tfilebuf);

         if (MessageBox (hwnd, WARN_TEMPORARYFILEEXISTS, WARN_ATTENTION, 
                                        MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)
                         == IDYES)
              {
                   strcpy(szFileName, tfile);
                   datasaved = false;
                   EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);
              }
       }
      }  
      //////----------added 1.2, modified1.3
      SetWindowText(hwndNoun, "");      
      SetWindowText(hwndVerb, "");
      SetWindowText(hwndCase, "");
      SetWindowText(hwndTalker, "");
      

      ListView_DeleteAllItems(hwndListV); 
      /////    
   
      bool result, nonzerofile, sciold = false, hascomments = false;      
      if ( !(result = LoadFile(hwnd, szFileName, &msgObj)) )
      {
         if (msgObj) {
            delete msgObj;
            msgObj = 0;
         }

         MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
      }
      strcpy(szFileName, oldfilename);  //added in 1.2beta3 , to restore the name after the temp file loading
      strcpy(szFindFileName, szFileName);
      
      remove(szTempFileName);
      sprintf(szTempFileName, "%s.temp", szFileName);

      SetWindowText(hwndPhrase, "");
      SetWindowText(hwndEdit, "");
      SetWindowText(hwndComment, "");

      if (msgObj)
         nonzerofile = (msgObj->getPhrasesCount());

      if (msgObj && nonzerofile) {
         setStringsFont(msgObj->fontSize());  
                     
         if (tempwindowRect.right)   
         {  
            WINDOWPLACEMENT wndpl;
            wndpl.length = sizeof(WINDOWPLACEMENT); 
            GetWindowPlacement(hwnd, &wndpl);
            
            wndpl.rcNormalPosition = tempwindowRect;
            SetWindowPlacement(hwnd, &wndpl);
         }    
               
         SetWindowText(hwndPhrase, msgObj->getPhrase(msgObj->currentString()));
         SetWindowText(hwndComment, msgObj->getComment(msgObj->currentString()));
         const char *transtr = msgObj->getTranslation(msgObj->currentString());
         SetWindowText(hwndEdit, (transtr ? transtr : "\0"));
    
         EnableWindow(hwndScrollBar, true);

         SCROLLINFO sist;
         sist.cbSize = sizeof(SCROLLINFO);
         sist.fMask = SIF_PAGE | SIF_RANGE |SIF_POS;
         sist.nPage = 1;
         sist.nMin = 0;
         sist.nMax = msgObj->getPhrasesCount()-1;
         sist.nPos = msgObj->currentString();
         SetScrollInfo(hwndScrollBar, SB_CTL, &sist, true);
        
         EnableMenuItem(menu, IDM_LEFT, (msgObj->currentString() ? MF_ENABLED : MF_GRAYED));
         EnableMenuItem(menu, IDM_RIGHT, (msgObj->currentString()+1 != msgObj->getPhrasesCount() ? MF_ENABLED : MF_GRAYED));
     
         sciold = ((msgObj->getVersion() & 0xFFFFFF00) == SCI_01);
         hascomments = msgObj->hasComments();   //TODO: implement comments in every SCI version (as long as we use traduSCI format)
         
         EnableMenuItem(menu, IDM_EXPORTMAP, ((!sciold && (msgObj->getAudMap() != 0)) ? MF_ENABLED : MF_GRAYED));
         //CHECK sciold is only SCI_01, must exclude from MAP also SCI_02??
          
         EnableMenuItem(menu, IDM_CLONE, ((msgObj->getVersion() & 0xFFFFFF00) >= SCI_05 ? MF_ENABLED : MF_GRAYED));                                 
                                                                                                                                           
         EnableWindow(hwndPhraseButton, true);
         EnableWindow(hwndEditButton, true);
         EnableWindow(hwndCaseButton, true);    
         //if (!searching)                //TODO must use it or not?
         //   SendMessage(hwndPhraseButton, BM_CLICK, 0, 0);
         
         //for (unsigned short i = 0; i<msgObj->getPhrasesCount(); i++)  //adding to the listbox
         char msgStr[10240], translStr[10240] = "";
         for (long i = msgObj->getPhrasesCount()-1; i>=0; i--)
             InsertListViewItem(i, msgStr, translStr);
                   
         SetWindowText(hwndNoun, msgObj->getNounName(msgObj->getNoun(msgObj->currentString())));      
         SetWindowText(hwndVerb, msgObj->getVerbName(msgObj->getVerb(msgObj->currentString())));
         SetWindowText(hwndCase, msgObj->getCaseName(msgObj->getCase(msgObj->currentString())));
         SetWindowText(hwndTalker, msgObj->getTalkerName(msgObj->getTalker(msgObj->currentString())));
          
                   
         SortMyListView(hwnd, msgObj->sorting());
                  
      } else {

         EnableWindow(hwndScrollBar, false);
         
         EnableMenuItem(menu, IDM_LEFT, MF_GRAYED);
         EnableMenuItem(menu, IDM_RIGHT, MF_GRAYED);
         
         EnableMenuItem(menu, IDM_EXPORTMAP, MF_GRAYED);
         EnableMenuItem(menu, IDM_VERIFYMAP, MF_GRAYED);
         
         EnableWindow(hwndPhraseButton, false);
         EnableWindow(hwndEditButton, false);
         EnableWindow(hwndCaseButton, false); 
            
         datasaved = true;                             //added in 1.2beta3 for temp file autoload
         EnableMenuItem(menu, IDM_FILESAVE, MF_GRAYED); //added in 1.2beta3 for temp file autoload  
      }     

      char wname[MAX_PATH + 15] = "SCIaMano";
      if (result) {
         strcat(wname, " - ");
         if (strlen(szFileName)<64)         //TODO: must change this number according to window width
			strcat(wname, szFileName);
		  else
		  {
			int pos = 2;
			for (int i=strlen(szFileName); i>strlen(szFileName)-61; i--) 
				if (szFileName[i] == '\\')
					pos = i;
			
			strcat(wname, "...");
			strcat(wname, (char *)(((long)szFileName)+pos));
		  }
      }
      SetWindowText(hwnd, wname);    

      EnableWindow(hwndBarFrame, result && nonzerofile);
      EnableWindow(hwndListV, result && nonzerofile);
      EnableWindow(hwndStatusBar, result && nonzerofile);
      
      EnableWindow(hwndEdit, result && nonzerofile);
      EnableWindow(hwndComment, result && nonzerofile && hascomments);
     
      EnableWindow(hwndNoun, result && nonzerofile); 
      EnableWindow(hwndVerb, result && nonzerofile);
      EnableWindow(hwndCase, result && nonzerofile);
      EnableWindow(hwndTalker, result && nonzerofile); 
     
                      
     ///////////////////////////////////////////
     
      EnableMenuItem(menu, IDM_FILESAVEAS, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_IMPORTTEXT, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_EXPORTTEXT, ((result && nonzerofile) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_IMPORTFILE, ((result && nonzerofile) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_COMPAREWITH, ((result && nonzerofile) ? MF_ENABLED : MF_GRAYED));
     
      EnableMenuItem(menu, IDM_PROPERTIES, (result ? MF_ENABLED : MF_GRAYED));
       
      EnableMenuItem(menu, IDM_ADDTOWL, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_DOWORDSLOOKUP, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_SHOWWL, (result ? MF_ENABLED : MF_GRAYED));
      
      EnableMenuItem(menu, IDM_NOUNSFROMHEAP, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_LOADLABELSTXT, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_EXPORTNOUNS, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_EXPORTVERBS, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_EXPORTCASES, (result ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_EXPORTTALKERS, (result ? MF_ENABLED : MF_GRAYED));
            
     
      EnableMenuItem(menu, IDM_FIRST, ((result && nonzerofile) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_LAST, ((result && nonzerofile) ? MF_ENABLED : MF_GRAYED));
     
      
//TODO for old sci create a lista (not a list view)

      EnableMenuItem(menu, IDM_DRAWGF, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_DRAWGFBW, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_DRAWINDEXGF, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_DRAWNOUNGF, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_DRAWVERBGF, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_DRAWCASEGF, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      EnableMenuItem(menu, IDM_DRAWTALKERGF, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      //TODO EcoQuest may support index&talker graphs, but for now they are disabled ( && !sciold)
 
      EnableMenuItem(menu, IDM_NEXTCOMMENT, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
           //CHECK we disable comments for sciold  ... ok?
           
      EnableMenuItem(menu, IDM_LOADMAP, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED)); 
      EnableMenuItem(menu, IDM_VERIFYMAP, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));
      
      EnableMenuItem(menu, IDM_LOADAUDRES, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));    
      EnableMenuItem(menu, IDM_PLAY, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));    
      EnableMenuItem(menu, IDM_AUTOPLAY, ((result && nonzerofile && !sciold) ? MF_ENABLED : MF_GRAYED));    

      for (int i =14; i<=26; i++)
          EnableMenuItem(menu, IDM_FONT+i, ((result && nonzerofile) ? MF_ENABLED : MF_GRAYED));
     

      if (result && nonzerofile)
         updateStrIndex(hwnd, msgObj->currentString()); //NOTE: FIX to display clones when they occur as first displayed phrase
      
      if (result && nonzerofile)
      {
        if (msgObj->getFutureBoxSize() >0)
           MessageBox (hwnd, WARNING_FUTUREBOXDATA, WARN_ATTENTION, MB_OK | MB_ICONEXCLAMATION);
      
        //let's verify if the external MAP file exists, and if it's coherent with MSG stored content
        
        if (msgObj->getAudMap() != 0)
        {
          char audfile[MAX_PATH] = "";
          sprintf(audfile, "%s\\%s", szFilePath, msgObj->getAudMapName()); 

          if (!ValidateExternalMAP(hwnd, msgObj->getAudMap(), audfile))
             if (IDYES == MessageBox (hwnd, MAPS_MAPFILENOTFOUND, WARN_ATTENTION,
                                  MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO))
                ExportAudMAP(hwnd, msgObj->getAudMap(), audfile);
              
        }  
      }                      
               
      InvalidateRect(hwnd, NULL, true); //force repaint
      //UpdateWindow(hwnd);  
                        
      return result;
   }
   return FALSE;
}



BOOL DoFileSave(HWND hwnd)
{
   unsigned char tradformat = msgObj->tradusciFormat();
   if (FILE *tempf = fopen(szFileName, "rb"))
      fclose(tempf);
   else {
      MessageBox(hwnd, ERR_FILEMOVED, ERR_TITLE,
                       MB_OK | MB_ICONEXCLAMATION);
      return FALSE;
   }
   
   int btn;   
   switch (tradformat) {
   case SIERRA: //Sierra's file format   
      btn = MessageBox (hwnd, WARN_SIERRAFORMAT, WARN_ATTENTION,
                              MB_APPLMODAL | MB_ICONQUESTION | MB_YESNOCANCEL);
      if (btn == IDYES)
         tradformat = TRADUSCI2;
      if (btn == IDCANCEL)
         return FALSE; 
      break; 
   case OLDTRADUSCI: //old TraduSCI 1.0 file format       
      btn = MessageBox (hwnd, WARN_OLDFORMAT, WARN_ATTENTION,
                              MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO);
      if (btn == IDYES)
         tradformat = TRADUSCI2;
      if (btn == IDNO)
         return FALSE; 
      break;
   }
   
   char bakname[MAX_PATH];
   sprintf(bakname, "%s.BAK", szFileName);
   //if (fileexists
   //TODO we might use more than one backup file, by renaming old backups with numerals !!!
   remove(bakname);
   rename(szFileName, bakname);

   updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
   if(!SaveFile(hwnd, szFileName, (bool) tradformat))
   { 
       MessageBox(hwnd, ERR_CANTSAVECHANGES, ERR_TITLE,
                  MB_OK | MB_ICONEXCLAMATION);
       rename(bakname, szFileName);
       return FALSE;
   } else {
       if (msgObj->getAudMap())
       {  //NOTE we only updare the MAP when calling "Save" and not "Save As..." too
          char audmapfile[MAX_PATH];
          sprintf(audmapfile, "%s\\%s", szFilePath, msgObj->getAudMapName());
          ExportAudMAP(hwnd, msgObj->getAudMap(), audmapfile);
       }
       datasaved = true;
       msgObj->tradusciFormat(tradformat);
       EnableMenuItem(menu, IDM_FILESAVE, MF_GRAYED);
       remove(szTempFileName);
   }

   return TRUE;
}




BOOL DoFileSaveAs(HWND hwnd)
{
   OPENFILENAME ofn;
   char szSaveFileName[MAX_PATH] = "";
   strcpy(szSaveFileName, szFileName);
   
   ZeroMemory(&ofn, sizeof(ofn));
   //szSaveFileName[0] = 0;

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = INTERFACE_SAVEFILEFILTER;
   ofn.nFilterIndex = 2;
   ofn.lpstrFile = szSaveFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrDefExt = "msg"; //"trn";

   ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
               OFN_OVERWRITEPROMPT;
         
   if(GetSaveFileName(&ofn))
   {
       updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
 
       if(!SaveFile(hwnd, szSaveFileName, (ofn.nFilterIndex == 2)))
       { 
           MessageBox(hwnd, ERR_CANTSAVE, ERR_TITLE,
                            MB_OK | MB_ICONEXCLAMATION);
           return FALSE;
       } else {
           datasaved = true;
           msgObj->tradusciFormat(((ofn.nFilterIndex == 2) ? TRADUSCI2 : SIERRA));
           EnableMenuItem(menu, IDM_FILESAVE, MF_GRAYED);
       }

       char wname[MAX_PATH + 15] = "SCIaMano";  
       strcat(wname, " - ");
       if (strlen(szSaveFileName)<64)
			strcat(wname, szSaveFileName);
	   else
       {
			int pos = 2;
			for (int i=strlen(szSaveFileName); i>strlen(szSaveFileName)-61; i--) 
				if (szSaveFileName[i] == '\\')
					pos = i;
			
			strcat(wname, "...");
			strcat(wname, (char *)(((long)szSaveFileName)+pos));
	   }
   
       SetWindowText(hwnd, wname); 

       memcpy(szFileName, szSaveFileName, MAX_PATH);
       long i=strlen(szFileName);
       while (i>=0 && szFileName[i] != '\\') i--;
      
       strncpy(szFilePath, szFileName, i);  szFilePath[i] = 0;
       
       remove(szTempFileName);
       sprintf(szTempFileName, "%s.temp", szFileName);
       //CHECK and FIX is this the best solution? by doing this, the source folder is always changed!
   }

   return TRUE;
}




BOOL DoMsgImport(HWND hwnd)
{
   MsgData *impObj = 0;   
         
   OPENFILENAME ofn;

   ZeroMemory(&ofn, sizeof(ofn));
   //szImpFileName[0] = 0;

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = INTERFACE_OPENFILEFILTER; 
   ofn.lpstrFile = szImpFileName;
   ofn.nMaxFile = MAX_PATH;
 
   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   if(GetOpenFileName(&ofn))
   {
      bool result;
      if ( !(result = LoadFile(hwnd, szImpFileName, &impObj)) )
      {
         if (impObj) {
            delete impObj;
            impObj = 0;
         }

         MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
            
         return FALSE;
      }

      unsigned short impstrscount = impObj->getPhrasesCount();
      if (impstrscount!=msgObj->getPhrasesCount())
      {
         delete impObj; 
          
         MessageBox(hwnd, ERR_DIFFIMPFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
            
         return FALSE; 
      }
      
      bool importTrans = false;
      if (impObj->tradusciFormat()!=SIERRA)
         switch (MessageBox (hwnd, IMPORT_WOULDYOULIKEORIGINALSTRING, WARN_ATTENTION, 
              MB_APPLMODAL | MB_ICONQUESTION | MB_YESNOCANCEL))
         {
            case IDNO:
              importTrans = true;
            case IDYES:  
              break;
            default:
              return false;  
              
         }
      bool indexImport = true;
      if ((impObj->getVersion() & 0xFFFFFF00) != SCI_01)
         switch (MessageBox (hwnd, IMPORT_WOULDYOULIKELABELSEARCH, WARN_ATTENTION, 
              MB_APPLMODAL | MB_ICONQUESTION | MB_YESNOCANCEL))  
         {
            case IDYES:
              indexImport = false;
            case IDNO:  
              break;
            default:
              return false;  
              
         }
      for (unsigned short i=0; i< impstrscount; i++)
      {
          const char *tstr = impObj->getTranslation(impObj->findListIndex(i));
          if (!tstr || !importTrans) 
             tstr = impObj->getPhrase(impObj->findListIndex(i)); 
          
          unsigned short midx = msgObj->getPhrasesCount()+1; //NOTE just an invalid value
                
          if (indexImport)            
             midx =msgObj->findListIndex(i);
          else
          {
              SortedMsg *smsg = (SortedMsg *) impObj->getLabelFromIndex(impObj->findListIndex(i));
              midx = msgObj->findListIndex(*smsg);   
          }
          
          if (midx < msgObj->getPhrasesCount())
          {
                 msgObj->setTranslation(midx, (char *) tstr);
                 
                 LV_ITEM lvi;
                 lvi.pszText = (char *) tstr;
                 lvi.cchTextMax = 10240;                   
                 lvi.mask = LVIF_TEXT;
                 lvi.iItem = midx;
                 lvi.iSubItem = 7;
                 ListView_SetItem(hwndListV, &lvi);
          }  //NOTE else we could signal error, but...
      }
      
      const char *transtr = msgObj->getTranslation(msgObj->currentString());
      SetWindowText(hwndEdit, (transtr ? transtr : "\0"));
      
      datasaved = false;
      EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);
       
      delete impObj; 
   }

   return TRUE;
}




BOOL DoFileExport(HWND hwnd)
{
   OPENFILENAME ofn;
   char szTextFileName[MAX_PATH] = "";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = INTERFACE_EXPORTTEXTFILTER;
   ofn.lpstrFile = szTextFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrDefExt = "txt";

   ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
               OFN_OVERWRITEPROMPT;
         
   if(GetSaveFileName(&ofn))
   {
       if(!ExportFile(hwnd, szTextFileName))
       { 
           MessageBox(hwnd, ERR_CANTEXPORTTEXT, ERR_TITLE,
                            MB_OK | MB_ICONEXCLAMATION);
           return FALSE;
       }

   }

   return TRUE;
}




BOOL DoFileCompare(HWND hwnd)       //TODO everything must be changed here, we must import by noun case verb etc. matchings
                                    // NOTE not sure, it depends on the meaning of noun and verb. Probably they're related to each script, then
                                    //a matching like the proposed one might be useless (that is,  you couldn't compare different files).
                                    //A verification of this might be made by checking differences in nouns between GK floppy and GK CD, and of
                                    //course that was one of the reasons I intended to add this new matching feature.
{
   MsgData *cmpObj = 0;   
         
   OPENFILENAME ofn;

   ZeroMemory(&ofn, sizeof(ofn));
   
   ofn.lpstrTitle = INTERFACE_FILECOMPARETITLE;
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = INTERFACE_OPENFILEFILTER; 
   ofn.lpstrFile = szCmpFileName;
   ofn.nMaxFile = MAX_PATH;
 
   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   if(GetOpenFileName(&ofn))
   {
      if (!stricmp(szFileName, szCmpFileName))
      {
         MessageBox(hwnd, ERR_CANTCMPSAMEFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
            
         return FALSE;
      }
      
      
      bool result;
      if ( !(result = LoadFile(hwnd, szCmpFileName, &cmpObj)) )
      {
         if (cmpObj) {
            delete cmpObj;
            cmpObj = 0;
         }

         MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
            
         return FALSE;
      }

      unsigned short cmpstrscount = cmpObj->getPhrasesCount();
      if (cmpstrscount!=msgObj->getPhrasesCount())
      {
         delete cmpObj; 
          
         MessageBox(hwnd, ERR_DIFFCMPFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
            
         return FALSE; 
      }
      
      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

      char difffilepath[MAX_PATH];
      sprintf(difffilepath, "%s\\diff.txt", szAppDir);
      FILE *diffile = fopen(difffilepath, "w");
      if (diffile)
      { 
         fprintf(diffile, EXPORT_DIFFTITLE, szFileName, szCmpFileName);
         //List<StringItem> *tlist = cmpObj->getStringList();
         for (unsigned short i=0; i< cmpstrscount; i++)
         {
            const char *orgstr = msgObj->getPhrase(msgObj->findListIndex(i));
            const char *tstr = cmpObj->getTranslation(cmpObj->findListIndex(i));
            const char *mystr = msgObj->getTranslation(msgObj->findListIndex(i));
            
            
            if (strcmp((tstr!=NULL?tstr:""), (mystr!=NULL?mystr:"")))
            {
               fprintf(diffile, EXPORT_ITEM2, i+1, orgstr);
               fprintf(diffile, EXPORT_TRANSLATION, mystr);
               fprintf(diffile, "%s\n\n", tstr); 
            }  
                      
         }
         fprintf(diffile, EXPORT_ENDOFFILE);
         
         fclose(diffile);
         
         ShellExecute(hwnd, "edit", difffilepath, NULL, NULL, SW_SHOW);
         
         //TODO must Save As... to a choosen file, or
         //   find a way to delete the temp text file once used
      }
      else
      {
         MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
            
         return FALSE;
      }
       
      delete cmpObj; 
   }

   return TRUE;
}




void versionString(char *vers)
{    //FIX ME: this should be handled in a better way due to changes in versions
    switch ( msgObj->getVersion() & 0xFFFFFF00 ) {
    case SCI_01:
       memcpy(vers, "SCI1.1.01", 10);
       break;     
    case SCI_02_B:
    case SCI_02_C:
    case SCI_02_D:
       memcpy(vers, "SCI1.1.02", 10);
       break;
    case SCI_05:
       memcpy(vers, "SCI1.1.05", 10);
       break;
    case SCI_06:
       memcpy(vers, "SCI1.1.06", 10);
       break;
    case SCI32_100:
       memcpy(vers, "SCI32.100", 10);
       break; 
    /*case SCI013:
       memcpy(vers, "SCI1.1.013", 11);
       break;    
    case SCI274_411:
    case SCI274_411p:
       memcpy(vers, "SCI1.1.2", 10);
       break;    
    case SCI029:
       memcpy(vers, "SCI1.1.029", 11);
       break;    
    case SCI05x_sm:
       memcpy(vers, "SCI1.1.05", 10);
       break;    
    case SCI05x:
       memcpy(vers, "SCI1.1.05", 10);
       break;    
    case SCI06x:
       memcpy(vers, "SCI1.1.06", 10);
       break;    
    case SCI06x_EQ2:
       memcpy(vers, "SCI1.1.06", 10);
       break;    
    /*case SCI07x:
       memcpy(vers, "SCI1.1.07", 10);
       break;*/    
    /*case SCI081:
       memcpy(vers, "SCI1.1.08", 10);
       break;*/    
/*    case SCI083:
       memcpy(vers, "SCI1.1.083", 11);
       break;    
    case SCI113:
       memcpy(vers, "SCI1.1.113", 11);
       break;    
    case SCIv2:
       memcpy(vers, "SCI1.1/32", 10);
       break;    
    case SCIv3:
    case SCIv3_2: 
       memcpy(vers, "SCI32", 6);
       break;    */
    }

    return;
}


void AddClonedStrings(SortedMsg csmsg, char *strapp, bool translmsg)
{           //RECURSIVE function to resolve clones
              
              sprintf(strapp, CLONES_LABELFORMAT, strapp, csmsg.noun, csmsg.verb, csmsg.ncase); 
              
              unsigned short k = 0;
              while (k < msgObj->getPhrasesCount())
              {
                  if ((msgObj->getNoun(k) == csmsg.noun) &&
                      (msgObj->getVerb(k) == csmsg.verb) &&
                      (msgObj->getCase(k) == csmsg.ncase))
                      break;
                  k++;
              }
              
              
              unsigned short z = k;
              while (z < msgObj->getPhrasesCount()) //has been found
              {
               //SEARCH AND ADD STUFF HERE
               if ((msgObj->getNoun(z) != csmsg.noun) ||
                   (msgObj->getVerb(z) != csmsg.verb) ||
                   (msgObj->getCase(z) != csmsg.ncase))
                   break;
               
                SortedMsg clonechildmsg;
                clonechildmsg.noun = msgObj->getCloneNoun(z);
                clonechildmsg.verb = msgObj->getCloneVerb(z);
                clonechildmsg.ncase = msgObj->getCloneCase(z); 
                if (!(clonechildmsg.noun+clonechildmsg.verb+clonechildmsg.ncase))
                {
                   char *tname = msgObj->getTalkerName(msgObj->getTalker(z));
                   char tvoid[2] = "";  
                   char *tmessage = (char *) (translmsg ? (msgObj->getTranslation(z)?msgObj->getTranslation(z): tvoid): msgObj->getPhrase(z));
                   sprintf(strapp, "%s\r\n(%d) %s%s%s", strapp, msgObj->getFileIndex(z)+1, (tname ?tname:""), (tname?": ":""), tmessage);
                } else {
                  sprintf(strapp, "%s\r\n", strapp);
                  AddClonedStrings(clonechildmsg, strapp, translmsg);  
                }
                z++;
              }
}


void updateStrIndex(HWND hwnd, unsigned short value)
{ 

//FIX for the noun fields:
    SetFocus(hwndEdit);  
    
    bool playaudio = ((value != msgObj->currentString()) && autoplay);
    
    if (value < msgObj->getPhrasesCount())
    {
          char editext[10240];
              
          
          SortedMsg clonesmsg;
          
          bool isclone = msgObj->isClone(msgObj->currentString());
          
          if (!(((msgObj->getVersion() & 0xFFFFFF00) >= SCI_05) && isclone))  
          {  //if it is a clone we don't store the data in the message hunk
          
               /* must load the new stringitem but first the current values
                  in the interface must be saved : */
               
               GetWindowText(hwndEdit, editext, 10240);      
              
              if (!msgObj->setTranslation(msgObj->currentString(), editext))
                MessageBox(hwnd, ERR_CANTALLOCSTRING, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);  
              
          }
          
          GetWindowText(hwndComment, editext, 10240);      
          msgObj->setComment(msgObj->currentString(), editext);

          //we are saving the labels names in memory structures
          GetWindowText(hwndNoun, editext, 100);
          msgObj->setNounName(msgObj->getNoun(msgObj->currentString()), editext);
          GetWindowText(hwndVerb, editext, 100);
          msgObj->setVerbName(msgObj->getVerb(msgObj->currentString()), editext);
          GetWindowText(hwndCase, editext, 100);
          msgObj->setCaseName(msgObj->getCase(msgObj->currentString()), editext);
          GetWindowText(hwndTalker, editext, 100);
          msgObj->setTalkerName(msgObj->getTalker(msgObj->currentString()), editext);
          

          if (!datasaved)
             SaveFile(hwnd, szTempFileName, true);

          msgObj->currentString(value);
          
          seqclone = false;
          
          if ((msgObj->getVersion() & 0xFFFFFF00) >= SCI_05) //if clones are supported
          {
             isclone = msgObj->isClone(msgObj->currentString());
             if (isclone)  
             {                         
                  EnableWindow(hwndEdit, false);
            
                  actualclone = msgObj->currentString();             
                  clonesmsg.seq = 1;             

                  clonesmsg.noun = msgObj->getCloneNoun(actualclone);
                  clonesmsg.verb = msgObj->getCloneVerb(actualclone);
                  clonesmsg.ncase = msgObj->getCloneCase(actualclone);

                  actualclone = msgObj->findListIndex(clonesmsg);

                  char strapp[10240] = "";
                  AddClonedStrings(clonesmsg, strapp, false);
                  SetWindowText(hwndPhrase, strapp);               
                                    
                  strapp[0]=0;
                  AddClonedStrings(clonesmsg, strapp, true);
                  SetWindowText(hwndEdit, strapp);
             
                  clonesmsg.seq = 2;             
                  if (msgObj->findListIndex(clonesmsg) < msgObj->getPhrasesCount())
                  //if a second element matching has been found
                  {
                     seqclone = true;
                  }  
                   
             } else{
                  actualclone = -1;
                  EnableWindow(hwndEdit, true);
                  SetWindowText(hwndPhrase, msgObj->getPhrase(msgObj->currentString()));               
                  const char *transtr = msgObj->getTranslation(msgObj->currentString());
                  SetWindowText(hwndEdit, (transtr ? transtr : "\0"));  
             }
             
          
          } else {                    //OLDER SCI VERSION (< 05)
                 SetWindowText(hwndPhrase, msgObj->getPhrase(msgObj->currentString()));               
                 const char *transtr = msgObj->getTranslation(msgObj->currentString());
                 SetWindowText(hwndEdit, (transtr ? transtr : "\0"));         
          }
          
          SetWindowText(hwndComment, msgObj->getComment(msgObj->currentString()));
           
          ScrollBar_SetPos(hwndScrollBar, msgObj->currentString(), true);
          
          EnableMenuItem(menu, IDM_LEFT, (msgObj->currentString() ? MF_ENABLED : MF_GRAYED));
          EnableMenuItem(menu, IDM_RIGHT, (msgObj->currentString()+1 != msgObj->getPhrasesCount() ? MF_ENABLED : MF_GRAYED));
          
          ///////////// THIS IS THE NEW SECTION FOR THE LABELS STUFF //////////////////
          
          char rinfo[32] = "";
          sprintf(rinfo, "%d", msgObj->getNoun(msgObj->currentString()));
          SetWindowText(hwndNounIndex, rinfo);      
          sprintf(rinfo, "%d", msgObj->getVerb(msgObj->currentString()));
          SetWindowText(hwndVerbIndex, rinfo);
          sprintf(rinfo, "%d", msgObj->getCase(msgObj->currentString()));
          SetWindowText(hwndCaseIndex, rinfo);                         
          sprintf(rinfo, "%d", msgObj->getTalker(msgObj->currentString()));
          SetWindowText(hwndTalkerIndex, rinfo); 
          
          //we load and display strings from the array   
          EnableWindow(hwndNoun, msgObj->getNoun(msgObj->currentString())); 
          SetWindowText(hwndNoun, msgObj->getNounName(msgObj->getNoun(msgObj->currentString())));
          EnableWindow(hwndVerb, msgObj->getVerb(msgObj->currentString()));
          SetWindowText(hwndVerb, msgObj->getVerbName(msgObj->getVerb(msgObj->currentString())));
          EnableWindow(hwndCase, msgObj->getCase(msgObj->currentString()));
          SetWindowText(hwndCase, msgObj->getCaseName(msgObj->getCase(msgObj->currentString())));
          EnableWindow(hwndTalker, msgObj->getTalker(msgObj->currentString()));
          SetWindowText(hwndTalker, msgObj->getTalkerName(msgObj->getTalker(msgObj->currentString())));  
          //////////////////////////////////////////////////////////////////////////////
          LV_FINDINFO lvfi;
          lvfi.flags = LVFI_PARAM;
          lvfi.lParam = msgObj->getFileIndex(msgObj->currentString());
          long listitem = ListView_FindItem(hwndListV, -1, &lvfi);
          ListView_SetItemState(hwndListV, listitem, LVIS_SELECTED, LVIS_SELECTED);
          ListView_EnsureVisible(hwndListV, listitem, true);

             
          if (unkbytesdecodinginfo)
          {
             SetRect(&rc, 0, 470, 200, 800);                       
             InvalidateRect(hwndMain, &rc, true); //force repaint                                                                        
          }
          
          if (playaudio) PlayAudio(hwnd, true);   
          
                              
          return;
          
    } 
    if (!msgObj->getPhrasesCount()) {
       SetWindowText(hwndPhrase, "");
       SetWindowText(hwndComment, "");
       SetWindowText(hwndEdit, "");
    }     
}


typedef BOOL (*Func)(HWND, char *, unsigned char, char *, char *);
 
/* this is what is going to be holding our function, I like to make it the same name as the function we are importing, but you can make it whatever you like. */
Func ExtractFromVolume;


int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    
    InitCommonControls();
    
    HINSTANCE DLL = LoadLibrary("SCIdump.dll");
    /* check for error on loading the DLL */
    if(DLL==NULL) 
              MessageBox(NULL, ERR_CANTLOADDLL, ERR_TITLE, MB_OK | MB_ICONERROR);

    ExtractFromVolume=(Func)GetProcAddress((HMODULE)DLL, "?ExtractFromVolumeSkel@@YAHPAUHWND__@@PADE11@Z");
    /* check for error on getting the function */
    if(ExtractFromVolume==NULL) 
    {
       FreeLibrary((HMODULE)DLL);
       MessageBox(NULL, ERR_CANTLOADDLL, ERR_TITLE, MB_OK | MB_ICONERROR);
    }

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = "themenu"; /* from resource file */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use light-gray as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BTNSHADOW;//COLOR_WINDOW; //GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&wincl)) return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "TransSCI",          /* Title Text */
            WS_OVERLAPPEDWINDOW ,   /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           WIN_WIDTH,                 /* The programs width */
           WIN_HEIGHT,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* no Menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    /* Make the window visible on the screen */
    ShowWindow(hwnd, nFunsterStil);
    
    hwndMain = hwnd;
    
    menu = GetMenu(hwnd); 

    
    hfDefault = CreateFont(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Courier New");    //NOTE: was "MS Sans Serif"
    hfInterface = (HFONT) GetStockObject(ANSI_VAR_FONT);//OEM_FIXED_FONT);//DEFAULT_GUI_FONT);  //TODO replace with a fixed font
    
    WNDCLASSEX winbarcl;        /* Data structure for the windowclass */
        
    /* The Window structure */
    winbarcl.hInstance = hThisInstance;
    winbarcl.lpszClassName = "BAR_FRAME";
    winbarcl.lpfnWndProc = WindowBarProcedure;      /* This function is called by windows */
    winbarcl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    winbarcl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    winbarcl.hIcon = NULL;
    winbarcl.hIconSm = NULL;
    winbarcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    winbarcl.lpszMenuName = NULL; /* from resource file */
    winbarcl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    winbarcl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use light-gray as the background of the window */
    winbarcl.hbrBackground = (HBRUSH) COLOR_BTNSHADOW;//COLOR_WINDOW; //GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&winbarcl)) return 0;

    RECT hwindRect;
    GetClientRect(hwnd, &hwindRect);

    /* The class is registered, let's create the program*/
    hwndBarFrame = CreateWindowEx(WS_EX_STATICEDGE, "BAR_FRAME", "", 
            WS_CHILD | WS_VISIBLE,
            -5, 0, hwindRect.right+10, 26, hwnd, NULL, hThisInstance, NULL);
    


    WNDCLASSEX wintextcl;        /* Data structure for the windowclass */
        
    /* The Window structure */
    wintextcl.hInstance = hThisInstance;
    wintextcl.lpszClassName = "TEXT_FRAME";
    wintextcl.lpfnWndProc = WindowTextProcedure;      /* This function is called by windows */
    wintextcl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wintextcl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wintextcl.hIcon = NULL;
    wintextcl.hIconSm = NULL;
    wintextcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wintextcl.lpszMenuName = NULL; /* from resource file */
    wintextcl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wintextcl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use light-gray as the background of the window */
    wintextcl.hbrBackground = (HBRUSH) COLOR_BTNSHADOW;//COLOR_WINDOW; //GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&wintextcl)) return 0;
    

    /* The class is registered, let's create the program*/
    hwndFrame = CreateWindowEx(0, "TEXT_FRAME", "", 
            WS_CHILD | WS_VISIBLE, //SS_ETCHEDHORZ
            //10, 50, 520, 30, hwnd, NULL, GetModuleHandle(NULL), NULL); 
            0, 26, hwindRect.right, hwindRect.bottom-50, hwnd, NULL, hThisInstance, NULL);
       

    hwndStatusBar = CreateWindowEx(0, "msctls_statusbar32", "",
                  WS_CHILD | WS_VISIBLE,
                  0,hwindRect.right,hwindRect.bottom-24,24, hwnd, NULL,GetModuleHandle(NULL), NULL); 
     
    //let's subclass the statusbar     
    g_pfnWndProcOrig = (WNDPROC) SetWindowLong(hwndStatusBar, GWL_WNDPROC, (LONG) WinStatusBarProcedure); 

    
    hwndListV = CreateWindowEx(WS_EX_CLIENTEDGE, "SysListView32", "",
            WS_CHILD|WS_VISIBLE|WS_TABSTOP|LVS_SINGLESEL|LVS_REPORT,       //LVS_SHOWSELALWAYS|
            18, 0, hwindRect.right-18, 260,
            hwndFrame, (HMENU) BTNLISTV_ID, hThisInstance, NULL); 
        
    //SetWindowFont(hwndListV, hfInterface, false);     
    //TODO for now we remove that, but maybe set this font also to other controls
    hfComments = GetWindowFont(hwndListV);
    
    SendMessage(hwndListV, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM) LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP|LVS_EX_ONECLICKACTIVATE); // |LVS_EX_BORDERSELECT|LVS_EX_GRIDLINES );


    LV_COLUMN lvc; 

    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text, and subitem members
    // of the structure are valid. 
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	  
 
        lvc.iSubItem = 0;
        lvc.pszText = (char*)"Dummy";	
        lvc.cx = 10;     // width of column in pixels
        ListView_InsertColumn(hwndListV, 0, &lvc); 
        
        lvc.cx = 55;
        lvc.fmt = LVCFMT_RIGHT;
        lvc.pszText = (char*)INTERFACE_INDEX;
        ListView_InsertColumn(hwndListV, 1, &lvc); 
        
        lvc.cx = 70;
        lvc.pszText = (char*)INTERFACE_NOUN;
        ListView_InsertColumn(hwndListV, 2, &lvc); 

        lvc.pszText = (char*)INTERFACE_VERB;
        ListView_InsertColumn(hwndListV, 3, &lvc); 

        lvc.pszText = (char*)INTERFACE_CASE;
        ListView_InsertColumn(hwndListV, 4, &lvc); 
        
        lvc.cx = 50;
        lvc.pszText = (char*)INTERFACE_SEQUENCE;
        ListView_InsertColumn(hwndListV, 5, &lvc); 

        lvc.cx = 70;
        lvc.pszText = (char*)INTERFACE_TALKER;
        ListView_InsertColumn(hwndListV, 6, &lvc); 


        lvc.cx = 400;     // width of column in pixels
        lvc.fmt = LVCFMT_LEFT;  // left-aligned column
         
        lvc.pszText = (char*)INTERFACE_MESSAGE;
        ListView_InsertColumn(hwndListV, 7, &lvc); 
        
        lvc.pszText = (char*)INTERFACE_MTRANSLATION;
        ListView_InsertColumn(hwndListV, 8, &lvc); 
        
        lvc.pszText = (char*)INTERFACE_COMMENT;
        ListView_InsertColumn(hwndListV, 9, &lvc); 

        ListView_DeleteColumn(hwndListV, 0);
        
    hwndMessageTag = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", INTERFACE_ORIGINAL, 
            WS_CHILD | WS_VISIBLE | SS_CENTER, 
            0, 265, 18, 60, hwndFrame, NULL, GetModuleHandle(NULL), NULL);                                                
    SetWindowFont(hwndMessageTag, hfInterface, false);
    
    hwndTranslationTag = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", INTERFACE_TRANSLATION, 
            WS_CHILD | WS_VISIBLE | SS_CENTER, 
            0, 335, 18, 60, hwndFrame, NULL, GetModuleHandle(NULL), NULL);                                                
    SetWindowFont(hwndTranslationTag, hfInterface, false);
                                                                                                                                                                                                                                                                                                                                                                                                                                                          
    hwndCommentsTag = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", INTERFACE_COMMENTS, 
            WS_CHILD | WS_VISIBLE | SS_CENTER, 
            0, 405, 18, 50, hwndFrame, NULL, GetModuleHandle(NULL), NULL);                                                
    SetWindowFont(hwndCommentsTag, hfInterface, false); 
    
       

      
    hwndNounLabel = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", INTERFACE_NOUN, 
            WS_CHILD | WS_VISIBLE, 
            10, 4, 50, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndNounLabel, hfInterface, false);                                                                                                                                                                                                                                                    
    
    hwndVerbLabel = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", INTERFACE_VERB, 
            WS_CHILD | WS_VISIBLE, 
            210, 4, 40, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndVerbLabel, hfInterface, false);                                                                                                                                                                                                                                                                                                                                                                                                       
                                        
    hwndCaseLabel = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", INTERFACE_CASE, 
            WS_CHILD | WS_VISIBLE, 
            400, 4, 40, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndCaseLabel, hfInterface, false);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    hwndTalkerLabel = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", INTERFACE_TALKER, 
            WS_CHILD | WS_VISIBLE, 
            585, 4, 40, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndTalkerLabel, hfInterface, false);
    

                                                                                                                                                                    
    hwndNounIndex = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", "0", 
            WS_CHILD | WS_VISIBLE | SS_RIGHT, 
            60, 4, 20, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndNounIndex, hfInterface, false);                                                                                                                                                                                                                                                    
    
    hwndVerbIndex = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", "0", 
            WS_CHILD | WS_VISIBLE | SS_RIGHT, 
            250, 4, 20, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndVerbIndex, hfInterface, false);                                                                                                                                                                                                                                                                                                               
    hwndCaseIndex = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", "0", 
            WS_CHILD | WS_VISIBLE | SS_RIGHT, 
            435, 4, 20, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndCaseIndex, hfInterface, false);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
    hwndTalkerIndex = CreateWindowEx(WS_EX_WINDOWEDGE, "STATIC", "0", 
            WS_CHILD | WS_VISIBLE | SS_RIGHT, 
            630, 4, 20, 20, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);                                                                                                                                        
    SetWindowFont(hwndTalkerIndex, hfInterface, false);
    
                                                                                                                                                                                    
    hwndPhrase = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_VSCROLL | ES_NOHIDESEL, 
            18, 260, hwindRect.right-18, 70, hwndFrame, NULL, GetModuleHandle(NULL), NULL);
    if(hwndPhrase == NULL)
       MessageBox(hwnd, ERR_FIELD1, ERR_TITLE, MB_OK | MB_ICONERROR);

    SetWindowFont(hwndPhrase, hfDefault, false);
     

    hwndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_MULTILINE | WS_VSCROLL | ES_NOHIDESEL, 
            18, 330, hwindRect.right-18, 70, hwndFrame, NULL, GetModuleHandle(NULL), NULL);
    if(hwndEdit == NULL)
       MessageBox(hwnd, ERR_FIELD2, ERR_TITLE, MB_OK | MB_ICONERROR);

    SetWindowFont(hwndEdit, hfDefault, false);    

   
    hwndComment = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",  
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_MULTILINE | WS_VSCROLL | ES_NOHIDESEL, 
            18, 402, hwindRect.right-18, 60, hwndFrame, NULL, GetModuleHandle(NULL), NULL);
    if(hwndComment == NULL)
       MessageBox(hwnd, ERR_FIELD3, ERR_TITLE, MB_OK | MB_ICONERROR);

    SetWindowFont(hwndComment, hfComments, false);
    //TODO to leave hfComments?(now same font of list) or should I set the small hfinterface?          
    hwndScrollBar = CreateWindow (                    
        "scrollbar",/* Builtin button class */
        "",
        WS_CHILD | WS_DISABLED | SBS_VERT | WS_VISIBLE,
        0, 0, 18, 260,
        hwndFrame,/* Parent is this window. */
        (HMENU) SCRLBAR_ID,/* Control ID: 2 */
        hThisInstance, // ((LPCREATESTRUCT) lParam)->hInstance,
        NULL
    );
     
           
    /////////////////////////////
   
    hwndNoun = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_CENTER | ES_AUTOHSCROLL, 
            90, 2, 105, 19, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);
    SetWindowFont(hwndNoun, hfInterface, false);
    
    hwndVerb = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_CENTER | ES_AUTOHSCROLL, 
            280, 2, 105, 19, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);
    SetWindowFont(hwndVerb, hfInterface, false); 
    
    hwndCase = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_CENTER | ES_AUTOHSCROLL, 
            465, 2, 105, 19, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);
    SetWindowFont(hwndCase, hfInterface, false);

    hwndTalker = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_CENTER | ES_AUTOHSCROLL, 
            660, 2, 105, 19, hwndBarFrame, NULL, GetModuleHandle(NULL), NULL);
    SetWindowFont(hwndTalker, hfInterface, false);
    
   
                    
    hwndFindButton = CreateWindow (
        "button",/* Builtin button class */
        INTERFACE_FINDBUTTON,
        WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
        5, 4, 70, 18,
        hwndStatusBar,/* Parent is this window. */
        (HMENU) BTNFIND_ID,/* Control ID: 2 */
        hThisInstance, // ((LPCREATESTRUCT) lParam)->hInstance,
        NULL
    );
 
    hwndReplaceButton = CreateWindow (
        "button",/* Builtin button class */
        INTERFACE_REPLACEBUTTON,
        WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON,
        534, 4, 90, 18,
        hwndStatusBar,/* Parent is this window. */
        (HMENU) BTNREPLACE_ID,/* Control ID: 2 */
        hThisInstance, // ((LPCREATESTRUCT) lParam)->hInstance,
        NULL
    );
    SetWindowFont(hwndFindButton, hfInterface, true);
    SetWindowFont(hwndReplaceButton, hfInterface, true);

    hwndFindText = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_LEFT | ES_NOHIDESEL | ES_AUTOHSCROLL, 
            80, 4, 105, 18, hwndStatusBar, NULL, GetModuleHandle(NULL), NULL);
    if(hwndFindText == NULL)
       MessageBox(hwnd, ERR_FIELD5, ERR_TITLE, MB_OK | MB_ICONERROR);

    hwndReplaceText = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_DISABLED | ES_LEFT | ES_NOHIDESEL | ES_AUTOHSCROLL, 
            630, 4, 105, 18, hwndStatusBar, NULL, GetModuleHandle(NULL), NULL);
    if(hwndReplaceText == NULL)
       MessageBox(hwnd, ERR_FIELD6, ERR_TITLE, MB_OK | MB_ICONERROR);

    SetWindowFont(hwndFindText, hfInterface, true);
    SetWindowFont(hwndReplaceText, hfInterface, true);


    hwndCaseButton = CreateWindow (
        "button",/* Builtin button class */
        INTERFACE_CASESENSITIVE,
        WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_AUTOCHECKBOX,
        410, 3, 110, 20,
        hwndStatusBar,/* Parent is this window. */
        (HMENU) BTNCASE_ID,/* Control ID: 2 */
        hThisInstance, // ((LPCREATESTRUCT) lParam)->hInstance,
        NULL
    );
     //TODO: add, perhaps, this button to search in all files)
  /*  hwndAllFilesButton = CreateWindow (
        "button",
        INTERFACE_ALLFILES,
        WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_AUTOCHECKBOX,
        23, 495, 130, 22,
        hwndBarFrame,
        (HMENU) BTNFILES_ID,
        hThisInstance, 
        NULL
    );      */
   ////////////////////////////////////////
    hwndPhraseButton = CreateWindow (
        "button",/* Builtin button class */
        INTERFACE_ORIGINALBUTTON,
        WS_CHILD | WS_VISIBLE | WS_DISABLED | WS_GROUP | BS_AUTORADIOBUTTON,
        220, 3, 80, 20,
        hwndStatusBar,/* Parent is this window. */
        (HMENU) BTNPHRASE_ID,/* Control ID: 2 */
        hThisInstance, // ((LPCREATESTRUCT) lParam)->hInstance,
        NULL
    );
 
    hwndEditButton = CreateWindow (
        "button",/* Builtin button class */
        INTERFACE_TRANSLATIONBUTTON,
        WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_AUTORADIOBUTTON,
        300, 3, 90, 20,
        hwndStatusBar,/* Parent is this window. */
        (HMENU) BTNEDIT_ID,/* Control ID: 2 */
        hThisInstance, // ((LPCREATESTRUCT) lParam)->hInstance,
        NULL
    );

    SetWindowFont(hwndCaseButton, hfInterface, true);
    SetWindowFont(hwndPhraseButton, hfInterface, true);
    SetWindowFont(hwndEditButton, hfInterface, true);
    
   
          
    hInst= hThisInstance;
    accelTable = LoadAccelerators(hThisInstance, TEXT("theAccel"));
 
    char szAppPath[MAX_PATH];
     
    GetModuleFileName(NULL, szAppPath, MAX_PATH);
    strncpy(szAppDir, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
    szAppDir[strrchr(szAppPath, '\\') - szAppPath] = '\0'; 
 
    if (lpszArgument[0]!=0)
    {
	   char startupfile[MAX_PATH];
       if (lpszArgument[0]=='\"')
	   {
		   strncpy(startupfile, lpszArgument+1,strlen(lpszArgument)-2);
		   startupfile[strlen(lpszArgument)-2]=0;
	   }
	   else
		   strcpy(startupfile, lpszArgument);

       DoFileOpen(hwnd, startupfile, false);
	} else
    {  
       //SPLASH SCREEN
       PostMessage(hwndMain, WM_COMMAND, IDM_FILEABOUT, 0);
    }
    
    /* Run the message loop. It will run until GetMessage( ) returns 0 */
    while(GetMessage(&messages, NULL, 0, 0))
    {
           if (!TranslateAccelerator(hwnd, accelTable, &messages))
           {
              /* Translate virtual-key messages into character messages */
              TranslateMessage(&messages);
              /* Send message to WindowProcedure */
              DispatchMessage(&messages);
           }
    }
    
    DeleteObject(hfDefault);
    FreeLibrary((HMODULE)DLL);
    
    /* The program return-value is 0 - The value that PostQuitMessage( ) gave */
    return messages.wParam;
}

LRESULT CALLBACK SearchStatus(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
            
         case IDCANCEL:
			
          case IDYES:
          case IDNO:
          EndDialog(hDlg, LOWORD(wParam));
          return LOWORD(wParam);
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK AboutBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

    case WM_CLOSE:
         EndDialog(hDlg, LOWORD(wParam));
         return FALSE;
   
    case WM_CTLCOLORSTATIC: {
         SetBkMode((HDC)wParam,TRANSPARENT);
         return (long)CreateSolidBrush(0xFFFFFF); 
    }           
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
            
            case IDC_LOADMSG:
            case IDC_LOADRES:
            case IDC_MANUAL:
            case IDC_CLOSEDLG:

          EndDialog(hDlg, LOWORD(wParam));
          return LOWORD(wParam);
		}
		break;
	}
	return FALSE;
}


/* This function is called by the Windows function DispatchMessage( ) */
LRESULT CALLBACK WindowTextProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   

    switch (message)                  /* handle the messages */
    {
           case WM_SIZE:
           {
                long hquantum = (HIWORD(lParam)-62)/6;
                long hremainder = (HIWORD(lParam)-62)%6; 
                MoveWindow(hwndListV, 18, 0, LOWORD(lParam)-18, hquantum*4+hremainder, false);
                MoveWindow(hwndPhrase, 18, hquantum*4+hremainder, LOWORD(lParam)-18, hquantum, false);
                MoveWindow(hwndEdit, 18, hquantum*5+hremainder, LOWORD(lParam)-18, hquantum, false);
                MoveWindow(hwndComment, 18, hremainder+hquantum*6 +1, LOWORD(lParam)-18, 60, false);
                MoveWindow(hwndCommentsTag, 0, hremainder+hquantum*6 +5, 18, 50, false); 
                MoveWindow(hwndMessageTag, 0, hquantum*4+hremainder +5, 18, hquantum-10, false); 
                MoveWindow(hwndTranslationTag, 0, hquantum*5+hremainder +5, 18, hquantum-10, false); 
                MoveWindow(hwndScrollBar, 0, 0, 18, hquantum*4+hremainder, false);
                 
           break;
           }

           case WM_VSCROLL:
           {  
                if ((HWND) lParam == hwndScrollBar)
                {
                    switch(LOWORD(wParam))
                    {                      
                      case SB_LINEUP:
                           if (scrolllefttimer <= (GetTickCount()/10))
                              scrolllefttimer = GetTickCount()/10 + 25; //NOTE: scroll bar delay time
                           else break;
                      case SB_PAGEUP:  
                           if (ScrollBar_GetPos(hwndScrollBar) > 0)
                              updateStrIndex(hwndMain, ScrollBar_GetPos(hwndScrollBar) -1);        
                           break;
                      case SB_LINEDOWN:       
                           if (scrollrighttimer <= (GetTickCount()/10))
                              scrollrighttimer = GetTickCount()/10 + 25;  //NOTE: scroll bar delay time
                           else break;
                      case SB_PAGEDOWN:  
                           if (ScrollBar_GetPos(hwndScrollBar) < (msgObj->getPhrasesCount() -1))
                              updateStrIndex(hwndMain, ScrollBar_GetPos(hwndScrollBar) +1);       
                           break;
                      //case SB_THUMBPOSITION: 
 
                      case SB_THUMBTRACK:
                           updateStrIndex(hwndMain, HIWORD(wParam));    
                           break;
                      case SB_ENDSCROLL:
                           scrolllefttimer = 0;
                           scrollrighttimer = 0;
                           break; 
                    }  
                } 
                break;  
          }   
           case WM_COMMAND:
                                                
                if (HIWORD(wParam) == EN_CHANGE && (((HWND) lParam == hwndEdit) || ((HWND) lParam == hwndComment)))
                {
                   datasaved = false;
                   EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);
                   
                   char textdata[10240];
                   GetWindowText((HWND) lParam, textdata, 10240);
                   LV_ITEM lvi;
                   lvi.pszText = textdata;
                   lvi.cchTextMax = 10240;                   
                   lvi.mask = LVIF_TEXT;
                   lvi.iItem = ListView_GetNextItem(hwndListV, -1, LVNI_SELECTED);
                   lvi.iSubItem = (((HWND) lParam == hwndEdit)?7:8);
                   ListView_SetItem(hwndListV, &lvi);
                   
                   return 0;
                }
    
              break;
           //case WM_REFLECT:     ????
           case WM_NOTIFY:
              if  (((LPNMHDR)lParam)->hwndFrom == hwndListV)
                switch (((LPNMHDR)lParam)->code)
                {      
                  case NM_CUSTOMDRAW:
                       if (msgObj && ((LPNMLVCUSTOMDRAW) lParam))
                          if (((LPNMLVCUSTOMDRAW) lParam)->nmcd.dwItemSpec == msgObj->currentString()) 
                            { 
                               ((LPNMLVCUSTOMDRAW) lParam)->clrTextBk = RGB(0,0,0);
                               ((LPNMLVCUSTOMDRAW) lParam)->clrText = RGB(255,255,255);
                            }

                       return CDRF_NOTIFYITEMDRAW;  
                       
                  case NM_SETFOCUS:    //we don't want to let listview have focus (keyup notifies are not sent to parent)
                       //if (actualclone!=-1)
                          SetFocus(hwndComment);
                       //else   
                          SetFocus(hwndEdit);
                       break;
                   
                  case NM_CLICK:
                  case NM_RCLICK:
                  case LVN_BEGINDRAG:
                  case LVN_BEGINRDRAG:
                  case LVN_KEYDOWN:
                  //case LVN_ITEMACTIVATE:
                   {    

                      if (msgObj) {
                          LV_ITEM lvi;
                          lvi.mask = LVIF_PARAM;
                          lvi.iItem = ListView_GetNextItem(hwndListV, -1, LVNI_SELECTED);
                          lvi.iSubItem = 0;
                          ListView_GetItem(hwndListV, &lvi);

                          updateStrIndex(hWnd, msgObj->findListIndex(lvi.lParam));

                          InvalidateRect(hwndListV, NULL, false);
                      }
                   
                   }
                   break;
                  case LVN_COLUMNCLICK:
                   {    
                      if (msgObj)
                        switch (((LPNM_LISTVIEW) lParam)->iSubItem)
                        {
                           case 0: //index
                           case 1: //noun
                           case 2: //verb
                           case 3: //case
                           case 5: //talker   
                                
                                SortMyListView(hWnd, ((LPNM_LISTVIEW) lParam)->iSubItem);
                                                                              
                        }
                   
                   }
                   break;
                    
                }    
             break;
                     
           default:                   /* for messages that we don't deal with */
              return DefWindowProc(hWnd, message, wParam, lParam);
    }

     return 0;
}

/* This function is called by the Windows function DispatchMessage( ) */
LRESULT CALLBACK WindowBarProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
     switch (message)                  /* handle the messages */
    {
           case WM_SIZE:
           {
                long vsize = (LOWORD(lParam)-360)/4;
                long vadd = vsize -105;
  
                MoveWindow(hwndNoun, 90, 2, vsize, 19, false);
                MoveWindow(hwndVerb, vadd+280, 2, vsize, 19, false);
                MoveWindow(hwndCase, (2*vadd)+465, 2, vsize, 19, false);
                MoveWindow(hwndTalker, (3*vadd)+660, 2, vsize, 19, false);

                MoveWindow(hwndVerbLabel, vadd+210, 4, 40, 20, false);
                MoveWindow(hwndCaseLabel, (2*vadd)+400, 4, 40, 20, false);
                MoveWindow(hwndTalkerLabel, (3*vadd)+585, 4, 40, 20, false);
 
                MoveWindow(hwndVerbIndex, vadd+250, 4, 20, 20, false);
                MoveWindow(hwndCaseIndex, (2*vadd)+435, 4, 20, 20, false);
                MoveWindow(hwndTalkerIndex, (3*vadd)+630, 4, 20, 20, false);
   
                                
           break;
           }
    
           case WM_COMMAND:
        
                if ((HIWORD(wParam) == EN_CHANGE) && (
                   ((HWND)lParam == hwndNoun && GetFocus() == hwndNoun    )
                || ((HWND)lParam == hwndVerb  && GetFocus() == hwndVerb   )
                || ((HWND)lParam == hwndCase  && GetFocus() == hwndCase  )
                || ((HWND)lParam == hwndTalker && GetFocus() == hwndTalker )))
                {

                   datasaved = false;
                   EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);

                   char textdata[100];
                   GetWindowText((HWND) lParam, textdata, 100);
                   
                   
                   if ((HWND) lParam == hwndNoun)
                           UpdateListLabels(msgObj->getNoun(msgObj->currentString()), textdata, 1);   
                   else if ((HWND) lParam ==  hwndVerb)
                           UpdateListLabels(msgObj->getVerb(msgObj->currentString()), textdata, 2); 
                   else if ((HWND) lParam == hwndCase)
                           UpdateListLabels(msgObj->getCase(msgObj->currentString()), textdata, 3); 
                   else if ((HWND) lParam == hwndTalker)
                           UpdateListLabels(msgObj->getTalker(msgObj->currentString()), textdata, 5); 
                   

                   return 0;
                }
                                             
                                
                break;
           default:                   /* for messages that we don't deal with */
           return DefWindowProc(hWnd, message, wParam, lParam);
    }
 return 0;
}



/* This function is called by the Windows function DispatchMessage( ) */
LRESULT CALLBACK WinStatusBarProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
        
    switch (message)                  /* handle the messages */
    {
           case WM_SIZE:
           {
                long vsize = (LOWORD(lParam)-570)/2;
                long vadd = vsize -105;
  
                MoveWindow(hwndFindText, 80, 4, vsize, 18, false);
                MoveWindow(hwndReplaceText, vadd+630, 4, vsize, 18, false);
                MoveWindow(hwndReplaceButton, vadd+534, 4, 90, 18, false);
                MoveWindow(hwndCaseButton, vadd+410, 3, 110, 20, false);
                MoveWindow(hwndPhraseButton, vadd+220, 3, 80, 20, false);
                MoveWindow(hwndEditButton, vadd+300, 3, 90, 20, false);
                                   
           break;
           }

           case WM_COMMAND:
                
                if (HIWORD(wParam) == EN_CHANGE && (HWND)lParam == hwndFindText)
                {
                   szFirstFindFileName[0] = 0; //resets the search
                   return 0;
                }
                
                 /* Check the control ID, notification code and
                 * control handle to see if this is a button click
                 * message from our child button. */

                if (HIWORD(wParam) == BN_CLICKED)
                {                    
                                                           
                    if (LOWORD(wParam) == BTNPHRASE_ID &&    
                              (HWND) lParam == hwndPhraseButton)
                    {
                        searchoriginal=true;
                        
                        EnableWindow(hwndFindText, true);
                        EnableWindow(hwndReplaceText, false);
                        EnableWindow(hwndFindButton, true);
                        EnableWindow(hwndReplaceButton, false);
                        EnableMenuItem(menu, IDM_FIND, MF_ENABLED);
                        EnableMenuItem(menu, IDM_REPLACE, MF_GRAYED);
                        SetFocus(hwndFindText); 
                        
                        return 0;
                    }
                    if (LOWORD(wParam) == BTNEDIT_ID &&    
                              (HWND) lParam == hwndEditButton)
                    {
                        searchoriginal=false;
                       
                        EnableWindow(hwndFindText, true);
                        EnableWindow(hwndReplaceText, true);
                        EnableWindow(hwndFindButton, true);
                        EnableWindow(hwndReplaceButton, true);
                        EnableMenuItem(menu, IDM_FIND, MF_ENABLED);
                        EnableMenuItem(menu, IDM_REPLACE, MF_ENABLED);
                        SetFocus(hwndFindText);   
                           
                        return 0;
                    }
                    if (LOWORD(wParam) == BTNCASE_ID &&    
                              (HWND) lParam == hwndCaseButton)
                    {
                        searchcase=(searchcase? false: true);
                             
                        return 0;
                    }
                    if (LOWORD(wParam) == BTNFIND_ID &&    
                              (HWND) lParam == hwndFindButton)
                    {
                        char searchStr[10240];
                        
                        Edit_GetText(hwndFindText, searchStr, 10240);
                       
                        if (searchStr[0]!=0) {
                           do { 
                              int selStart, selEnd, searchpos;                        
                              char textStr[10240];
                              HWND temphwnd = (searchoriginal ?hwndPhrase :hwndEdit);
                           
                              //SetFocus(temphwnd); 
                              selStart = Edit_GetSel(temphwnd);
                              selEnd=HIWORD(selStart);
                              selStart=LOWORD(selStart);
                              searchpos =((selStart!=selEnd) ?selEnd :selStart);
                           
                              Edit_GetText(temphwnd, textStr, 10240);    
                              if (findText((char *)textStr, searchpos, (char *)searchStr, searchcase)) {
                                 Edit_SetSel(temphwnd, searchpos, (int)(searchpos+strlen(searchStr)));
                                 SetFocus(temphwnd); 
                    
                                 break;
                              }
                              //no occurrence in current phrase
                              //---support for multiple file search
                              if (msgObj->currentString()+1 < msgObj->getPhrasesCount()) {
                                 updateStrIndex(hwndMain, msgObj->currentString()+1); 
                              } else { 
                                 int btn = DialogBox(hInst, (LPCTSTR)IDD_SEARCHDIALOG, hwndMain, (DLGPROC)SearchStatus);
                                 
                                 switch (btn)
                                 {
                                 case IDYES:
                                          if (DoNextFile(hwndMain))
                                          {
                                             updateStrIndex(hwndMain, 0);
                                          }
                                          else
                                              goto searchexit;
                                          break;
                                 case IDNO:
                                          updateStrIndex(hwndMain, 0);
                                          break;
                                 default:
                                         goto searchexit;
                                 } 
                              }
                           } while (true);
                           
                        }     
                        searchexit:                 
                        return 0;
                    }
                    if (LOWORD(wParam) == BTNREPLACE_ID &&    
                              (HWND) lParam == hwndReplaceButton)
                    {   
                        char replaceStr[10240];
                        
                        Edit_GetText(hwndReplaceText, replaceStr, 10240);
                   
                        if (replaceStr[0]!=0) {                       
                           HWND temphwnd = (searchoriginal ?hwndPhrase :hwndEdit);
                           int selection = Edit_GetSel(temphwnd);
                           
                           //SetFocus(temphwnd); 
                           if (LOWORD(selection)!=HIWORD(selection)) {  //if there's an actual selection to replace    
                              Edit_ReplaceSel(temphwnd, replaceStr);
                           }
                        }
                              
                        return 0;
                    }
                    
                }             
    
           break;
           default:                   /* for messages that we don't deal with */
           return CallWindowProc(g_pfnWndProcOrig, hWnd, message, wParam, lParam);


    }

     return 0;
}


/* This function is called by the Windows function DispatchMessage( ) */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    switch (message)                  /* handle the messages */
    {
           case WM_SIZE:
           {                 
                MoveWindow(hwndFrame, 0, 26, LOWORD(lParam), HIWORD(lParam)-50, false);               
                MoveWindow(hwndBarFrame, -5, 0, LOWORD(lParam)+10, 26, false); 
                MoveWindow(hwndStatusBar, 0, HIWORD(lParam)-24,LOWORD(lParam),24,false);
                InvalidateRect(hwnd, NULL, true);
                
                WINDOWPLACEMENT wndpl;
                wndpl.length = sizeof(WINDOWPLACEMENT); 
                GetWindowPlacement(hwnd, &wndpl);
                windowRect = wndpl.rcNormalPosition;
      
                break;  
                               
           }       
           case WM_SIZING: 
           {    //NOTE: control for undersizing           
                if ((((RECT *) lParam)->right - ((RECT *) lParam)->left) < WIN_WIDTH)   
                   ((RECT *) lParam)->right = ((RECT *) lParam)->left + WIN_WIDTH;
                if ((((RECT *) lParam)->bottom - ((RECT *) lParam)->top) < WIN_HEIGHT)
                   ((RECT *) lParam)->bottom = ((RECT *) lParam)->top + WIN_HEIGHT; 
                
                break;
           }
           case WM_PAINT:
                
                /* The window needs to be painted (redrawn). */
                hdc = BeginPaint (hwnd, &ps);
       /*         GetClientRect (hwnd, &rc);  
                                
                if ((msgObj != 0) && unkbytesdecodinginfo)
                {
                  char rinfo[256];
                          
                  sprintf(rinfo, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n\n",    //TODO hide those not relevant to older sci files !!
                               msgObj->getUnkByte(msgObj->currentString(),0),
                               msgObj->getUnkByte(msgObj->currentString(),1),
                               msgObj->getUnkByte(msgObj->currentString(),2),
                               msgObj->getUnkByte(msgObj->currentString(),3),
                               msgObj->getUnkByte(msgObj->currentString(),4),
                               msgObj->getUnkByte(msgObj->currentString(),5),
                               msgObj->getUnkByte(msgObj->currentString(),6),
                               msgObj->getUnkByte(msgObj->currentString(),7),
                               msgObj->getUnkByte(msgObj->currentString(),8)                          
                               );                                        
                  SetRect(&rc, 50, 470, 100, 800);   
                  DrawText (hdc, rinfo, -1, &rc, DT_LEFT | DT_TOP);
                  
                  sprintf(rinfo, "%d\n%d\n%d\n%d\n%d\n%d",       //TODO hide those not relevant to older sci files !!
                               msgObj->getComUnkByte(msgObj->currentString(),0),
                               msgObj->getComUnkByte(msgObj->currentString(),1),
                               msgObj->getComUnkByte(msgObj->currentString(),2),
                               msgObj->getComUnkByte(msgObj->currentString(),3),
                               msgObj->getComUnkByte(msgObj->currentString(),4),
                               msgObj->getComUnkByte(msgObj->currentString(),5)                           
                               );                                        
                  SetRect(&rc, 140, 470, 200, 800);   
                  DrawText (hdc, rinfo, -1, &rc, DT_LEFT);              
                }
                      
                */                      
                EndPaint (hwnd, &ps);
           break;
           case WM_CLOSE:
                if ( exit_proc(hwnd) )
                   DestroyWindow(hwnd);
           break;
           case WM_DESTROY:
                if (msgObj)
                   delete msgObj;
                PostQuitMessage(0);  /* send a WM_QUIT to the message queue */
           break;
           case WM_COMMAND:
                          
                switch(LOWORD(wParam))    //CHECK must be checked if HIword is 0??
                {
                 case IDM_FILEOPEN:
                    DoFileOpen(hwnd, NULL, false);
                    break;
                 case IDM_FILEOPENVOL:
                 {
                    char exFile[MAX_PATH]="";
                    if (ExtractFromVolume(hwnd, NULL, 0x8F, (char*)"msg", exFile))
                       DoFileOpen(hwnd, exFile, false);         
                    break;
                 }
                 case IDM_FILESAVE:
                    DoFileSave(hwnd);
                    break;
                 case IDM_FILESAVEAS:
                    DoFileSaveAs(hwnd);
                    break;
                 case IDM_IMPORTFILE:
                    DoMsgImport(hwnd);
                    break;
                 case IDM_IMPORTTEXT:
                    if (ImportTextFile(hwnd, msgObj))
                    {
                       datasaved = false;
                       EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);

                       updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
                    }
                    break;   
                 case IDM_EXPORTTEXT:
                    DoFileExport(hwnd);
                    break;
                 case IDM_COMPAREWITH:
                      DoFileCompare(hwnd);
                      break;
                 case IDM_FILEABOUT:
                 
                      switch (DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hwnd, (DLGPROC)AboutBox)) 
		              {
            
                             case IDC_LOADMSG:
                                  PostMessage(hwndMain, WM_COMMAND, IDM_FILEOPEN, 0);
                                  break;
                             case IDC_LOADRES: 
                                  PostMessage(hwndMain, WM_COMMAND, IDM_FILEOPENVOL, 0);
                                  break;
                             case IDC_MANUAL:
                                  PostMessage(hwndMain, WM_COMMAND, IDM_HELP, 0);
                                  break;
                             
                             case IDC_CLOSEDLG: 
                             default: 
                                  break;                                
                      }
                      break;
                 
                 case IDM_FILEEXIT:
                    if ( exit_proc(hwnd) )
                       DestroyWindow(hwnd);
                    break;
                /* case IDM_EDITUNDO:
                   break;
                 case IDM_EDITCUT:
                      break;
                 case IDM_EDITCOPY:
                       break;   
                 case IDM_CHOOSEFONT:
                      ChooseNewFont(hwndEdit);     
                      break; 
                      */
                 case IDM_FIRST:
                      updateStrIndex(hwnd, 0);

                      break;
                 case IDM_LEFT:
                      updateStrIndex(hwnd, msgObj->currentString() -1);

                      break;
                 case IDM_RIGHT:
                      updateStrIndex(hwnd, msgObj->currentString() +1);
                      
                      break;
                 case IDM_LAST:
                      updateStrIndex(hwnd, msgObj->getPhrasesCount() -1);

                      break;
                   
                 case IDM_FIND:
                      PostMessage(hwndFindButton, BM_CLICK, 0, 0);                      

                      break;
                 case IDM_REPLACE:
                      PostMessage(hwndReplaceButton, BM_CLICK, 0, 0);                      

                      break;
                 case IDM_FONT14:
                 case IDM_FONT16:
                 case IDM_FONT18:
                 case IDM_FONT20:
                 case IDM_FONT22:
                 case IDM_FONT24:
                 case IDM_FONT26:     
                 
                      setStringsFont(LOWORD(wParam)-IDM_FONT);
                      
                      break;
                      
                 case IDM_DRAWGF:
                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                         DrawNounGraph(hwnd, msgObj->sorting(), true);

                      break; 
                 case IDM_DRAWGFBW:
                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                         DrawNounGraph(hwnd, msgObj->sorting(), false);

                      break; 
     
                 case IDM_DRAWNOUNGF:
                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
            
                         DrawNounGraph(hwnd, ByNoun, true);
                      break;     
                 case IDM_DRAWCASEGF:
                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                         DrawNounGraph(hwnd, ByCase, true);
                      break;  
                 case IDM_DRAWVERBGF:
                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
            
                         DrawNounGraph(hwnd, ByVerb, true);
                      break;     
                 case IDM_DRAWTALKERGF:
                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                         DrawNounGraph(hwnd, ByTalker, true);
                      break; 
                 case IDM_DRAWINDEXGF:
                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                         DrawNounGraph(hwnd, ByIndex, true);
                      break; 
                 case IDM_ADDTOWL: //add to word lookup (prontuario)
                   {
                      int selection = Edit_GetSel(hwndPhrase);
                      int selection2 = Edit_GetSel(hwndEdit);
                      if ((LOWORD(selection)!=HIWORD(selection)) &&
                         (LOWORD(selection2)!=HIWORD(selection2)))
                      {  //if there's actually a selection   
                                            
                         char phraseStr[256] = "", editStr[256] = "";
                         Edit_GetText(hwndPhrase, phraseStr, 256);
                         memmove(phraseStr, phraseStr+LOWORD(selection), HIWORD(selection)-LOWORD(selection));
                         phraseStr[HIWORD(selection)-LOWORD(selection)] = 0;
                         
                         Edit_GetText(hwndEdit, editStr, 256);
                         memmove(editStr, editStr+LOWORD(selection2), HIWORD(selection2)-LOWORD(selection2));
                         editStr[HIWORD(selection2)-LOWORD(selection2)] = 0;
                         
                         char wlfile[MAX_PATH] = "";
                         sprintf(wlfile, "%s\\wordlookup.txt", szFilePath); 
                                             
                         
                         switch (addWordToWL(wlfile, phraseStr, editStr))
                         {
                            case -1:    
                                      if(IDYES == MessageBox (hwnd, WORDS_WORDALREADYEXISTS, WARN_ATTENTION, 
                                                           MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO))
                                      {                     
                                            if (appendWordToWL(wlfile, phraseStr, editStr))                                           
                                               break;
                                      } else break;
                            case -2:  MessageBox(hwnd, WORDS_CANTACCESSWORDSFILE, ERR_TITLE,
                                                MB_OK | MB_ICONEXCLAMATION);
                                      break;

                         }
                      } else
                        MessageBox(hwnd, WORDS_NOSELECTIONTOADD, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);

                      
                      break;
                    } 
                 case IDM_DOWORDSLOOKUP:
                 {
                      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
                      
                      char wlfile[MAX_PATH] = "";
                      sprintf(wlfile, "%s\\wordlookup.txt", szFilePath); 
   
                      doWordsLookup(hwnd, wlfile);
                      break;  
                 }
                 case IDM_SHOWWL:
                 {
                      char wlfile[MAX_PATH] = "";
                      sprintf(wlfile, "%s\\wordlookup.txt", szFilePath); 
                      
                      ShellExecute(hwnd, "edit", wlfile, NULL, NULL, SW_SHOW);
                      
                      break;
                 }
                 case IDM_HELP:
                              
                         ShellExecute(hwnd, "open", MANUAL_PATH, NULL, szAppDir, SW_SHOW);
                      break; 
                 case IDM_NOUNSFROMHEAP:
                      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                      if (LoadNounsFromHEAP(hwnd))
                      {
                         SetWindowText(hwndNoun, msgObj->getNounName(msgObj->getNoun(msgObj->currentString())));
                      
                         datasaved = false;
                         EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);

                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
                      }
                      break;
                 /*case IDM_SHOWUNKBYTES:
                      unkbytesdecodinginfo = !unkbytesdecodinginfo;
                      CheckMenuItem(menu, IDM_SHOWUNKBYTES, (unkbytesdecodinginfo?MF_CHECKED:MF_UNCHECKED));  
                      SetRect(&rc, 0, 470, 200, 800);                       
                      InvalidateRect(hwndMain, &rc, true); //force repaint  
                      break;   */
                 case IDM_PROPERTIES:
                   {
                      char version[32];
                      versionString(version);
                      char format[32];
                      switch (msgObj->tradusciFormat())
                      {
                        case SIERRA:
                           memcpy(format, "Sierra Message", 15);
                           break;     
                        case OLDTRADUSCI:
                           memcpy(format, "TraduSCI 1.0", 13);
                           break;
                        case TRADUSCI2:
                           memcpy(format, "TraduSCI2", 10);
                      }
                      char propertext[1000]; //CHECK 1000 is enough ?
                      sprintf(propertext, INTERFACE_PROPERTIESFORMATSTRING, version, format, msgObj->getPhrasesCount(), msgObj->getUnkCount(), (msgObj->hasComments()?EXPORT_YES:EXPORT_NO), (msgObj->getAudMapName()[0]!=0 ?msgObj->getAudMapName():MAPS_NOTLOADED));
                      MessageBox (hwnd, propertext, INTERFACE_PROPERTIESTITLE, /*MB_ICONINFORMATION*/ MB_USERICON | MB_OK); //
                      
                      //TODO when opening the file one must check if the MAP exists,
                      //and if its content is the same as that stored into the msg. Warn the user if it's different! 
                      break;
                   }   
                 case IDM_LOADMAP:
                  
                    ImportAudMapStub(hwnd);
  
                    break;  
                  
                 case IDM_EXPORTMAP:
                  {         
                    ProgramAudMap *pam =  msgObj->getAudMap();
                    if (pam != 0)
                       ExportAudMAP(hwnd, pam, 0);   //TODO handle bool result?
                 
  
                    break;  
                  }
                 case IDM_VERIFYMAP:
                   
                    VerifyAudMap(hwnd, msgObj, 0);
                    break;
                 case IDM_LOADAUDRES:
                 {
                       OPENFILENAME ofn;
                       char szAudFileName[MAX_PATH] = "RESOURCE.AUD";
   
                       ZeroMemory(&ofn, sizeof(ofn));

                       ofn.lStructSize = sizeof(ofn);
                       ofn.hwndOwner = hwnd;
                       ofn.lpstrFilter = INTERFACE_OPENAUDRESFILTER; 
                       ofn.lpstrFile = szAudFileName;
                       ofn.nMaxFile = MAX_PATH;

                       ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOVALIDATE;
                       if (GetOpenFileName(&ofn)!=0)
                       {
                           char pfile[MAX_PATH] = "";
                           sprintf(pfile, "%s\\RESOURCE_AUD.path", szFilePath); 
                           FILE *inifile = fopen(pfile, "w");
                           if (inifile)
                           {
                              fprintf(inifile, szAudFileName);
                              fclose(inifile);
                           }   
                       }  

                 }                   
                      break;
                 case IDM_PLAY:
           
                      PlayAudio(hwnd, false);
           
                      break;
                 case IDM_AUTOPLAY:
                      autoplay = !autoplay;
                      CheckMenuItem(menu, IDM_AUTOPLAY, (autoplay?MF_CHECKED:MF_UNCHECKED));  

                     break;    
                 case IDM_NEXTCOMMENT:
                  {
                     unsigned short i = msgObj->currentString() + 1;
                     while (i<msgObj->getPhrasesCount())
                     {
                           if (msgObj->getComment(i)!=0)
                              if ((msgObj->getComment(i))[0] != 0)
                              {
                                 updateStrIndex(hwnd, i);
                                 break;
                              }   
                           i++;
                     } 
                     break;        
                  }
                 case IDM_LOADLABELSTXT:
                      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                      if (ImportLabels(hwnd, "labels.txt"))
                      {
                         SetWindowText(hwndNoun, msgObj->getNounName(msgObj->getNoun(msgObj->currentString())));
                         SetWindowText(hwndVerb, msgObj->getVerbName(msgObj->getVerb(msgObj->currentString())));
                         SetWindowText(hwndCase, msgObj->getCaseName(msgObj->getCase(msgObj->currentString())));
                         SetWindowText(hwndTalker, msgObj->getTalkerName(msgObj->getTalker(msgObj->currentString())));  
                      
                         datasaved = false;
                         EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);

                         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
                      }
                      break;
                 case IDM_EXPORTNOUNS:
                      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                      ExportLabels(hwnd, 1, "nouns.txt");
                      break;
                 case IDM_EXPORTVERBS:
                      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                      ExportLabels(hwnd, 2, "verbs.txt");
                      break;   
                 case IDM_EXPORTCASES:
                      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                      ExportLabels(hwnd, 3, "cases.txt");
                      break;
                 case IDM_EXPORTTALKERS:
                      updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

                      ExportLabels(hwnd, 4, "talkers.txt");
                      break;  
                 case IDM_CLONE:
                      DoCloning(hwndMain);
                      break;              
                }
                
           break;
           default:                   /* for messages that we don't deal with */
           return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}


bool exit_proc(HWND hwnd)
{
     if (msgObj && !datasaved)
     {    
         updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
         int btn = MessageBox (hwnd, WARN_QUITONUNSAVED, WARN_ATTENTION, 
                                        MB_APPLMODAL | MB_ICONQUESTION | MB_YESNOCANCEL);
         switch (btn)
         {
            case IDYES:
                 if (!DoFileSave(hwnd))
                    return false;
                 break;
            case IDNO:
                 break;      
            default:
                    return false;    
         }
         
         
      }
      remove(szTempFileName);
      return true;
}



