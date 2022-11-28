/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	This code header is the class definition for the MSG file type utilized
 *	in SCI1/SCI32.
 *	
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstring>
#include "list.h"
#include "maps.hpp"
                                                                                            
                                                                                                
/* 0xF identifies a Message file. Older games have this value ORed with 0x80  */                
#define PATCH      0x000F                                                                       
#define PATCH80    0x008F                                                                       
                                                                                                
/* TraduSCI searchs for one these Signatures in Message files, which should be located          
 * before the very first phrase and is accesed directly after the last map entry */             
#define TRADUSCI_SIGN     "TraduSCI"  //This SHOULD BE common to all future versions!!!         
#define TRADUSCI1_SIGN    "TraduSCI 1.0 di Enrico Rolfi (Endroz)"                               
#define TRADUSCI2_SIGN    "TraduSCI2 1.1 di Enrico Rolfi (Endroz)"                              
                                                                                                
/* These are constants to identify the file format and version and to test them */              
#define SIERRA            0                                                                     
#define OLDTRADUSCI       1                                                                     
#define TRADUSCI2         2                                                                     
                                                                                                
                                                                                                
/* These SCI tags are the first long in every Message file and identifies                       
 * the file version, which causes sligth changes in format                   */                 
enum SCI_Tags {                                                                                 
     SCI_01 = 0x00000800,      /* eco1     806 834 835                       */                 
                                                                                                
     SCI_02_B = 0x00000B00,    /* laura2   bb8 bc5                                              
                                  pq1rem   bc5                               */                 
     SCI_02_C = 0x00000C00,    /* laura2   c29 c8a c94 ca9 cbc ce4                              
                                  qfg1     c27 cbc ce4                                          
                                  pq1rem   c2d c94 cdc                       */                 
     SCI_02_D = 0x00000D00,    /* laura2   d0c                                                  
                                  qfg1     d0c d48                                              
                                  pq1rem   d0c d48 d52 d53                   */                 
     SCI_05 = 0x00000F00,      /* qfg3     fa0 faa                                              
                                  isle1    fa0 faa                                              
                                  isle2    ???                                                  
                                  kq6      fa0 faa                                              
                                  sq4win   ???                                                  
                                  eq2      fa0 faa                                              
                                  sq5      fa0                                                  
                                  pepper   ???                                                  
                                  freddy   fa0                               */                 
     SCI_06 = 0x00001000,      /* eq2      1068 1072 1073 10e0                                  
                                  sq5      10e0                                                 
                                  pepper   ???                                                  
                                  freddy   1004 1073 10e1 10cc                                  
                                  qfg3it   1004                                                 
                                  lsl6     1072 10e1 10f5                                       
                                  qfg4     1072 10e1                                            
                                  gk1      10e1                                                 
                                  torin    ???                                                                                                 
                                  kq7      ???                                                  
                                  phanta   ???                                                  
                                  qfg4cd   1072 10e1 10f5                                       
                                  lsl7     10e1                              */                 
     SCI32_100 = 0x00001300    /* qfg4cd   1388
                                  gk2      1388                                                 
                                  shiver   ???                                                  
                                  phant2   ???                                                  
                                  lsl7     1388 1392                                            
                                  RAMA     ???                                                  
                                  shivr2   ???                                                  
                                  lighth   ???                                                  
                                  sq6      ???                               */                 
};                                                                                              
                                                                                                



/* Each StringItem has its own set of data bytes plus one phrase position
   pointer in the file. Number and displacement of data bytes depends on
   file version as follows:
 
   Version SCI1.1.001 files have 2 data bytes followed by a phrase pointer;
   Version SCI1.1.002 files have five data bytes followed by a phrase pointer
                      and yet other three data bytes (these do not appear to be clone stuff);
   AFAIK, all files after SCI1.1.002 have five data bytes followed by a
                      phrase pointer and yet other four data bytes (these are clone stuff)
           */

class StringItem {
    friend class List<StringItem>;
protected:
	unsigned char _unkByte[9]; // these are the clones refs, and other unknown stuff
	char *_phrase; //in later msg files this is located after fifth unkbyte and before sixth 
    char *_frase;  //the translated one :) this is not present in the original Sierra file
public:
    StringItem() : _phrase(0), _frase(0) { for (char i=0; i<9; i++) _unkByte[i]=255; } //TODO OLDGAME: disable in other way   and set back 0 here
    ~StringItem();

    const char *getString() const { return _phrase; }
    bool setString(char *what);

    const char *getTranString() const { return _frase; }
    bool setTranString(char *what);

    const unsigned char getUnkByte(unsigned char which) const;
    void setUnkByte(unsigned char ub, unsigned char which);
};


/* Each CommentItem has its own set of data bytes plus one comment string.
   Number and displacement of data bytes depends on file version as follows:
  
   Version SCI1.1.001 files have 2 data bytes followed by a comment string;
   Version SCI1.1.002 files have no data bytes but only a comment string;
   AFAIK, all files after SCI1.1.002 starts with a comment string
                      followed by six data bytes                             */

class CommentItem { 
    friend class List<CommentItem>;
protected:
 	char *_comment; //this is located before the six unkbytes in later msg files
	unsigned char _unkByte[6]; // all of these bytes are unknown, however they should be of no use
	                           // byte 6 is always 0 or 1
	
public:
    CommentItem() : _comment(0) { for (char i=0; i<6; i++) _unkByte[i]=255; } //TODO OLDGAME: disable in other way   and set 0 here
    ~CommentItem();
 
    const char *getString() const { return _comment; }
    bool setString(char *what);

    const unsigned char getUnkByte(unsigned char which) const;
    void setUnkByte(unsigned char ub, unsigned char which);
};

/* This Message Header like structure is temporary and used when first loading
   a file into memory to check its content before allocating objects.
   However this header has been modified in time as follows:
   
   Version SCI1.1.001 has no _commentsPos and no _unkCount;
   Versions before SCI1.1.005 have no _unkCount;
   Newer versions has it all.                                                 */

typedef struct MsgStruct { //this is used to load from file
    unsigned long _msgVersionTag;  //identifies SCI version.... i.e. 1000 = SCI32 see maindefs.h  
	unsigned short _commentsPos;   //file position of programmer's script comments
	unsigned short _unkCount;      //I have no idea of what it is, but it's useless //TODO check that it is not crucial when adding new strings (unclone seq)
	unsigned short _phrasesCount;  //this is also comments count! comments are related to phrases
};
          


struct SortedMsg {
 unsigned short index;
 unsigned char noun;
 unsigned char verb;
 unsigned char ncase; 
 unsigned char seq;       
 unsigned char talker;
};

enum sorts { ByIndex = 0, ByNoun, ByVerb, ByCase, BySeq, ByTalker }; 

/* MsgData objects represent an entire Message file loaded in memory, however
   certain variables are introduced by TraduSCI for its own use:
   
   _comments is a pointer to a CommentItem list
   _phrases is a pointer to a StringItem list
   _stringIndex is the actual item being displayed to the user
   _hasComments is used when saving, to handle Msg file with no comments
   _tradusciFormat set to 1 or 2 or higher (future file formats) if any 
    traduSCI signature was detected in a file; set to 0 if Sierra format   */
 
class MsgData {

 private:
	unsigned long _msgVersionTag;  
	List<CommentItem> *_comments;  //list of programmer's script comments
	unsigned short _unkCount;
	unsigned short _phrasesCount;
	List<StringItem> *_phrases;    //this is not part of SCI, but I need it
    unsigned short _stringIndex;   //this is not part of SCI
    bool _hasComments;             //this is not part of SCI, but I need it
    unsigned char _tradusciFormat;   //this is not part of SCI
    unsigned char _fontSize;         //this is not part of SCI; added since version 1.1.2

    List<SortedMsg> *_mlist;
    
    char _sorting;
    
     /////
    char *_nouns[256];
    char *_verbs[256];
    char *_cases[256];
    char *_talkers[256];  
    ///v1.1.5 - structure for storing noun verb etc names defined by user or read from heaps/scripts

    ProgramAudMap *_audMap;
    char _audMapName[256];
    
    unsigned char *_futurebox; //added from 1.2
    long _futureboxsize; //added from 1.2
    
    
 public:
	MsgData(MsgStruct &msgh);
    ~MsgData();

    unsigned long getVersion() const { return _msgVersionTag; }
 //   List<CommentItem> *getCommentList() const { return _comments; }
    unsigned short getUnkCount() const { return _unkCount; }
    unsigned short getPhrasesCount() const { return _phrasesCount; }
 //   List<StringItem> *getStringList() const { return _phrases; }
    unsigned short currentString() { return _stringIndex; }
    void currentString(unsigned short doesit) { _stringIndex = doesit; }
    bool hasComments() { return _hasComments; }
    void hasComments(bool doesit) { _hasComments = doesit; }
    unsigned char tradusciFormat() { return _tradusciFormat; }
    void tradusciFormat(unsigned char version) { _tradusciFormat = version; }
    unsigned char fontSize() { return _fontSize; }
    void fontSize(unsigned char points) { _fontSize = points; }

    const char *getPhrase(unsigned short which) const;
    const char *getTranslation(unsigned short which) const;
    const char *getComment(unsigned short which) const;
   
    long getFileIndex(unsigned short idx) const;
    const SortedMsg *getLabelFromIndex(unsigned short idx) const;
    
    bool setPhrase(unsigned short which, char *what);
    bool setTranslation(unsigned short which, char *what);
    bool setComment(unsigned short which, char *what);
    
    const unsigned char getUnkByte(unsigned short phrase, unsigned char which) const;
    void setUnkByte(unsigned short phrase, unsigned char ub, unsigned char which);
    
    const unsigned char getComUnkByte(unsigned short comment, unsigned char which) const;
    void setComUnkByte(unsigned short comment, unsigned char ub, unsigned char which);
                                                                                       //NOTE this SCI_01 condition is for eco quest (only the talker is recognizeable)
    const unsigned char getNoun(unsigned short phrase) const { return (((getVersion() & 0xFFFFFF00) == SCI_01)?0:getUnkByte(phrase, 0)); };
    void setNoun(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 0); };
    const unsigned char getVerb(unsigned short phrase) const { return (((getVersion() & 0xFFFFFF00) == SCI_01)?0:getUnkByte(phrase, 1)); };
    void setVerb(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 1); };
    const unsigned char getCase(unsigned short phrase) const { return (((getVersion() & 0xFFFFFF00) == SCI_01)?0:getUnkByte(phrase, 2)); };
    void setCase(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 2); };
    const unsigned char getSeq(unsigned short phrase) const { return (((getVersion() & 0xFFFFFF00) == SCI_01)?0:getUnkByte(phrase, 3)); };
    void setSeq(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 3); };
    const unsigned char getTalker(unsigned short phrase) const { return (((getVersion() & 0xFFFFFF00) == SCI_01)?getUnkByte(phrase,0):getUnkByte(phrase, 4)); };
    void setTalker(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 4); };
    
    const unsigned char getCloneNoun(unsigned short phrase) const { return getUnkByte(phrase, 5); };
    void setCloneNoun(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 5); };
    const unsigned char getCloneVerb(unsigned short phrase) const { return getUnkByte(phrase, 6); };
    void setCloneVerb(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 6); };
    const unsigned char getCloneCase(unsigned short phrase) const { return getUnkByte(phrase, 7); };
    void setCloneCase(unsigned short phrase, unsigned char ub) {setUnkByte(phrase, ub, 7); };
    
    bool setNounName(unsigned char which, char *what);
    bool setVerbName(unsigned char which, char *what);
    bool setCaseName(unsigned char which, char *what);
    bool setTalkerName(unsigned char which, char *what);
    
    char *getNounName(unsigned char which) const;
    char *getVerbName(unsigned char which) const;
    char *getCaseName(unsigned char which) const;
    char *getTalkerName(unsigned char which) const;
    
    void initSort();
    void sortBy(char type);
    unsigned short sorting() { return _sorting;}
    
    
    unsigned short findListIndex(unsigned short fileidx) const; 
    unsigned short findListIndex(SortedMsg which) const; 
    
    
    ProgramAudMap *getAudMap() const { return _audMap; }
    void setAudMap(ProgramAudMap *map) { _audMap = map; }
    
    const char *getAudMapName() const { return _audMapName; }
    void setAudMapName(char *map) { memcpy(_audMapName, map, 256); }
    
    SortedMsg DuplicateSeq(SortedMsg orig);
    
    bool isClone(unsigned short phrase) { return ((getCloneNoun(phrase) + getCloneVerb(phrase) + getCloneCase(phrase)) && ((getVersion() & 0xFFFFFF00) >= SCI_05)); }

    unsigned char *getFutureBox() { return _futurebox; }
    void setFutureBox(unsigned char *fb) { _futurebox = fb; }
    long getFutureBoxSize() { return _futureboxsize; }
    void setFutureBoxSize(long fbs) { _futureboxsize = fbs; }
    

};


#endif //MESSAGE_H
