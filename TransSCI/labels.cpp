/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	labels.cpp  -  labels (noun verb case,..) import/export
 *	
 */

#include "common.h"

void setNounLabel(HWND hwnd, unsigned char which, char *what)
{
          char *previous = 0;
          previous = msgObj->getNounName(which);
          if (previous)
            if (strcmp(previous, what)!=0)
            {
             char question[300];
             sprintf(question, LABELS_REDEFINENAMEFROMSCRIPT, GRAPH_NOUN, which, previous, what);
             if (MessageBox (hwnd, question, WARN_ATTENTION, 
                            MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)
                  == IDNO)
                     return;
          
            }   
          msgObj->setNounName(which, what);
          UpdateListLabels(which, what, 1);
}


void setVerbLabel(HWND hwnd, unsigned char which, char *what)
{
          char *previous = 0;
          previous = msgObj->getVerbName(which);
          if (previous)
            if (strcmp(previous, what)!=0)
            {
             char question[300];
             sprintf(question, LABELS_REDEFINENAMEFROMSCRIPT, GRAPH_VERB, which, previous, what);
             if (MessageBox (hwnd, question, WARN_ATTENTION, 
                            MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)
                  == IDNO)
                     return;
          
            }   
          msgObj->setVerbName(which, what);
          UpdateListLabels(which, what, 2);         
}


void setCaseLabel(HWND hwnd, unsigned char which, char *what)
{
          char *previous = 0;
          previous = msgObj->getCaseName(which);
          if (previous)
            if (strcmp(previous, what)!=0)
            {
             char question[300];
             sprintf(question, LABELS_REDEFINENAMEFROMSCRIPT, GRAPH_CASE, which, previous, what);
             if (MessageBox (hwnd, question, WARN_ATTENTION, 
                            MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)
                  == IDNO)
                     return;
          
            }   
          msgObj->setCaseName(which, what);
          UpdateListLabels(which, what, 3);
}



void setTalkerLabel(HWND hwnd, unsigned char which, char *what)
{
          char *previous = 0;
          previous = msgObj->getTalkerName(which);
          if (previous)
            if (strcmp(previous, what)!=0)
            {
             char question[300];
             sprintf(question, LABELS_REDEFINENAMEFROMSCRIPT, GRAPH_TALKER, which, previous, what);
             if (MessageBox (hwnd, question, WARN_ATTENTION, 
                            MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)
                  == IDNO)
                     return;
          
            }   
          msgObj->setTalkerName(which, what);
          UpdateListLabels(which, what, 5);
}


bool ImportLabels(HWND hwnd, const char *importfile)
{
  bool changes = false;
  
  OPENFILENAME ofn;
  char szTextFileName[MAX_PATH] = "";
  strcpy(szTextFileName, importfile);
   
  ZeroMemory(&ofn, sizeof(ofn));
    
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = INTERFACE_EXPORTTEXTFILTER; 
  ofn.lpstrFile = szTextFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrDefExt = "txt";

  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOVALIDATE;
   
  if (GetOpenFileName(&ofn)==0)
         return changes;

  
  FILE *cfilebuf = fopen(szTextFileName, "r");
  if (cfilebuf)
  { 

      char store1[1000]="", store2[1000]="";    
      
      while (fgets(store1, 1000, cfilebuf) != NULL)
      {
         char *s = store1;
         char *s2 = store2;
         
         //remove commented stuff '#' 
         if (strchr(s, '#'))
         {
            strncpy(s2, s, strchr(s, '#')-s); s2[strchr(s, '#')-s] = 0;
         } else
           strcpy(s2, s);


         //magic
         if (strchr(s2, '='))
         {
            while (((unsigned char *)s2)[0] <= 32)     //remove spaces before
                  s2++;
            while (((unsigned char *)s2)[strlen(s2)-1] <= 32)
                  s2[strlen(s2)-1]= 0; //remove spaces and CR after
            
            char *org = s2;     

            unsigned short defined=0;      
            sscanf(s2, "%*s %d", &defined);
            
            s2 = strchr(s2, '=')+1;  
         
            while (((unsigned char *)s2)[0] <= 32)     //remove spaces before
               s2++;
               
            if (defined<256 && defined != 0)
            {  
               changes = true;
               if (strncasecmp(s, "noun", 4)==0)
                  setNounLabel(hwnd, defined, s2);     
               else if (strncasecmp(s, "case", 4)==0)
                  setCaseLabel(hwnd, defined, s2);
               else if (strncasecmp(s, "verb", 4)==0)
                    setVerbLabel(hwnd, defined, s2);
               else if (strncasecmp(s, "talker", 6)==0)
                    setTalkerLabel(hwnd, defined, s2);
               else
                   changes = false;
            }  

         } //else the row is not a label definition, JUMPing


      } 

    fclose(cfilebuf);
     
  } else MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
  
  return changes;   
}

char *getLabelName(char type, unsigned char which)
{
     char *ret =0;
     switch (type)
     {
         case 1:
              ret = msgObj->getNounName(which);
              break;
         case 2:
              ret = msgObj->getVerbName(which);
              break;
         case 3:
              ret = msgObj->getCaseName(which);
              break;
         case 4:
              ret = msgObj->getTalkerName(which);
              break;
         default:
         break;//TODO?
     }
     
     if (ret)
        if (ret[0] == 0)
           ret = 0;
                   
     return ret;
}



long searchLabelInTXT(HWND hwnd, char *filename, char type, unsigned char which, char *label)
{
      FILE *cfilebuf = fopen(filename, "r");
      if (cfilebuf)
      {  
         //we must search for an already existing definition and then signal the user
         char searchtag[20];
         switch (type)
         {
                case 1:
                     strcpy(searchtag, "noun");
                     break;
                case 2:
                     strcpy(searchtag, "verb");
                     break;
                case 3:
                     strcpy(searchtag, "case");
                     break;
                case 4:
                     strcpy(searchtag, "talker");
                     break;
                default:
                //TODO
                break;
         }
                                      
         char store1[1000]="", store2[1000]="";    
         long linecount=0;
         while (fgets(store1, 1000, cfilebuf) != NULL)
         {
           linecount++;
           char *s = store1;
           char *s2 = store2;
         
           //remove commented stuff '#' 
           if (strchr(s, '#'))
           {
              strncpy(s2, s, strchr(s, '#')-s); s2[strchr(s, '#')-s] = 0;
           } else
             strcpy(s2, s);


           //magic
           if (strchr(s2, '='))
           {
              while (((unsigned char *)s2)[0] <= 32)     //remove spaces before
                  s2++;
              while (((unsigned char *)s2)[strlen(s2)-1] <= 32)
                  s2[strlen(s2)-1]= 0; //remove spaces and CR after
            
              char *org = s2;     
              
              unsigned short defined=0;      
              sscanf(s2, "%*s %d", &defined);
            
              s2 = strchr(s2, '=')+1;  
         
              while (((unsigned char *)s2)[0] <= 32)     //remove spaces before
               s2++;
               
              if (defined == which)
              {  
                  
                if (strncasecmp(s, searchtag, strlen(searchtag))==0)
                {
                  if (strcmp(s2, label) != 0)
                  {    
                      char question[300];
                      sprintf(question, LABELS_REDEFINENAMEFROMSCRIPT, GRAPH_NOUN, which, s2, label);
                      if (MessageBox (hwnd, question, WARN_ATTENTION, 
                            MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)
                            == IDNO)                             
                      {
                         fclose(cfilebuf);   
                         return -2; //dont replace
                      }
                      return linecount;         
                  }
                  else
                      {
                         fclose(cfilebuf);   
                         return -2; //dont replace  (already there!)
                      }  
                }                                   
              }  

           } //else the row is not a label definition, JUMPing


         } 

         fclose(cfilebuf);                                            
      } //else return -20;

      return -1;  //not found
}


void insertLabelInTXT(HWND hwnd, char *filename, char type, unsigned char which, char *label, long linenum)
{
   unsigned long linecount = 0;
   if (linenum)
   {                               
      FILE *cfilebuf = fopen(filename, "r");
      if (cfilebuf)
      {
         fseek(cfilebuf, 0, SEEK_END);
         long filesize = ftell(cfilebuf);
         if (filesize)
         {  
            char store1[1002] = "";
            char *filedata = new char [filesize + strlen(label) + 20]; 
            if (filedata)
            {
               filedata[0] = '\0';
               fseek(cfilebuf, 0, SEEK_SET);
               while (fgets(store1, 1000, cfilebuf) != NULL)
               {
                  linecount++;
                  if (linecount == linenum)
                  {
                     char scomments[1000] = ""; 
                     if (strchr(store1, '#'))
                     {
                        scomments[0] = ' ';
                        strncpy(scomments+1, strchr(store1, '#'), 998);
                        
                     }
                     store1[strchr(store1, '=')-store1+1] = 0;
                     
                     strcat(store1, label);
                     strcat(store1, scomments);
                     if (scomments[0] == '\0')
                        strcat(store1, "\n");    //we need to add the removed \n
                  }
                  strcat(filedata, store1);
               }

               cfilebuf = freopen(filename, "w", cfilebuf);
               if (cfilebuf)
               {
     
                   fputs(filedata, cfilebuf);

                   fclose(cfilebuf);
                   
                  // delete filedata;
                  // return true;
                          
               } 
               delete filedata;
                            
            } else fclose(cfilebuf);
            
         } else fclose(cfilebuf); 
      } 
        
   }

}


void appendLabelInTXT(HWND hwnd, char *filename, char type, unsigned char which, char *label)
{
     FILE *cfilebuf = fopen(filename, "a");
     if (cfilebuf)
     {
         char stype[20];
         switch (type)
         {
                case 1:
                     strcpy(stype, "noun");
                     break;
                case 2:
                     strcpy(stype, "verb");
                     break;
                case 3:
                     strcpy(stype, "case");
                     break;
                case 4:
                     strcpy(stype, "talker");
                     break;
                default:
                     break;
         }
         fprintf(cfilebuf, "%s %d = %s\n", stype, which, label);
        
         fclose(cfilebuf);
     }
                 
}

void ExportLabels(HWND hwnd, char type, const char *exportfile)       
{   //type: 1 nouns 2 verbs 3 cases 4 talkers [0 all (TODO?)]
   OPENFILENAME ofn;
   char szTextFileName[MAX_PATH] = "";
   strcpy(szTextFileName, exportfile);
   
   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = INTERFACE_EXPORTTEXTFILTER;
   ofn.lpstrFile = szTextFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrDefExt = "txt";

   ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
               
   if(!GetSaveFileName(&ofn))
       return;                         
                
   
   //FILE *cfilebuf;
   for (unsigned short i = 1; i<256; i++)
     if (getLabelName(type, i)!=0)
     {         
         long position =  searchLabelInTXT(hwnd, szTextFileName, type, i, getLabelName(type, i));
         switch (position)
         {
            case -1: //not found
                 appendLabelInTXT(hwnd, szTextFileName, type, i,  getLabelName(type, i));
                 break; 
            case -2:  //dont replace
                 break;
            
           /* case -20: //file error
                 MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
                 return;
             */
            default:
                 insertLabelInTXT(hwnd, szTextFileName, type, i,  getLabelName(type, i), position);
      


         }
     }
         
                                                                                                                                                
}



