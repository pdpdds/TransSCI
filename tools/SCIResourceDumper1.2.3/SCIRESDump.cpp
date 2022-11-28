/*****************************************************************************************

SCI resource dumper, part of VAG's SCI Tools - Dumper for Sierra's SCI resources.
Copyright (C) 2001-2002  VAG

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Any your applications which uses any part of this source(s) SHOULD BE also
distributed with FREELY AVAILABLE source code. Also, you should include my
name in the source and binary file(s) of your application.
Please, do not remove this banner, it must be included in any source based
on this one. Thanks.

Fell free to contact with author at vagsoft@mail.ru

*****************************************************************************************/
/*
               SIERRA's SCI Hacking project by VAG at 2001-2002
     This file may contain some information about SCI internals. All such
  information/data formats is discovered by myself and may me wrong/incomplete.
  If you found this file useful or have additional information - please contact me,
  i'll glad to hear your comments.

  File created: VAG, 17 jul 2001 - Remake of my old ResDump
  Was modified: VAG, 21 jul 2002 - SCI1 support added
  Was modified: VAG, 23 jul 2002 - Formats handling rewritten
  Was modified: Drigo, 6 mar 2003 - Fixed some minor bugs
  Was modified: Drigo, 6 jun 2003 - Added optional file header
*/

#include "stdafx.h"
#include "resvol.h"
#include <stdlib.h>
#include <ctype.h> //Drigo - for tolower()
#include <memory.h>
#include "lzs.h"
#include "pkdcl.h"

#define SCI1	1
#define SCI32	32

char *res_type[] = {
// 0     1       2      3      4      5      6       7     8       9        10       11        12      13     14     15      
"view","pic","script","text","sound","etc","vocab","font","cursor","patch","bitmap","palette","audio","audio","sync","message",   //drigo replaced '<3>' with 'text'
//16     17     18    19         20      21             22     23    24      25     26    27   28    29  30   31   
"map","heap","chunk","audio36","sync36","translation","robot","vmd","duck","clut","targa","zzz",NULL,NULL,NULL,NULL,  //drigo replaced '<17>' with 'heap'
// 0x20
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
//0x80
"view","pic","script","text","sound","memory","vocab","font","cursor","patch","bitmap","palette","wave","audio","sync","message",
"map","heap","audio36","sync36","xlate"};

char *res_ext[] = {
// 0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15   
"v56","p56","scr","tex","snd","etc","voc","fon","cur","pat","bmp","pal","au2","aud","syn","msg", //drigo replaced '#3' with 'tex'; replaced 'mes' with 'msg'
//16    17    18    19    20    21    22    23    24    25    26    27
"map","hep","chk","a36","s36","trn","rbt","vmd","duk","clu","tga","zzz",NULL,NULL,NULL,NULL, //drigo replaced '#17' with 'hep'
// 0x20
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
//0x80
"v56","p56","scr","tex","snd","mem","voc","fon","cur","pat","bmp","pal","wav","aud","syn","msg",
"map","hep","a36","s36","trn"};

RESMAPDIRSCI		tdir[256];
union {
RESMAPIDSSCI1		rdir1;
RESMAPIDSSCI32		rdir32;
} rdir;

union {
RESVOLENTRYSCI1		rhd1;
RESVOLENTRYSCI32	rhd32;
} rhd;

FILE *F;

//STACSTATE state;

char outname[15];
FILE *VH;
FILE *dfh;
long slen_,dlen_;
void *sp,*dp;
void *sp_,*dp_;

long dumpnum=0;

//Defaults
char *map=NULL;
char *vol=NULL;
int  mapformat=0;
int  volformat=0;
int  pakformat=0;
int  head=false;  //added by Drigo

void cmdlineerror(char *what)
{
	printf("CMDLINE: Strange %sspecified, ignored...\n",what);
	return;
}

char* cmdlinegetformat(char *cmdline,int &result)
{
	switch (*cmdline++)
	{
	case '1':
		{
			result=SCI1;
			break;
		}
	case '3':
		{
			result=SCI32;
			break;
		}
	default:
		result=0;
	}
	return cmdline;
}

int parsecmdline(int argc, char *argv[])
{
    for(int i=1;i<argc;i++)
	{
		char *c=argv[i];
		if(*c=='/')
		{
			while(*c++=='/')
			{
				switch (tolower(*c++))
				{
				case 'p':
					{
						c=cmdlinegetformat(c,pakformat);
						if(pakformat==0)
							cmdlineerror("decompressor ");
						break;
					}
				case 'm':
					{
						c=cmdlinegetformat(c,mapformat);
						if(mapformat==0)
							cmdlineerror("map format ");
						break;
					}
				case 'v':
					{
						c=cmdlinegetformat(c,volformat);
						if(volformat==0)
							cmdlineerror("volume format ");
						break;
					}
                case 'h': //added by Drigo
                    {
                        head = true;
						break;
                    }
                case 'n':
					{
						vol=c;
						c="";	//terminate scanning
					}
				default:
					cmdlineerror("switch ");
				}
			}
		}
		else
		{
			if(map==NULL)
				map=c;
			else
				dumpnum=atol(c);
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
int i;
long j;
unsigned short	vol_no;
unsigned long	vol_offs;
unsigned short	dlen;

	printf("SCI1/32 resource dumper v1.2.3 (c) VAG, Enrico Rolfi 2-jan-2k5, ARR.\n"); //modified by Drigo
	if(argc<2)
	{
		printf("Usage: dumper [/p1|3][/m1|3][/v1|3][/h][/nVolumeFile] <resource.map> [dump_index]\n");   //modified by Drigo
		printf("Where:\n  /p - select decompress mode\n  /m - select map format\n  /v - select volume format\n  /h - enable sci type header\n  /n - specify volume name\n"); //modified by Drigo
		return -1;
	}

	parsecmdline(argc,argv);

	if((F=fopen(map,"rb"))==NULL)
	{
		printf("Can't open map file!\n");
		return -2;
	}

	sp=dp=NULL;

	for(i=0;i<256;i++)
	{
		fread(&tdir[i],RESMAPDIRSCISIZE,1,F);
		if(mapformat==0)
		{
			if(tdir[i].type&0x80)
				mapformat=SCI1;
			else
				mapformat=SCI32;
			printf("Detected SCI%s map format...\n",(mapformat==SCI1)?"1":"32");
		}
		if(tdir[i].type==0xFF) break;
	}

	// Setup SCI type-specific vars

	if(volformat==0)
		if(mapformat==SCI1)
			volformat=SCI1;
		else
			volformat=SCI32;

	int dirsize=(mapformat==SCI1) ? RESMAPIDSSCI1SIZE : RESMAPIDSSCI32SIZE;  //Drigo use mapformat to determine struct size instead of volformat

	if(pakformat==0)
		if(volformat==SCI1)
			pakformat=SCI1;
		else
			pakformat=SCI32;

	if(vol==NULL)
		if(volformat==SCI1)    //Drigo use volformat instead of mapformat to determine filename
			vol="resource.000";
		else
			vol="ressci.000";
   

	printf("Using SCI%s map format, SCI%s volume format, SCI%s unpacking mode and%sheader\n",  /*modified by Drigo */
		(mapformat==SCI1)?"1":"32",
		(volformat==SCI1)?"1":"32",
		(pakformat==SCI1)?"1":"32",
        (head)?" ":" no ");   //added by Drigo
	printf("Volume name: %s\n",vol);
    
    VH=fopen(vol,"rb"); //DRIGO
	i=0;
	j=1;
	while(tdir[i].type!=0xFF)
	{
		dlen=tdir[i+1].list-tdir[i].list;
		printf("Directory type %d (%s), offs= %x, len= %x (%d entries) :\n",tdir[i].type,res_type[tdir[i].type],tdir[i].list,dlen,dlen/dirsize);
		fseek(F,tdir[i].list,SEEK_SET);
		while(dlen>=dirsize)
		{
			fread(&rdir,dirsize,1,F);
			vol_no=0;				//not used yet :-/
			if(mapformat==SCI1)
			{
				vol_offs=rdir.rdir1.offset*2;
			}
			else
			{
				vol_offs=rdir.rdir32.offset;
			}
			printf("  #%5ld: id=%5u: vol= %2d, offs= %8lx",j,(mapformat==SCI1)?rdir.rdir1.id:rdir.rdir32.id,vol_no,vol_offs);

			if((dumpnum==-1)||(j==dumpnum))
			{
				printf(" Saving...");
				if(VH!=NULL) //DRIGO
				{
					fseek(VH,vol_offs,SEEK_SET);
					fread(&rhd,1,(volformat==SCI1)?RESVOLENTRYSCI1SIZE:RESVOLENTRYSCI32SIZE,VH);
					unsigned long paklen=(volformat==SCI1)?rhd.rhd1.paklen:rhd.rhd32.paklen;
					unsigned long unplen=(volformat==SCI1)?rhd.rhd1.unplen:rhd.rhd32.unplen;
// fun begins...
					if(((sp=(void*)malloc(paklen))!=NULL)&&((dp=(void*)malloc(unplen))!=NULL))
					{
						fread(sp,1,paklen,VH);

						if(paklen<unplen)
						{
							sp_=sp;
							dp_=dp;
							slen_=paklen;
							dlen_=unplen;
							if(pakformat==SCI1)
							{
								dcl_unpack(sp_,dp_);
							}
							else
							{
								//packed and unpacked lengths not used yet...
								lzs_unpack(sp_,dp_);
							}
						}
						else
						{
							memcpy(dp,sp,unplen);
						}
						sprintf(outname,"%d.%s",(mapformat==SCI1)?rdir.rdir1.id:rdir.rdir32.id,res_ext[tdir[i].type]);
						if((dfh=fopen(outname,"wb+"))!=NULL)
						{
                            if (head)
							{  //added by Drigo
                               switch (tdir[i].type & 0x7F)
							   {
							   case 0: //view
								{
									unsigned long patchID =0;
									patchID += 0x008080;
									fwrite(&patchID,4,1,dfh);

									unsigned short tshort=320;
									fwrite(&tshort,2,1,dfh);
									tshort=200;
									fwrite(&tshort,2,1,dfh);
									tshort=5;
									fwrite(&tshort,2,1,dfh);
									tshort=6;
									fwrite(&tshort,2,1,dfh);
									tshort=256;
									fwrite(&tshort,2,1,dfh);
									tshort=0;
									for (int zkj=0;zkj<6;zkj++)
										fwrite(&tshort,2,1,dfh);
								}
									break;
							   case 1: //pic
                               {
									unsigned long patchID=0;
									patchID = 0x00008181;
									fwrite(&patchID,4,1,dfh);
                               }
									break;
							   default:
									fputc(tdir[i].type|0x80, dfh); //added by Drigo
									fputc(0x00, dfh);  //added by Drigo
							   }
                            }
							fwrite(dp,1,unplen,dfh);
							fclose(dfh);
							printf(" -> Saved ok! (%lx->%lx)",paklen,unplen);
						}
						else
						{
							printf(" Output error!");
						}
						//fclose(VH); DRIGO
					}
					else
					{
						printf("\n *** Mem error (s=%p,d=%p, sl=%lx dl=%lx)",sp,dp,
								paklen,unplen);
					}
					if(sp!=NULL)
					{
						free(sp);
						sp=NULL;
					}
					if(dp!=NULL)
					{
						free(dp);
						dp=NULL;
					}
				}
				else
				{
					printf(" Can't open volume!");
				}
			}
			printf("\n");
			dlen-=dirsize;
//			if(dumpnum==j)	//Don't browse anymore if resource extracted
//				break;
			j++;
		}
		if(dlen)
		{
			printf("Hole! len= %d bytes\n",dlen);
		}
		printf("\n");
		i++;
        
	}
	fclose(F);
    if (VH!=NULL)
       fclose(VH);    //DRIGO

	return 0;
}
