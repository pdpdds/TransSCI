/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *  SCRParse.cpp : VAG's SCRParse mini-mod for SCIaMano
 *                 used to guess Noun names from Heap files.
 *
 * 	NOTE: Noun names are just guessed by the program!
 *        for full accuracy, one must first dissassemble script 0 and do a lot of fancy stuff!
 *        while the method here proved to be good enough for our needs
 */
 

#include "common.h"

//extern BOOL ExtractFromVolume(HWND hwnd, char *mapname, unsigned char type, char *exten, char *filename);

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;

HWND hwndlink = 0;

#define PATCHHEP      0x0011                                                                       
#define PATCH80HEP    0x0091  

class Obj
{
	
 public:
    ulong offs;

	ulong len;
    char *oname;
	ulong rpos;


	ushort dict;
	bool IsClass;
	void Load(void *buf,ulong &pos);
	void DoMagic();
    char *GetName();
    bool SetName2Noun(char prop);
 };
 
 

void Obj::Load(void *buf,ulong &pos)
{
	offs=(long)buf+pos;
    short *o=(short*)offs; 
         
	rpos=pos;
	dict=*(ushort*)&((uchar*)offs)[2];
	len=dict*2;
	pos+=len;
    IsClass=(o[7]&0x8000)!=0; //NOTE: attribute 7 usually identifies a class
    oname = (char*)buf+(o[8]); //NOTE: attribute 8 USUALLY is the name (but not always! so we are guessing here)
	return;
}

char *Obj::GetName()
{
    return oname;
}

bool Obj::SetName2Noun(char prop)
{
    short *o=(short*)offs;
    
    if ((o[prop]&0xFFFF) <256)
    {
       if (o[prop] != 0)
       {
          char *previous = 0;
          previous = msgObj->getNounName((unsigned char) (o[prop]&0xFFFF));
          if (previous)
          {
             char question[300];
             sprintf(question, LABELS_REDEFINENAMEFROMSCRIPT, GRAPH_NOUN, (unsigned char) (o[prop]&0xFFFF), previous, GetName());
             if (MessageBox (hwndlink, question, WARN_ATTENTION, 
                            MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)
                  == IDNO)
                     return true;
          
          }   
          msgObj->setNounName((unsigned char) (o[prop]&0xFFFF), GetName());
          UpdateListLabels((unsigned char) (o[prop]&0xFFFF), GetName(), 1);
       
       }
       return true;     //FIX it is better outside or inside the previous block?
    }
    return false;
}

void Obj::DoMagic()
{
	short *o=(short*)offs;
    //TODO if isclass is not useful remove it (maybe classes never have noun?? CHECK!
	/*printf("\n\n\n%s %s\n{\n",(IsClass?"CLASS":"OBJECT"), oname);


              printf("      0: -objID-       = %x\n",o[0]&0xFFFF);
              printf("      1: -size-        = %x\n",o[1]&0xFFFF);
              printf("      2: -propDict-    = %x\n",o[2]&0xFFFF);
              printf("      3: -methDict-    = %x\n",o[3]&0xFFFF);
              printf("      4: -classScript- = %x\n",o[4]&0xFFFF);
              printf("      5: -script-      = %x\n",o[5]&0xFFFF);
              printf("      6: -super-       = %x\n",o[6]&0xFFFF);
              printf("      7: -info-        = %x\n",o[7]&0xFFFF);
                */
	   //       for(int i=8;i<o[1];i++)
		//          printf("    %3d: PROP_%-3d = %x\n",i,i,o[i]&0xFFFF);
      //  
      
      if (o[9]==0)            //TODO add low version exclusion and move qfg1room in the lower if-s
         if (o[10] == 0)
         {
           if (o[11] == -1) //room noun GK/QFG1
                if (SetName2Noun(12))
                   return;
           
           if (o[11]==0 && o[12]==-1) //ROOM noun QFG4
                if (SetName2Noun(13))
                   return;
                   
           if (o[12]==0 && o[13]==-1) //QFG4 
              if (SetName2Noun(11))
                   return;
                   
         } else if (o[11]==0 && o[12]==-1) //GK
                if (SetName2Noun(10))
                   return;
                
      if (o[14]==-1 && ((msgObj->getVersion() & 0xFFFFFF00) < SCI32_100))
           if (SetName2Noun(13))
                   return;          
      
    return;
}





void *hep;		//storage for whole scr, heap and voc997
ulong hepS;		//Length of scr/heap/voc997
ulong heppos;	//current position on scr/heap/voc997
ulong hepFixOfs; //offset of fixups


char buf[100];

long SeekToData(FILE *F)
{
long sz;
char hdrlen;
	fseek(F,0,SEEK_END);
	sz=ftell(F);
	
        //NOTE added check for SCI header in the beginning of file
        fseek(F,0,SEEK_SET);
 unsigned short patchID=0;
    fread(&patchID,2,1,F);
    if (patchID != PATCH80HEP && patchID != PATCHHEP) 
       return 0;  
        ///////
        
    fseek(F,1,SEEK_SET);    
	fread(&hdrlen,1,1,F);
	fseek(F,hdrlen,SEEK_CUR);
	sz-=ftell(F);
	return sz;
}

void* LoadFile(char *fn,ulong &len)
{
FILE *F;
void *b;
	F=fopen(fn,"rb");
	if(F==NULL)  
		return NULL;


	len=SeekToData(F);
	//NOTE added check for SCI header in the beginning of file
    if (len>0)
                    b=malloc(len);      ///original
    else
        b = NULL;
    /////        
	if(b==NULL)
	{
		fclose(F);
		return NULL;
	}
	fread(b,1,len,F);
	fclose(F);
	return b;
}



Obj o[100];
int numobjs=0;


int LoadNounsFromHEAP(HWND hwnd)         
{
   hwndlink = hwnd;
   OPENFILENAME ofn;
   char szMapFileName[MAX_PATH] = "", pszFileName[MAX_PATH] = "";
   
   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFilter = INTERFACE_LOADHEAPFILEFILTER; 
   ofn.lpstrFile = szMapFileName;
   ofn.nMaxFile = MAX_PATH;

   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOVALIDATE;
   if (GetOpenFileName(&ofn)!=0)
   { 
     
     if (ofn.nFilterIndex==2)
     {
        //extract from resource
         if (!ExtractFromVolume(hwnd, szMapFileName, 0x91, (char*)"hep", pszFileName))
            return false;
         
     } else {     
     
        memcpy(pszFileName, szMapFileName, MAX_PATH);
     }
            


	hep=LoadFile(pszFileName,hepS);
	if(hep==NULL)
	{
		MessageBox(hwnd, ERR_CANTLOADHEAPFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);

		return false;
	}


	heppos=0;
    
    
//load Heap
        
	hepFixOfs=*(ushort*)&((uchar*)hep)[heppos];
	heppos+=2;

	if(*(ushort*)&((uchar*)hep)[heppos])
	{              //jump vars
	   heppos+=(*((ushort *)((ulong)hep+heppos)))*2 +2;   
	}
	else
	{
		heppos+=2;
	}

	while(*(ushort*)&((uchar*)hep)[heppos])
	{  
		o[numobjs].Load(hep,heppos);
	
          
            //do magic stuff here
            o[numobjs].DoMagic();    
             
        numobjs++;
	}


	free(hep);
    return true;
   }
   return false;
}
