/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	maps.cpp  -  ProgramAudMap class for storing map of audio entries in RESOURCE.AUD
 *                         Includes handling of cloning/uncloning operations
 */
 
#include "maps.hpp"
#include "common.h"



//extern BOOL ExtractFromVolume(HWND hwnd, char *mapname, unsigned char type, char *exten, char *filename);
 
#define PATCH10    0x0010
#define PATCH90    0x0090



//TODO when loading a new Map, we must call function CheckLabelsAllInMap
           //   and then query the user if the loaded content is not 100% correct (refer to red agenda)
/*  //now we can replace the existing map (if it exists)
           if (msgObj->_audMap)
              delete msgObj->_audMap;
           
           msgObj->_audMap = newAudMap;
*/

MAPSAUD *ProgramAudMap::getAudEntryByNum(unsigned short number) const
{
 
  PROGRAMAUDENTRY *paud = _list->getItem(number);
  
  if (paud)
     return &(paud->map);
 
  return 0;

} 


MAPSAUD *ProgramAudMap::getAudEntry(MAPSAUD *number) const
{
    return getAudEntryByNum(searchAud(number));

}

bool ProgramAudMap::removeAudEntry(MAPSAUD *remEntry)
{
    //searching for a possible match
    long whereIsIt = searchAud(remEntry);
    if (whereIsIt>=0)
       return _list->removeItem(whereIsIt);
                  
    return false;
}


long ProgramAudMap::getAudDataPosition(MAPSAUD *number) const
{
   long entry = searchAud(number);  
   if (entry==-1)
      return -1;
   
   PROGRAMAUDENTRY *paud;
   long offset = getInitialOffs();
   for (unsigned short i = 0; i<=entry; i++)
   {
     paud = _list->getItem(i);        
     offset += paud->map.relOffset;
   }
   
   offset += getSyncEntry1(entry) + getSyncEntry2(entry);
   
   return offset;
}


long ProgramAudMap::insertAudEntry(long position, MAPSAUD *newEntry)
{
    //searching for a possible match
    long tmpTwinIdx = searchAud(newEntry);
    if (tmpTwinIdx>=0)
    {
       return -2;
    }           
    
    PROGRAMAUDENTRY *paud = new PROGRAMAUDENTRY;
    
    paud->map = *newEntry;       //memcpy((paud->map), newEntry, MAPSAUDSIZE);
   
    if (position > 0)
    {  
       if (!_list->insertItem(position, paud))
         return -3; 
    }
    else   
      if (!_list->addItem(paud))
         return -1;
  
    return 0;
}

unsigned short ProgramAudMap::getSyncEntry1(unsigned short number) const
{
    PROGRAMAUDENTRY *paud = _list->getItem(number);
  
    if (paud)
    {
       if (paud->map.seq & 0x80)  //the highest 2 bits: 11000000
          return paud->syncsize1;
    }

    return 0;
}

unsigned short ProgramAudMap::getSyncEntry2(unsigned short number) const
{
    PROGRAMAUDENTRY *paud = _list->getItem(number);
  
    if (paud)
    {
       if (paud->map.seq & 0x40)  //the highest 2 bits: 11000000
          return paud->syncsize2;
    }

    return 0;
}
    
long ProgramAudMap::insertSyncEntry1(long position, unsigned short newEntry)
{
    //CHECK 
    PROGRAMAUDENTRY *paud = _list->getItem(position);
    if (paud)
    {  
       if (!(paud->map.seq & 0x80))  //hassync //the one of the highest 2 bits: 11000000
          return -3;
       
       paud->syncsize1 = newEntry;  
    }
    else   
         return -1;
  
    return 0;
}

long ProgramAudMap::insertSyncEntry2(long position, unsigned short newEntry)
{
    //CHECK 
    PROGRAMAUDENTRY *paud = _list->getItem(position);
    if (paud)
    {  
       if (!(paud->map.seq & 0x40))  //hassync //the one of the highest 2 bits: 11000000
          return -3;
       
       paud->syncsize2 = newEntry;  
    }
    else   
         return -1;
  
    return 0;
}

long ProgramAudMap::searchAud(MAPSAUD *number) const
{   
    MAPSAUD *tempAudPtr; 
    for (unsigned short i=0; i<_list->getLast(); i++)
    {
         tempAudPtr = getAudEntryByNum(i);
         if (!tempAudPtr)   //but that should never happen
            return -2;
         if ((tempAudPtr->noun == number->noun) && (tempAudPtr->verb == number->verb) &&
            (tempAudPtr->ncase == number->ncase) && ((tempAudPtr->seq & 0x3f) == (number->seq & 0x3f)))  //CHECK    //the lowwst 6 bits: 00111111
            return (long)i;
    }
     
    return -1;
}


 long ProgramAudMap::getInitialOffs() const { return _initial_offset; }
 
 void ProgramAudMap::setInitialOffs(long offs) { _initial_offset = offs; } 

//////////////////////////////////////////////////////////////////////////////////////////////



ProgramAudMap *ImportAudMAP(HWND hwnd, char **filename)
{
    
   char pszFileName[MAX_PATH] = "";
   
   if (*filename != 0)
      if ((*filename)[0] == 0)
         *filename =0;
         
   if (*filename == 0)
   {      
         OPENFILENAME ofn;
         char szMapFileName[MAX_PATH] = "";
   
         ZeroMemory(&ofn, sizeof(ofn));
         //szFileName[0] = 0;
    
         ofn.lStructSize = sizeof(ofn);
         ofn.hwndOwner = hwnd;
         ofn.lpstrFilter = INTERFACE_OPENMAPFILTER; 
         ofn.lpstrFile = szMapFileName;
         ofn.nMaxFile = MAX_PATH;
         //ofn.lpstrDefExt = "map";

         ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOVALIDATE;
   
         if (GetOpenFileName(&ofn)!=0)
         { 
     
           if (ofn.nFilterIndex==2)
           {
            //extract from resource
            if (!ExtractFromVolume(hwnd, szMapFileName, 0x90, (char*)"map", pszFileName))
               return 0;
         
           } else {     
             memcpy(pszFileName, szMapFileName, MAX_PATH);
           }
         }
   
   } else
     memcpy(pszFileName, *filename, MAX_PATH);  
         
     
   if (pszFileName[0] != 0)
   {    
     FILE *cfilebuf = fopen(pszFileName, "rb");
     if (cfilebuf)
     {
        unsigned char offset = 0;
        unsigned short patchID;
        
        fread((void *) &patchID, 2, 1, cfilebuf);
        if (patchID == PATCH90 || patchID == PATCH10)
        {
           //verify for FFFFFFFFFFFFFFF at the end of file:
           fseek(cfilebuf, -11, SEEK_END);
           for (int i=0; i<11; i++)
               if (fgetc(cfilebuf)!=0xFF)
               {
                  fclose(cfilebuf);
                  MessageBox(hwnd, MAPS_WRONGENDOFMAPFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
                  
                  return 0;
               }      
           
           fseek(cfilebuf, 0, SEEK_END);
                    
               
           ProgramAudMap *newAudMap = new ProgramAudMap(0,0);
           MAPSAUD pam;
        
           fseek(cfilebuf, 2, SEEK_SET);   //jump the header
           
           unsigned long initoffs = 0;
           fread((void *) &(initoffs), 4, 1, cfilebuf);
           newAudMap->setInitialOffs(initoffs);
          
           unsigned short items = 0;     
           do 
           {
               fread((void *) &(pam), MAPSAUDSIZE, 1, cfilebuf);
               newAudMap->insertAudEntry(-1, &pam); 
               
               unsigned short tshort = 0;               
               if (pam.seq & 0x80)  // 10000000
               {
                  fread((void *) &tshort, 2, 1, cfilebuf);
                  newAudMap->insertSyncEntry1(items, tshort);
               } 
               if (pam.seq & 0x40)  //NOTE 01000000  
               {
                  fread((void *) &tshort, 2, 1, cfilebuf);
                  newAudMap->insertSyncEntry2(items, tshort);
               }      
               items++;      
           } while ((pam.noun != 0xff) || (pam.verb != 0xff) || (pam.ncase != 0xff) || (pam.seq != 0xff));
               //NOTE we might check for sync structure to be FFFF FFFF too, but who cares

           fclose(cfilebuf);
           
           *filename = pszFileName;
           
           return newAudMap;
           
        } else { 
            MessageBox(hwnd, MAPS_STRANGESCIPATCHINMAPFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
        }
         
        fclose(cfilebuf);
        
                                                                                  
     } else {
         if(*filename == 0)    //else... the errors are handled by the caller function
                      MessageBox(hwnd, MAPS_CANTACCESSMAPFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
     }
   
   }
   return 0;

}

bool ExportAudMAP(HWND hwnd, ProgramAudMap *padmap, char *filename)
{  
   if (filename != 0)
      if (filename[0] == 0)
         filename =0;
         
   if (filename == 0)  
   {  
     
     OPENFILENAME ofn;
     char szSaveFileName[MAX_PATH] = "";

     ZeroMemory(&ofn, sizeof(ofn));
     //szSaveFileName[0] = 0;
     
     ofn.lStructSize = sizeof(ofn);
     ofn.hwndOwner = hwnd;
     ofn.lpstrFilter = INTERFACE_SAVEMAPFILTER;
     ofn.nFilterIndex = 1;
     ofn.lpstrFile = szSaveFileName;
     ofn.nMaxFile = MAX_PATH;
     ofn.lpstrDefExt = "map"; 

     ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
               OFN_OVERWRITEPROMPT;
     
     if(!GetSaveFileName(&ofn))
       return false;
       
     filename = szSaveFileName;
   }
     
         
   if (filename != 0) {
     FILE *cfilebuf = fopen(filename, "wb");
     if (cfilebuf)
     {
        unsigned short tempshort = PATCH90;
        fwrite((void *) &tempshort, 2, 1, cfilebuf);
        
        long templong = padmap->getInitialOffs();
        fwrite((void *) &templong, 4, 1, cfilebuf);
        
        for (long i = 0; i<padmap->getEntriesCount(); i++)
        {
            MAPSAUD *map = padmap->getAudEntryByNum(i);
            fwrite((void *) map, MAPSAUDSIZE, 1, cfilebuf);
            
            unsigned short tshort=0;
            if (map->seq & 0x80)              
            {
               tshort = padmap->getSyncEntry1(i);
               fwrite((void *) &tshort, 2, 1, cfilebuf);
            } 
            if (map->seq & 0x40)  //SEQ is 11000000, both sizes are used
            {              //CHECK if 0x40 case exists alone!!
               tshort = padmap->getSyncEntry2(i);
               fwrite((void *) &tshort, 2, 1, cfilebuf); 
            }                
        }    

    /*    MAPSAUD pam;                    //NOTE: not needed because we store the FFFFF in programaudiomap 
        pam.noun = 0xff;
        pam.verb = 0xff;
        pam.ncase = 0xff;
        pam.seq = 0xff;
        pam.relOffset = 0xffffff;
        fwrite((void *) &pam, 1, MAPSAUDSIZE, cfilebuf);
                                
        templong = 0xffffffff;                     //syncs empty tuple
        fwrite((void *) &templong, 4, 1, cfilebuf);
     */
        fclose(cfilebuf);
        
        return true;

     } else {
         MessageBox(hwnd, MAPS_CANTACCESSMAPFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
         
     }

   }
   
   return false;
}


bool AddExCloneToAudMAP(HWND hwnd, SortedMsg *cloneID, SortedMsg *origID, ProgramAudMap *map)
{      
      
    if (map != 0)   
    {
       
           //first, lets see if the clone is already there:
           MAPSAUD pam, pam2;
           pam.noun = cloneID->noun;
           pam.verb = cloneID->verb;
           pam.ncase = cloneID->ncase;
           pam.seq = cloneID->seq;
           pam.relOffset  = 0;   //so we will write a ref to the same file as the original
           
             
           if (map->searchAud(&pam) < 0)
           {
                //not found, then we can add it (we start over searching the original):
                pam2.noun = origID->noun;
                pam2.verb = origID->verb;
                pam2.ncase = origID->ncase;
                pam2.seq = origID->seq;
                long position  = map->searchAud(&pam2);
                
                if (position >=0)
                {
                   pam.seq |= (map->getAudEntryByNum(position)->seq & 0xC0);  
                   long result = map->insertAudEntry(position+1, &pam);
                   switch (result)   //save return value and handle errors
                   {
                      case -1:    
                          MessageBox(hwnd, MAPS_ITEMALREADYEXISTS, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
                          break;
                      case -2:
                      case -3:        
                          MessageBox(hwnd, MAPS_LISTERRORDURINGINSERT, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
                          break;
                      default:
                       if (result >= 0)
                       {
                          if (map->getAudEntryByNum(position)->seq & 0xC0)
                          {   
                             map->insertSyncEntry1(position+1, map->getSyncEntry1(position));
                             map->insertSyncEntry2(position+1, map->getSyncEntry2(position));
                          }

                          return true;
                       }   
                   }
                
                } else
                       MessageBox(hwnd, MAPS_CANTFINDORIGINALTOCOPYFROM, ERR_TITLE,
                           MB_OK | MB_ICONEXCLAMATION);
     
           
           } else
              MessageBox(hwnd, MAPS_ITEMALREADYEXISTS, ERR_TITLE,
                             MB_OK | MB_ICONEXCLAMATION);

           

    } else
               MessageBox(hwnd, MAPS_NOMAPINMEMORY, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
                           
                           
   return false;
}


void VerifyAudMap(HWND hwnd, MsgData *cmpObj, char *filename)
{
 //DESCRIPTION: this function is used to compare the content of a selected 
 //             MAP file with the nouns, verbs, etc in the MSG to find if
 //             they are all included in the MAP as well.

    
  ProgramAudMap *pam = ImportAudMAP(hwnd, &filename);
  
  if (pam)
  {

      unsigned short cmpstrscount = cmpObj->getPhrasesCount();

      char difffilepath[MAX_PATH];
      sprintf(difffilepath, "%s\\mapcheck.txt", szAppDir);
      FILE *diffile = fopen(difffilepath, "w");
      if (diffile)
      { 
         fprintf(diffile, EXPORT_MAPTITLE);
         
         for (unsigned short j=0, i; j< cmpstrscount; j++)
         {
            i = cmpObj->findListIndex(j);
            MAPSAUD map;
            map.noun = cmpObj->getNoun(i);
            map.verb = cmpObj->getVerb(i);
            map.seq = cmpObj->getSeq(i);
            map.ncase = cmpObj->getCase(i);  
            
            bool isclone = cmpObj->getCloneNoun(i) +
                         cmpObj->getCloneVerb(i) +
                         cmpObj->getCloneCase(i);  
            if (pam->searchAud(&map) <0)
              fprintf(diffile, MAPS_MAPCHECKFORMATSTRING, j+1, map.noun, map.verb, map.ncase, map.seq, (isclone ?MAPS_BUTITISACLONE:""));
                              
         }
         fprintf(diffile, EXPORT_ENDOFFILE);
         
         fclose(diffile);
         
         ShellExecute(hwnd, "edit", difffilepath, NULL, NULL, SW_SHOW);
         
         //TODO delete this temp file after it is read
         //remove(difffilepath);    //TO FIX: it's deleted before the external program can access it!
      }
      else
      {
         MessageBox(hwnd, ERR_CANTLOADFILE, ERR_TITLE,
            MB_OK | MB_ICONEXCLAMATION);
            
      }
         
  }

}

bool ValidateExternalMAP(HWND hwnd, ProgramAudMap *intmap, char *filename)
{
     ProgramAudMap *extmap = ImportAudMAP(hwnd, &filename);
     if (extmap)
     {
        for (long i = 0; i<intmap->getEntriesCount(); i++)
        {    
             MAPSAUD *map = intmap->getAudEntryByNum(i);
             if (extmap->searchAud(map) <0)
                return false;

        }
        return true;
     }
     return false;
}


void RemoveZeroCloneFromAudMAP(HWND hwnd, SortedMsg *cloneID, SortedMsg *origID, ProgramAudMap *map)
{     //for now, we only remove MAP entries to stuff uncloned by Sciamano (which is offset=0 however, so it is perfectly reversible)
      //NOTE IMPORTANT if we will remove non-clones, remember to shift offsets in the MAP file!        
      
     if (!map) return;
     
     MAPSAUD pam;
     pam.noun = cloneID->noun;
     pam.verb = cloneID->verb;
     pam.ncase = cloneID->ncase;
     pam.seq = cloneID->seq;
     pam.relOffset  = 0;
       
     MAPSAUD *mapd = map->getAudEntry(&pam);
     if (!mapd){
        //NOTE message? not necessarily
         return;
     }
     
     if (mapd->relOffset == 0) //TODO put a question with warning on real entries removal:
                               // allow user to proceed anyway (but must also fix offsets then!)
     {
        map->removeAudEntry(&pam);
        MessageBox(hwnd, MAPS_ENTRYREMOVED, MAPS_TITLE, MB_OK | MB_ICONINFORMATION);
     } else
         MessageBox(hwnd, MAPS_WASNTREMOVEDMIGHTBEUSEFUL, WARN_ATTENTION, MB_OK | MB_ICONEXCLAMATION);
          
}
