/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	maps.hpp  -  ProgramAudMap class for storing map of audio entries in RESOURCE.AUD
 *                         
 */


#ifndef MAPS_HPP
#define MAPS_HPP  
 
#include "list.h"

#pragma pack(1)


//  FORMAT OF MAP FILES WITH REFERENCES TO RESOURCE.AUD  (65535.MAP excluded: used for RESOURCE.SFX)

//+00 ulong  start	//offset in RES.AUD file where interest data begins in RESOURCE.AUD


/*typedef*/ struct MAPSAUD {
        unsigned char  noun;       
        unsigned char  verb;
        unsigned char  ncase;             
        unsigned char  seq;       //NOTE when set to 1, the 2 highest bits mean that the SYNCMAP is used      
        unsigned       relOffset:24;         //previous file size
                                           //the first is always 0x000000 of course
                                           //NOTE: when sync is used, this points to the sync structure,
                                           //    and then you must add the two SIZES below
}; // MAPSAUD, *PMAPSAUD;
#define MAPSAUDSIZE (sizeof(MAPSAUD))       //_MAPSAUD


/*
FFFFFFFF
FF
FF
FF
FF
FFFFFF

empty tuples at the EoF
*/
                                           

struct PROGRAMAUDENTRY {
     unsigned short syncsize1;           //size of first sync block
     unsigned short syncsize2;           //size of SYNC string                 

     MAPSAUD map;
};

#pragma pack()



class ProgramAudMap {
       unsigned long _initial_offset; //=0;
       unsigned short _id;
       List<PROGRAMAUDENTRY> *_list;
public:
       ProgramAudMap(unsigned short myid, unsigned short size) :_id(myid) { _list = new List<PROGRAMAUDENTRY>(size); }
       ~ProgramAudMap() { delete _list; };
       
       unsigned short getAudFileNumber() const { return _id; }
       unsigned short getEntriesCount() const { return _list->getLast(); };
       
       long insertSyncEntry1(long position, unsigned short newEntry);
       long insertSyncEntry2(long position, unsigned short newEntry);
       unsigned short getSyncEntry1(unsigned short number) const;
       unsigned short getSyncEntry2(unsigned short number) const;

       long insertAudEntry(long position, MAPSAUD *newEntry);
       MAPSAUD *getAudEntry(MAPSAUD *number) const;
       MAPSAUD *getAudEntryByNum(unsigned short number) const; 
       long searchAud(MAPSAUD *number) const;
       bool removeAudEntry(MAPSAUD *remEntry);
       
       long getInitialOffs() const;
       void setInitialOffs(long offs);
       
       long getAudDataPosition(MAPSAUD *number) const;
};



#endif

