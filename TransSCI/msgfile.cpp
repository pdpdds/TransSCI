/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	msgfile.c  -  functions for MSG file loading-in/saving-from a MsgData object
 * 
 */
 
#include "common.h"

//to save window size, position
 extern RECT windowRect, tempwindowRect;


bool LoadFile(HWND hwnd, LPSTR pszFileName, MsgData **msgObj)
{
    FILE *cfilebuf = fopen(pszFileName, "rb");
    if (cfilebuf)
    {
       //we destroy the old file structure from memory:
        if (*msgObj)
           delete *msgObj;
        *msgObj = 0;

       //let's see if the patch header is in the file :
        unsigned char offset = 0;
        unsigned short patchID;
        unsigned long versionID;
        unsigned short order =0;
        
        fread((void *) &patchID, 2, 1, cfilebuf);
        if (patchID == PATCH80 || patchID == PATCH)
           offset = 2;

        fseek(cfilebuf, offset, SEEK_SET);        

        fread((void *) &versionID, 4, 1, cfilebuf);

        MsgStruct fheader;
        if ((versionID & 0xFFFFFF00) >= SCI_05) {
           fseek(cfilebuf, offset, SEEK_SET);        
           fread((void *) &fheader, 10, 1, cfilebuf);        
        } else {   //older games
           fheader._msgVersionTag = versionID;
           if ((versionID & 0xFFFFFF00) > SCI_01)
              fread((void *) &(fheader._commentsPos), 2, 1, cfilebuf);
           fread((void *) &(fheader._phrasesCount), 2, 1, cfilebuf);
        }
  
        switch (fheader._msgVersionTag & 0xFFFFFF00) {
        case SCI_01:     
        case SCI_02_B:
        case SCI_02_C:
        case SCI_02_D:
        case SCI_05:
             if (patchID == PATCH)  //patchID is 0x0F only since SCI32v2.1
                break;
        case SCI_06:
        case SCI32_100: 
            *msgObj = new MsgData(fheader);   
            //List<StringItem> *strList = (*msgObj)->getStringList();
            //List<CommentItem> *comList = (*msgObj)->getCommentList();
            
            unsigned short i, j;
            unsigned short tCount = (*msgObj)->getPhrasesCount();          
            unsigned char unkbyte, unkBCount1 = 5, unkBCount2 = 9;

            switch (versionID & 0xFFFFFF00) {
            case SCI_01:
                 unkBCount1 = 2;
                 unkBCount2 = 2;
                 break;     
            case SCI_02_B:
            case SCI_02_C:
            case SCI_02_D: 
                 unkBCount2 = 8;
                 break;  
            }            

            unsigned short jumpvalue;
            unsigned long oldpos;
            unsigned long tradusci2DataPos = 0;

            //we must test if this is one of TraduSCI's file formats
            oldpos = ftell(cfilebuf);

            fseek(cfilebuf, (unkBCount2 + 2) * (*msgObj)->getPhrasesCount(), SEEK_CUR);
            char tempSign[265]; //to be sure the string read from file fits :)
            my_fgets(tempSign, 256, cfilebuf);
            if (!strcmp(tempSign, TRADUSCI1_SIGN)) {
               (*msgObj)->tradusciFormat(OLDTRADUSCI);
               (*msgObj)->hasComments(fgetc(cfilebuf));
               unsigned short ushort;
               fread((void *) &ushort, 2, 1, cfilebuf);
               (*msgObj)->currentString(ushort);
               (*msgObj)->fontSize(18); //reset to default font 
            } else if (!strcmp(tempSign, TRADUSCI2_SIGN)) {
               (*msgObj)->tradusciFormat(TRADUSCI2);
               (*msgObj)->hasComments(fgetc(cfilebuf));
               unsigned short ushort;
               fread((void *) &ushort, 2, 1, cfilebuf);
               (*msgObj)->currentString(ushort); 
               fread((void *) &tradusci2DataPos, 4, 1, cfilebuf);
               //version 1.1.2 introduces saving of Font size
               //we check for word 0FF0, if that thing exists, 
               //then we read for the Font size as unsigned char:
               fread((void *) &ushort, 2, 1, cfilebuf);
               if (ushort==0xF00F) {
                  unsigned char fsize;
                  fread((void *) &fsize, 1, 1, cfilebuf);
                  (*msgObj)->fontSize(fsize);
               } else
                 (*msgObj)->fontSize(18); //reset to default font
               
               //version 1.1.4 introduces saving of Sorting order
               //we check for word 0EE0, if that thing exists, 
               //then we read for the Sorting Order as unsigned char: 
               fread((void *) &ushort, 2, 1, cfilebuf);
               if (ushort==0xE00E) {
                  fread((void *) &order, 1, 1, cfilebuf);
               } else
                 order=ByIndex; //reset to default
              
               //added v 1.2 - stuff for window size              
               fread((void *) &ushort, 2, 1, cfilebuf);
               if (ushort==0xBAAB) {
                  char tch;
                  fread((void *) &tch, 1, 1, cfilebuf);    //discard findbaractive
                  fread((void *) &tch, 1, 1, cfilebuf);    //discard listbaractive

                  fread((void *) &tch, 1, 1, cfilebuf);    //discard windowmaximized

                  fread((void *) &(tempwindowRect.left), 2, 1, cfilebuf);
                  fread((void *) &(tempwindowRect.top), 2, 1, cfilebuf);
                  fread((void *) &(tempwindowRect.right), 2, 1, cfilebuf);
                  fread((void *) &(tempwindowRect.bottom), 2, 1, cfilebuf);
               }
               else
               { //default sizes
                   tempwindowRect.right = 0;
               }
               ////////////////////////////////////
            } else if (!strncmp(tempSign, TRADUSCI_SIGN, 8)){
               MessageBox(hwnd, ERR_NEWERFILEVERSION, ERR_TITLE,
                                MB_OK | MB_ICONEXCLAMATION);
               return FALSE;
            } else { //Sierra's format
               (*msgObj)->tradusciFormat(SIERRA);
               (*msgObj)->currentString(0);
               (*msgObj)->fontSize(18); //reset to default font
            }

            fseek(cfilebuf, oldpos, SEEK_SET);
            for (i=0; i<tCount; i++) {
                //StringItem *strItem = msgObj->getPhrase(i);
                for (j=0; j<unkBCount1; j++) {
                    unkbyte = (unsigned char) fgetc(cfilebuf);
                    (*msgObj)->setUnkByte(i, unkbyte, j);
                }

                fread((void *) &jumpvalue, 2, 1, cfilebuf);
                       
                oldpos = ftell(cfilebuf);

                fseek(cfilebuf, jumpvalue + offset, SEEK_SET);
 
                if ((*msgObj)->tradusciFormat()==SIERRA) { //Sierra's original file format
                   char tempStr[10240]; //to be sure the string read from file fits :)
                   my_fgets(tempStr, 10240, cfilebuf);

                   if (!((*msgObj)->setPhrase(i, tempStr))) {
                      MessageBox(hwnd, ERR_STRITEM1, ERR_TITLE,
                                       MB_OK | MB_ICONEXCLAMATION);
                      return FALSE;
                   }
                } else { //TraduSCI/2 file formats
                   char tempStr[10240]; //to be sure the string read from file fits :)
                   my_fgets(tempStr, 10240, cfilebuf);

                   if ((*msgObj)->tradusciFormat()==TRADUSCI2) {
                      jumpvalue = (unsigned short) ftell(cfilebuf); //for SCI_01 games with no comment pos
                      fseek(cfilebuf, tradusci2DataPos, SEEK_SET);
                   }
                   
                   bool translated = fgetc(cfilebuf);
                         
                   if (translated) {
                      if ((*msgObj)->tradusciFormat()==TRADUSCI2)
                         fseek(cfilebuf, -1, SEEK_CUR);    
                      char tempStr2[10240]; //to be sure the string read from file fits :)
                      my_fgets(tempStr2, 10240, cfilebuf);

                      if (!((*msgObj)->setPhrase(i,tempStr2))) {
                         MessageBox(hwnd, ERR_STRITEM2, ERR_TITLE,
                                          MB_OK | MB_ICONEXCLAMATION);
                         return FALSE;
                      }
                      if (!((*msgObj)->setTranslation(i,tempStr))) {
                         MessageBox(hwnd, ERR_STRITEM3, ERR_TITLE,
                                          MB_OK | MB_ICONEXCLAMATION);
                         return FALSE;
                      }
                   } else {
                      if (!((*msgObj)->setPhrase(i,tempStr))) {
                         MessageBox(hwnd, ERR_STRITEM4, ERR_TITLE,
                                          MB_OK | MB_ICONEXCLAMATION);
                         return FALSE;
                      }
                   }
                   if ((*msgObj)->tradusciFormat()==TRADUSCI2) {
                      tradusci2DataPos = ftell(cfilebuf);
                   }   
                }

                if ((*msgObj)->tradusciFormat()==OLDTRADUSCI)
                   jumpvalue = (unsigned short) ftell(cfilebuf); //for SCI_01 games with no comment pos
                
                
                fseek(cfilebuf, oldpos, SEEK_SET);
                for (j=unkBCount1; j<unkBCount2; j++) {
                    unkbyte = (unsigned char) fgetc(cfilebuf);
                    (*msgObj)->setUnkByte(i,unkbyte, j);
                }
            } 
      if ((versionID & 0xFFFFFF00) != SCI_01)
            {
               (*msgObj)->setNounName(0, INTERFACE_GENERICLABEL);
               (*msgObj)->setVerbName(0, INTERFACE_GENERICFLABEL);
               (*msgObj)->setCaseName(0, INTERFACE_GENERICLABEL);
            }
            
      if ((*msgObj)->tradusciFormat()==TRADUSCI2) {             
            ////////since version 1.1.5  - ARRAYS of label names
            
            
            (*msgObj)->setTalkerName(0, INTERFACE_GENERICLABEL);   //only talker for SCI_01
            if ((*msgObj)->tradusciFormat()==TRADUSCI2)
            {
               fseek(cfilebuf, tradusci2DataPos, SEEK_SET);
               
               unsigned short tempTag = 0;
               fread((void *)&tempTag, 2, 1, cfilebuf);
               if (tempTag == 0xD00D)
               {
                  unkbyte = (unsigned char) fgetc(cfilebuf);
                  while (unkbyte != 0)
                  {
                     char tempStr[101];
                     my_fgets(tempStr, 100, cfilebuf);
                     (*msgObj)->setNounName(unkbyte, tempStr);
                  
                     unkbyte = (unsigned char) fgetc(cfilebuf);
                  }

                  unkbyte = (unsigned char) fgetc(cfilebuf);
                  while (unkbyte != 0)
                  {
                     char tempStr[101];
                     my_fgets(tempStr, 100, cfilebuf);
                     (*msgObj)->setVerbName(unkbyte, tempStr);
                  
                     unkbyte = (unsigned char) fgetc(cfilebuf);
                  }
            
                  unkbyte = (unsigned char) fgetc(cfilebuf);
                  while (unkbyte != 0)
                  {
                     char tempStr[101];
                     my_fgets(tempStr, 100, cfilebuf);
                     (*msgObj)->setCaseName(unkbyte, tempStr);
                  
                     unkbyte = (unsigned char) fgetc(cfilebuf);
                  }
                  
                  unkbyte = (unsigned char) fgetc(cfilebuf);
                  while (unkbyte != 0)
                  {
                     char tempStr[101];
                     my_fgets(tempStr, 100, cfilebuf);
                     (*msgObj)->setTalkerName(unkbyte, tempStr);
                  
                     unkbyte = (unsigned char) fgetc(cfilebuf);
                  }
                  
                  tempTag = 0;
                  fread((void *)&tempTag, 2, 1, cfilebuf);
                  if (tempTag != 0xD0D0)
                     MessageBox(hwnd, WARN_ENDTAGNAMESNOTFOUND, WARN_ATTENTION, MB_OK | MB_ICONEXCLAMATION);
               } else
                   fseek(cfilebuf, tradusci2DataPos, SEEK_SET);
            }
            
            //////////////////
            
            // since version 1.1.6 (september 08)  - added audio MAP data
            long rewind = ftell(cfilebuf);
                 
            char tempStr[256];
            tempStr[0] = 0;
            my_fgets(tempStr, 9, cfilebuf);
            if (!strcmp(tempStr, "AMAP"))
            {
               my_fgets(tempStr, 255, cfilebuf);
               (*msgObj)->setAudMapName(tempStr);
            
               unsigned short ushort;
               fread((void *) &ushort, 2, 1, cfilebuf);
               
               ProgramAudMap *newAudMap = new ProgramAudMap(0,0);
               MAPSAUD pam;
                   
               unsigned long initoffs = 0;
               fread((void *) &(initoffs), 1, 4, cfilebuf);
               newAudMap->setInitialOffs(initoffs);
             
               for (unsigned short i = 0; i<ushort; i++)  
               {
                   fread((void *) &(pam), MAPSAUDSIZE, 1, cfilebuf);
                   newAudMap->insertAudEntry(-1, &pam); 
                   
                   unsigned short tshort = 0;               
                   if (pam.seq & 0x80)  // 10000000
                   {
                      fread((void *) &tshort, 2, 1, cfilebuf);
                      newAudMap->insertSyncEntry1(i, tshort);
                   } 
                   if (pam.seq & 0x40)  //NOTE 01000000  
                   {
                      fread((void *) &tshort, 2, 1, cfilebuf);
                      newAudMap->insertSyncEntry2(i, tshort);
                   }     
               }
               
               (*msgObj)->setAudMap(newAudMap);
               
               //let's jump FFFFFFFF
               fseek(cfilebuf, 4, SEEK_CUR);

               //check if it's the end     
               my_fgets(tempStr, 9, cfilebuf);
               if (!strcmp(tempStr, "PAMA"))
                  MessageBox(hwnd, WARN_ENDTAGMAPSNOTFOUND, WARN_ATTENTION, MB_OK | MB_ICONEXCLAMATION);
            } else
                  fseek(cfilebuf, rewind, SEEK_SET);
             
            ///////////////////////////////////////
            // THE BOX FOR FUTURE GENERATIONS//////
            ///////////////////////////////////////
            //***********************************//
            //we must save all the data beyond this point,
            //for it might have been wrote by FUTURE program versions!! :D
            long curPos = ftell(cfilebuf);
            fseek(cfilebuf, 0, SEEK_END);
            curPos = ftell(cfilebuf) - curPos;
            unsigned char *newFB = 0;
            if (curPos)
            { // data to save! LOL!
              fseek(cfilebuf, -curPos, SEEK_END);
              newFB = new unsigned char [curPos];
              fread(newFB, 1, curPos, cfilebuf);  
            } 
            (*msgObj)->setFutureBox(newFB);
            (*msgObj)->setFutureBoxSize(curPos);
            
            ///////////////////////////////////////
      }
            
            if ((versionID & 0xFFFFFF00) > SCI_01) {
               fseek(cfilebuf, 4 + offset, SEEK_SET); //to read comments location
               
               fread((void *) &jumpvalue, 2, 1, cfilebuf);
               jumpvalue += 6 + offset;
            }
            
            if ((*msgObj)->tradusciFormat()==SIERRA) //if it's a Sierra's format file and we got to find out if it has comments
            {
                  fseek(cfilebuf, 0, SEEK_END);
                  (*msgObj)->hasComments(jumpvalue < ftell(cfilebuf));
            }
            
            
            if ((*msgObj)->hasComments()) {                 
                fseek(cfilebuf, jumpvalue, SEEK_SET);
                switch (versionID & 0xFFFFFF00) {
                case SCI_01:
                     unkBCount1 = 2;
                     unkBCount2 = 0;
                     break;     
                case SCI_02_B:
                case SCI_02_C:
                case SCI_02_D:
                     unkBCount1 = 0;
                     unkBCount2 = 0;
                     break;
                default:
                     unkBCount1 = 0;
                     unkBCount2 = 6;
                }            

                for (i=0; i<tCount; i++) {
                    //CommentItem *comItem = comList->getPhrase(i);
                    
                    for (j=0; j<unkBCount1; j++) {
                        unkbyte = (unsigned char) fgetc(cfilebuf);
                        (*msgObj)->setComUnkByte(i, unkbyte, j);
                    }    

                    char tempStr[10240]; //to be sure the string read from file fits :)
                    my_fgets(tempStr, 10240, cfilebuf);
                
                    if (!((*msgObj)->setComment(i, tempStr))) {
                       MessageBox(hwnd, ERR_COMITEM, ERR_TITLE,
                                        MB_OK | MB_ICONEXCLAMATION);
                       return FALSE;
                    }                    
                  
                    for (j=0; j<unkBCount2; j++) {
                        unkbyte = (unsigned char) fgetc(cfilebuf);
                        (*msgObj)->setComUnkByte(i,unkbyte, j);
                    }
                }                               
            }
            
            fclose(cfilebuf);            
           
            (*msgObj)->initSort();
            (*msgObj)->sortBy(order);

            return TRUE;         
            break;
        }

        //NOTE:  this should be handled in another way due to changes in version stuff
        if ((fheader._msgVersionTag & 0xFFFFFF00) >= SCI_01 &&
            (fheader._msgVersionTag & 0xFFFFFF00) <= SCI32_100) {
            
            char errorStr[256];
            sprintf(errorStr, ERR_UNKNOWNGAME, fheader._msgVersionTag);
            MessageBox(hwnd, errorStr, ERR_MESSAGEFROMAUTHOR,
                             MB_OK | MB_ICONEXCLAMATION); 
        }                             
    }

    return FALSE;

}



bool SaveFile(HWND hwnd, LPSTR pszFileName, bool tradusci)
{
    FILE *cfilebuf = fopen(pszFileName, "wb");
    if (cfilebuf)
    {
       unsigned short i, j, ushort, strCount = 0;
       unsigned long ulong2 = 0, ulvers = 0, tradusci2DataP;
       
       ulvers = msgObj->getVersion();
       ushort = (((ulvers & 0xFFFFFF00) == SCI32_100) ? PATCH : PATCH80);
       fwrite(&ushort, 2, 1, cfilebuf);
       
       fwrite(&ulvers, 4, 1, cfilebuf);
       if ((ulvers & 0xFFFFFF00) > SCI_01) {
          ushort = 0;
          fwrite(&ushort, 2, 1, cfilebuf); //make place for comments position pointer
       }
       if ((ulvers & 0xFFFFFF00) >= SCI_05) {
          ushort = msgObj->getUnkCount();
          fwrite(&ushort, 2, 1, cfilebuf);
       }
       
       strCount = msgObj->getPhrasesCount();
       fwrite(&strCount, 2, 1, cfilebuf);

       //List<StringItem> *strList = msgObj->getStringList(); 
       unsigned char unkBCount1 = 5, unkBCount2 = 9, headersize = 10;

       switch (ulvers & 0xFFFFFF00) {
       case SCI_01:
            unkBCount1 = 2;
            unkBCount2 = 2;
            headersize = 6;
            break;     
       case SCI_02_B:
       case SCI_02_C:
       case SCI_02_D: 
            unkBCount2 = 8;
            headersize = 8;
       } 
     
       for (i = 0; i < strCount; i++) {
           //StringItem *strItem = strList->getPhrase(i);
           for (j = 0; j < unkBCount1; j++)
               fputc(msgObj->getUnkByte(msgObj->findListIndex(i), j), cfilebuf);
           
           ushort = 0;
           fwrite(&ushort, 2, 1, cfilebuf); //make place for string position pointer

           for (j = unkBCount1; j < unkBCount2; j++)
               fputc(msgObj->getUnkByte(msgObj->findListIndex(i),j), cfilebuf);    
       }

       if (tradusci) { //write an identifier and options for tradusci modified format
          my_fputs(TRADUSCI2_SIGN, cfilebuf);
          fputc(msgObj->hasComments(), cfilebuf);
          ushort = msgObj->getFileIndex(msgObj->currentString());
          fwrite(&ushort, 2, 1, cfilebuf);
          tradusci2DataP = ftell(cfilebuf); //position of the following pointer
          ulong2 = 0;
          fwrite(&ulong2, 4, 1, cfilebuf); //make place for TraduSCI2 data pointer
          //added since version 1.1.2, it is the font size choosen by the user
          //there is also a magic number (0FF0) used to check if this information is present
          //(if the file has been saved using version 1.1, the font size info won't be present
          //and i'm doing this cause I'm tired of changing formats... :)
          ushort = 0xF00F;
          fwrite(&ushort, 2, 1, cfilebuf);
          fputc(msgObj->fontSize(), cfilebuf);
          //added since version 1.1.4, it is the string order choosen by the user
          //there is also a magic number (0EE0) used to check if this information is present
          //(if the file has been saved using version 1.1.4, that info won't be present
          //and i'm doing this cause I'm tired of changing formats... :)
          ushort = 0xE00E;
          fwrite(&ushort, 2, 1, cfilebuf);
          fputc(msgObj->sorting(), cfilebuf);
          
          //v1.2 - we add this tag for window sizes info
          ushort = 0xBAAB;
          fwrite(&ushort, 2, 1, cfilebuf);
          fputc(1, cfilebuf);    //set old version 1.2 bars active
          fputc(1, cfilebuf);    //old bars
          fputc(0, cfilebuf);    //window not maximized (not used)
          ushort = windowRect.left;
          fwrite(&ushort, 2, 1, cfilebuf);
          ushort = windowRect.top;
          fwrite(&ushort, 2, 1, cfilebuf);
          ushort = windowRect.right;
          fwrite(&ushort, 2, 1, cfilebuf);
          ushort = windowRect.bottom;
          fwrite(&ushort, 2, 1, cfilebuf);
          //////////////////////////////////////////////
          
          ushort = 0xE00E;
          //NOTE to continue, we add ten 0x0EE0 words for future formats support...
          for (i=0; i<10; i++)
              fwrite(&ushort, 2, 1, cfilebuf);
       }

       for (i = 0; i < strCount; i++) {
           //StringItem *strItem = strList->getPhrase(i);
           
           fseek(cfilebuf, 0, SEEK_END);
           ushort = (unsigned short) ftell(cfilebuf) -2;  //-2 for the patch header
           
           if (msgObj->getTranslation(msgObj->findListIndex(i))) {
              my_fputs(msgObj->getTranslation(msgObj->findListIndex(i)), cfilebuf);  
           } else {   
              my_fputs(msgObj->getPhrase(msgObj->findListIndex(i)), cfilebuf); //use original string if not specified a translation yet
           }           

           fseek(cfilebuf, 2+headersize+(i*(unkBCount2+2))+unkBCount1, SEEK_SET);
           fwrite(&ushort, 2, 1, cfilebuf);                
       }

       if ((ulvers & 0xFFFFFF00) > SCI_01) {
          fseek(cfilebuf, 0, SEEK_END);
          ushort = (unsigned short) ftell(cfilebuf) -8;  //another -2 is for the patch header
          fseek(cfilebuf, 6, SEEK_SET);
          fwrite(&ushort, 2, 1, cfilebuf);  //comment position pointer
       }

       if (msgObj->hasComments()) {
          fseek(cfilebuf, 0, SEEK_END);
          //List<CommentItem> *comList = msgObj->getCommentList();

          switch (ulvers & 0xFFFFFF00) {
          case SCI_01:
               unkBCount1 = 2;
               unkBCount2 = 0;
               break;     
          case SCI_02_B:
          case SCI_02_C:
          case SCI_02_D: 
               unkBCount1 = 0;
               unkBCount2 = 0;
               break;
          default:
               unkBCount1 = 0;
               unkBCount2 = 6;
          }

          for (i = 0; i < strCount; i++) {
              //CommentItem *comItem = comList->getPhrase(i);
           
              for (j = 0; j < unkBCount1; j++)
                  fputc(msgObj->getComUnkByte(msgObj->findListIndex(i),j), cfilebuf);

              my_fputs(msgObj->getComment(msgObj->findListIndex(i)), cfilebuf);
           
              for (j = 0; j < unkBCount2; j++)
                  fputc(msgObj->getComUnkByte(msgObj->findListIndex(i),j), cfilebuf);
          }
       }

       if (tradusci) {
          fseek(cfilebuf, 0, SEEK_END); //to be sure
          ulong2 = ftell(cfilebuf);
          fseek(cfilebuf, tradusci2DataP, SEEK_SET);
          fwrite(&ulong2, 4, 1, cfilebuf); //to set data position in Tradusci's far header (near program signature)
          
          fseek(cfilebuf, 0, SEEK_END); //to start writing
          for (i = 0; i < strCount; i++) {
              //StringItem *strItem = strList->getPhrase(i);
           
              if (msgObj->getTranslation(msgObj->findListIndex(i))) {
                 my_fputs(msgObj->getPhrase(msgObj->findListIndex(i)), cfilebuf);  
              } else {   
                fputc(0, cfilebuf); //means that nothing follows (current string hasn't been translated yet!)
              }    
          } 
          
          //since version 1.1.5  - add ARRAYS of label names
   
          ushort = 0xD00D;                     //names tag
          fwrite(&ushort, 2, 1, cfilebuf);
          for (int i = 1; i<256; i++)     //NOTE we jump 0 (n/used)
              if (msgObj->getNounName(i))
              {
                 fputc(i ,cfilebuf);
                 my_fputs(msgObj->getNounName(i), cfilebuf);
              }
          fputc(0, cfilebuf);
          for (int i = 1; i<256; i++)     //NOTE we jump 0 (n/used)
              if (msgObj->getVerbName(i))
              {
                 fputc(i ,cfilebuf);
                 my_fputs(msgObj->getVerbName(i), cfilebuf);
              }
          fputc(0, cfilebuf);
          for (int i = 1; i<256; i++)     //NOTE we jump 0 (n/used)
              if (msgObj->getCaseName(i))
              {
                 fputc(i ,cfilebuf);
                 my_fputs(msgObj->getCaseName(i), cfilebuf);
              }
          fputc(0, cfilebuf);    
          for (int i = 1; i<256; i++)     //NOTE we jump 0 (n/used)
              if (msgObj->getTalkerName(i))
              {
                 fputc(i ,cfilebuf);
                 my_fputs(msgObj->getTalkerName(i), cfilebuf);
              }
          fputc(0, cfilebuf);
          ushort = 0xD0D0;                    //names end tag
          fwrite(&ushort, 2, 1, cfilebuf);
          ////////////////
          
          
          //since version 1.1.6 (september 2008)  - add audio MAP data
          ProgramAudMap *pam = msgObj->getAudMap();
          if (pam)
          {
              my_fputs("AMAP", cfilebuf);         //map tag
              my_fputs(msgObj->getAudMapName(), cfilebuf);
              ushort = pam->getEntriesCount();             
              fwrite(&ushort, 2, 1, cfilebuf);
              unsigned long offs = pam->getInitialOffs();
              fwrite(&offs, 4, 1, cfilebuf);
              MAPSAUD *maud;
              for (long i = 0; i<ushort; i++)        
              {
                  maud = pam->getAudEntryByNum(i);
                  fwrite((void *) maud, MAPSAUDSIZE, 1, cfilebuf);
                  
                  unsigned short tshort=0;
                  if (maud->seq & 0x80)              
                  {
                     tshort = pam->getSyncEntry1(i);
                     fwrite((void *) &tshort, 2, 1, cfilebuf);
                  } 
                  if (maud->seq & 0x40)  //if SEQ is 11000000, both sizes are used
                  {              //CHECK if 0x40 case exists alone!!
                     tshort = pam->getSyncEntry2(i);
                     fwrite((void *) &tshort, 2, 1, cfilebuf); 
                  }                
              }
     
               my_fputs("PAMA", cfilebuf);         //map end tag

          }  
          //////////////////////////////////////////////////////////////
            
            ///////////////////////////////////////
            // THE BOX FOR FUTURE GENERATIONS//////
            ///////////////////////////////////////
            //***********************************//
            //we must save all the data beyond this point,
            //for it might be read by FUTURE program versions!! :D
            
            if (msgObj->getFutureBoxSize())
               fwrite((void *) msgObj->getFutureBox(), 1, msgObj->getFutureBoxSize(), cfilebuf);   
            
            /////////////////////////////////////// 
       }

        fclose(cfilebuf);
               
       return TRUE;        
    }
    return FALSE;
}

