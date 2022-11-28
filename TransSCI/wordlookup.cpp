/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	Used to keep a Glossary of words/sentences translations
 *	
 */
 
#include "common.h"


unsigned long searchWordInWL(char *lookupfile, char *originalStr) //, char *translatedStr)
{
  FILE *cfilebuf = fopen(lookupfile, "r");
  if (cfilebuf)
  {  
      char store1[1000]="", store2[1000]="", store3[1000]="";   
      unsigned long linecount = 0;
      
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
              
            char org[1000];     
            strncpy(org, s2, strchr(s2, '=')-s2);  org[strchr(s2, '=')-s2] = 0;  
            while (((unsigned char *)org)[strlen(org)-1] <= 32)
                  org[strlen(org)-1]= 0; //remove spaces after
                  
            char *strapp = store3;
            strcpy(strapp, originalStr);
            while (((unsigned char *)strapp)[0] <= 32)     //remove spaces before
                  strapp++;
            while (((unsigned char *)strapp)[strlen(strapp)-1] <= 32)
                  strapp[strlen(strapp)-1]= 0; //remove spaces and CR after


            if (!stricmp(strapp, org)) //case insensitive compare
            {
               fclose(cfilebuf);
               return linecount; //has been found
            }
         } //else the row is not a wl definition

      } 
      
      fclose(cfilebuf);
  
  }   //cant open file --either cases it's false

  return 0;
}




bool appendWordToWL(char *lookupfile, char *originalStr, char *translatedStr)
{ 
   unsigned long linenum = searchWordInWL(lookupfile, originalStr), linecount = 0;
   if (linenum)
   {                               
      FILE *cfilebuf = fopen(lookupfile, "r");
      if (cfilebuf)
      {
         fseek(cfilebuf, 0, SEEK_END);
         long filesize = ftell(cfilebuf);
         if (filesize)
         {  
            char store1[1002] = "";
            char *filedata = new char [filesize + strlen(translatedStr) + 20]; 
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
                        store1[strchr(store1, '#')-store1] = 0;
                     } 
                     //we must remove new line chars
                     while (((unsigned char *)store1)[strlen(store1)-1] <= 32)
                             store1[strlen(store1)-1]= 0; //remove CR after
                             
                     strcat(store1, " | ");
                     strcat(store1, translatedStr);
                     strcat(store1, scomments);
                     if (scomments[0] == '\0')
                        strcat(store1, "\n");    //we need to add the removed \n
                  }
                  strcat(filedata, store1);
               }

               cfilebuf = freopen(lookupfile, "w", cfilebuf);
               if (cfilebuf)
               {
     
                   fputs(filedata, cfilebuf);

                   fclose(cfilebuf);
                   
                   delete filedata;
                   return true;
                          
               } 
               delete filedata;
                            
            } else fclose(cfilebuf);
            
         } else fclose(cfilebuf); 
      } 
        
   }
   return false;
}          




int addWordToWL(char *lookupfile, char *originalStr, char *translatedStr)
{   
   if (searchWordInWL(lookupfile, originalStr))
      return -1;
   
   FILE *cfilebuf = fopen(lookupfile, "a+");
   if (cfilebuf)
   {
      if (fgetc(cfilebuf)==EOF) //new file
      {
         fprintf(cfilebuf, WORDS_WFHEADERROW1);
         fprintf(cfilebuf, WORDS_WFHEADERROW2);
         fprintf(cfilebuf, WORDS_WFHEADERROW3);
         fprintf(cfilebuf, WORDS_WFHEADERROW4);
      }
      fseek(cfilebuf, 0, SEEK_END);
      fprintf(cfilebuf, "\n%s = %s", originalStr, translatedStr);
      fclose(cfilebuf);
      
      if (searchWordInWL(lookupfile, originalStr))
         return 0;
   }
   return -2;
}  

  


void doWordsLookup(HWND hwnd, char *lookupfile)
{
  FILE *cfilebuf = fopen(lookupfile, "r");
  if (cfilebuf)
  {  
    char wwfilepath[MAX_PATH]="";
    sprintf(wwfilepath, "%s\\wrongwords.txt", szAppDir);
    FILE *outfilebuf = fopen(wwfilepath, "w");
    if (outfilebuf)
    { 
      long definitions = 0;
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
            
            char org[1000];     
            strncpy(org, s2, strchr(s2, '=')-s2);  org[strchr(s2, '=')-s2] = 0;  
            while (((unsigned char *)org)[strlen(org)-1] <= 32)
                  org[strlen(org)-1]= 0; //remove spaces after
 
            
            s2 = strchr(s2, '=')+1;  
            strlwr(org);
            strlwr(s2); 
            
            unsigned short transCount = 0;
            char *transPtrs[100];
            do
            {
              while (((unsigned char *)s2)[0] <= 32)     //remove spaces before
                      s2++;
              
              transPtrs[transCount] = s2;
              
              s2 = strchr(s2, '|');
              if (s2)
              {
                 s2[0] = 0;
                 s2++;
                 while (((unsigned char *)transPtrs[transCount])[strlen(transPtrs[transCount])-1] <= 32)
                       transPtrs[transCount][strlen(transPtrs[transCount])-1]= 0; //remove spaces after
              }
              transCount++;
            } while (s2 && (transCount<100));
            
                 
            bool foundmismatches = false; 
            for (unsigned short i=0; i<msgObj->getPhrasesCount() ; i++)
            {    
                char *sourceStr = 0;
                if (msgObj->getPhrase(msgObj->findListIndex(i)))
                {
                   sourceStr = new char[(strlen(msgObj->getPhrase(msgObj->findListIndex(i))) +1)];
                   strcpy(sourceStr, msgObj->getPhrase(msgObj->findListIndex(i)));
                   strlwr(sourceStr);
                }
                
                char *destStr = 0;
                if (msgObj->getTranslation(msgObj->findListIndex(i)))
                {
                   destStr = new char[(strlen(msgObj->getTranslation(msgObj->findListIndex(i))) +1)];
                   strcpy(destStr, msgObj->getTranslation(msgObj->findListIndex(i)));
                   strlwr(destStr);
                }
                if (sourceStr)
                {
                   unsigned short present = 0, presentdest = 0;
                   char *tempStr = sourceStr;
                   while (sourceStr[0] != 0)
                   {   
                       char *foundsource = strstr(sourceStr, org);    //TODO use findText from utils.h
                       if (foundsource == NULL)
                          break;
                       
                       present++;   
                       sourceStr = foundsource + strlen(org);
                   }  
                   delete tempStr;
                   if (destStr)
                   {
                      for (unsigned short j = 0; j<transCount; j++)
                      {
                          tempStr = destStr;
                          while (tempStr[0] != 0)
                          {   
                              char *founddest = strstr(tempStr, transPtrs[j]);
                              if (founddest == NULL)
                                 break;
                       
                              presentdest++;
                              tempStr = founddest + strlen(transPtrs[j]);
                          }
                      }                    
                      delete destStr;
                   }  
                   if (present>presentdest)
                   {
                      foundmismatches = true;
                      if (present-presentdest-1)
                         fprintf(outfilebuf, WORDS_MISSEDMATCHSFORMAT, present-presentdest, i+1);
                      else
                         fprintf(outfilebuf, WORDS_MISSEDMATCHFORMAT, present-presentdest, i+1); 
                   }  
                }  
            }
            
            if (foundmismatches)
            {
               fprintf(outfilebuf, WORDS_MISSEDWORDFORMAT, org, transPtrs[0]);
               for (unsigned short j = 1; j<transCount; j++) fprintf(outfilebuf, WORDS_MISSEDALTERNATEWORDFORMAT, transPtrs[j]);
               fputc('\n', outfilebuf);
               if (strchr(s, '#')!=NULL)
                  fprintf(outfilebuf, WORDS_NOTESFORMAT, strchr(s, '#')+1);
               
               fputc('\n', outfilebuf);    
            }
            
            definitions++;
         } //else the row is not a wl definition


      } 

      fprintf(outfilebuf, WORDS_DEFSCOUNTF, definitions);
      
      fclose(outfilebuf);
      
      ShellExecute(hwnd, "edit", wwfilepath, NULL, NULL, SW_SHOW);
      
    } else MessageBox(hwnd, WORDS_CANTCREATEREPORT, ERR_TITLE,
                           MB_OK | MB_ICONEXCLAMATION);
    
    fclose(cfilebuf);
     
  } else MessageBox(hwnd, WORDS_CANTACCESSWORDSFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
     
}
