/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	msg2txt.c   -  export and import sentences to text file
 * 
 */

#include "common.h"

             
bool ExportFile(HWND hwnd, LPSTR pszFileName)
//NOTE: since we export text for the matter of grammar checking,
//      we prefer to not export nouns, verbs, too, as they would make the checking process longer                                                                                    
{
    updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings
                                                  
    FILE *cfilebuf = fopen(pszFileName, "w");
    if (cfilebuf)
    {

       //List<StringItem> *strList = msgObj->getStringList();
       for (unsigned short i = 0; i < msgObj->getPhrasesCount() ; i++)
       {   
           long pos = msgObj->findListIndex(i);
           char packedlabel[20];
           sprintf(packedlabel,"%03d%03d%03d%03d", msgObj->getNoun(pos),msgObj->getVerb(pos),msgObj->getCase(pos),msgObj->getSeq(pos));
           fprintf(cfilebuf,"%s\n%s%s%s\n\n", packedlabel, (msgObj->getTranslation(pos) ?msgObj->getTranslation(pos):""),(msgObj->getTranslation(pos) ?"\n":""),packedlabel);        
       }
       
       fprintf(cfilebuf, EXPORT_ENDSECTION);
       fprintf(cfilebuf, EXPORT_INFO);
       fprintf(cfilebuf, EXPORT_COPYRIGHT);
       fprintf(cfilebuf, EXPORT_FILEPATH, szFileName);
       char format[32];
       switch (msgObj->tradusciFormat()) {
       case SIERRA:
            memcpy(format, "Sierra Message", 15);
            break;     
       case OLDTRADUSCI:
            memcpy(format, "TraduSCI 1.0", 13);
            break;
       case TRADUSCI2:
            memcpy(format, "TraduSCI2", 10);
       }
       fprintf(cfilebuf, EXPORT_FORMAT, format);
       char version[32];
       versionString(version);
       fprintf(cfilebuf, EXPORT_VERSION, version);
       fprintf(cfilebuf, EXPORT_ITEMCOUNT, msgObj->getPhrasesCount() );
       fprintf(cfilebuf, EXPORT_COMMENTS, (msgObj->hasComments() ? EXPORT_YES : EXPORT_NO) );
              
       fprintf(cfilebuf, "\n\n\n");
       
       //List<CommentItem> *comList = msgObj->getCommentList();
       for (unsigned short i = 0; i < msgObj->getPhrasesCount() ; i++) {
           fprintf(cfilebuf, EXPORT_ITEM2, i+1, msgObj->getPhrase(msgObj->findListIndex(i)));

           if (msgObj->getTranslation(msgObj->findListIndex(i)))
              fprintf(cfilebuf, EXPORT_TRANSLATION, msgObj->getTranslation(msgObj->findListIndex(i)));
   
           if (msgObj->hasComments() && strcmp(msgObj->getComment(msgObj->findListIndex(i)), ""))
              fprintf(cfilebuf, EXPORT_COMMENTITEM, msgObj->getComment(msgObj->findListIndex(i)));

           fprintf(cfilebuf, "\n\n");
       }

       fprintf(cfilebuf, EXPORT_ENDOFFILE);

       fclose(cfilebuf);
       
       return TRUE;        
    }
    return FALSE;
}

//TODO for old sci, we do not allow import for now (doesn't have enough labels)
bool ImportTextFile(HWND hwnd, MsgData *mdo)
{
   bool changed = false;    //at least one string has been changed
   updateStrIndex(hwnd, msgObj->currentString());  //to write in memory current editings

   OPENFILENAME ofn;
   char szTextFileName[MAX_PATH] = "";
   
   ZeroMemory(&ofn, sizeof(ofn));
    
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = INTERFACE_EXPORTTEXTFILTER; 
   ofn.lpstrFile = szTextFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrDefExt = "txt";

   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOVALIDATE;
   
   if (GetOpenFileName(&ofn)==0)
         return changed;
         
   FILE *cfilebuf = fopen(szTextFileName, "r");
   if (cfilebuf)
   {      
       char readdata[10240] = "", storedata[10240] = "";
       for (unsigned short i = 0; i < mdo->getPhrasesCount() ; i++)
       {   
           long pos = mdo->findListIndex(i);
           char packedlabel[20]="", packedlabel2[20]="";
           sprintf(packedlabel,"%03d%03d%03d%03d\n", mdo->getNoun(pos),mdo->getVerb(pos),mdo->getCase(pos),mdo->getSeq(pos));
           
           do           //we jump CRs
             fgets(packedlabel2,20,cfilebuf);
           while (strcmp(packedlabel2,"\n")==0);
           
           if (strcmp(packedlabel,packedlabel2)!=0)
           { 
             char errorthing[100]="";
             sprintf(errorthing, IMPORT_CANTFINDLABEL, packedlabel, packedlabel2);
             MessageBox(hwnd, errorthing, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
             return changed;
           }
           
           readdata[0] = 0;
           storedata[0] = 0;
           
           do {   
              fgets(readdata,10240,cfilebuf);
              if (strcmp(packedlabel,readdata)==0)
                 break;
              
              strcat(storedata, readdata);      
           } while (!feof(cfilebuf));   
           
           if (storedata[0] != 0)
           {
              storedata[strlen(storedata)-1] = 0 ; //to remove last CR character 
              mdo->setTranslation(pos, storedata);
              changed = true;

              LV_ITEM lvi;
              lvi.pszText = storedata;
              lvi.cchTextMax = 10240;                   
              lvi.mask = LVIF_TEXT;
              lvi.iItem = pos;
              lvi.iSubItem = 7;
              ListView_SetItem(hwndListV, &lvi);
           }
        } 
         
              
                        
   
   } else
     MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION); 
                
   return changed;
}

