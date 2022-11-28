/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	This file contains class methods implementation for the MSG file type
 *  defined in message.h which is utilized in SCI1/SCI32 games.
 *	
 */
 
#include "list.h"
#include "message.h"
#include "language.h"


/* methods for StringItem class */

StringItem::~StringItem()
{
   if (_phrase)
      delete [] _phrase;
   if (_frase)
      delete [] _frase;
} 


bool StringItem::setString(const char *what)
{ 
   if (_phrase)
      delete [] _phrase;
   if (_phrase = new char[(strlen(what) +1)]) 
   {   
       memcpy(_phrase, what, strlen(what) +1);
       return true;
   }
   return false;
}

bool StringItem::setTranString(const char *what)
{
   if (_frase)
   {
      delete [] _frase;
      _frase = 0;
   }
   if (what)
      if (what[0])
         if (_frase = new char[(strlen(what) +1)]) 
         {   
             memcpy(_frase, what, strlen(what) +1);
             if (memcmp(_frase, what, strlen(what) +1))
                return false;    
         }
   return true;
}

const unsigned char StringItem::getUnkByte(unsigned char which) const
{
   if (which<9)
      return _unkByte[which];

   return 0;
}

void StringItem::setUnkByte(unsigned char ub, unsigned char which)
{
   if (which<9) 
      _unkByte[which] = ub;
}

/* methods for CommentItem class*/

CommentItem::~CommentItem()
{
   if (_comment)
      delete [] _comment;
}
 

bool CommentItem::setString(const char *what)
{
   if (_comment)
      delete [] _comment;
   if (_comment = new char[(strlen(what) +1)]) 
   {   
       memcpy(_comment, what, strlen(what) +1);
       return true;
   }
   return false;  
}

const unsigned char CommentItem::getUnkByte(unsigned char which) const
{
   if (which<6)
      return _unkByte[which];

   return 0;
}

void CommentItem::setUnkByte(unsigned char ub, unsigned char which)
{
   if (which<6) 
      _unkByte[which] = ub;
}


/* Constructor and Destructor for class MsgData */

MsgData::MsgData(MsgStruct &msgh) : _msgVersionTag(msgh._msgVersionTag),
                                      _unkCount(msgh._unkCount),
                                      _phrasesCount(msgh._phrasesCount),
                                      _hasComments(false), _sorting(ByIndex)
{
   _comments = new List<CommentItem>(_phrasesCount);
   _phrases = new List<StringItem>(_phrasesCount);
   _mlist = new List<SortedMsg>(_phrasesCount);
   SortedMsg smsg;
   for (unsigned short i = 0; i<_phrasesCount; i++)
   {    
         smsg.index = i;
        _mlist->addItem(&smsg);
   }
   
   for (int i=0; i<256; i++)
   {
       _nouns[i] = 0;
       _verbs[i] = 0;
       _cases[i] = 0;
       _talkers[i] = 0;
   }
   
   _audMap = 0;
   _audMapName[0]= 0;
   
   _futureboxsize = 0;
}

MsgData::~MsgData()
{
   if (_comments)
      delete _comments;
   if (_phrases)
      delete _phrases;
   if (_mlist)
      delete _mlist;
  
   for (int i=0; i<256; i++)
   { 
     if (_nouns[i])
        delete _nouns[i];
     if (_verbs[i])
        delete _verbs[i];
     if (_cases[i])
        delete _cases[i];
     if (_talkers[i])
        delete _talkers[i];
   }
   
   if (_audMap)
      delete _audMap;      
}


/* Methods for class MsgData */

bool MsgData::setNounName(unsigned char which, const char *what)
{
   if (_nouns[which])
      delete [] _nouns[which];
   
   if (what[0])
      if (_nouns[which] = new char[(strlen(what) +1)]) 
      {   
          memcpy(_nouns[which], what, strlen(what) +1);
          return true;
      }      
   _nouns[which] = 0;
   return false;
}

bool MsgData::setVerbName(unsigned char which, const char *what)
{
   if (_verbs[which])
      delete [] _verbs[which];
        
   if (what[0])
      if (_verbs[which] = new char[(strlen(what) +1)]) 
      {   
          memcpy(_verbs[which], what, strlen(what) +1);
          return true;
      }
   _verbs[which] = 0;
   return false;
}

bool MsgData::setCaseName(unsigned char which, const char *what)
{
   if (_cases[which])
      delete [] _cases[which];
      
   if (what[0])
      if (_cases[which] = new char[(strlen(what) +1)]) 
      {   
          memcpy(_cases[which], what, strlen(what) +1);
          return true;
      }      
   _cases[which] = 0;
   return false;
}

bool MsgData::setTalkerName(unsigned char which, const char *what)
{
   if (_talkers[which])
      delete [] _talkers[which];
   
   if (what[0])
      if (_talkers[which] = new char[(strlen(what) +1)]) 
      {   
          memcpy(_talkers[which], what, strlen(what) +1);
          return true;
      }
   _talkers[which] = 0;
   return false;
}

    
char *MsgData::getNounName(unsigned char which) const
{
   return _nouns[which];   
}

char *MsgData::getVerbName(unsigned char which) const
{
   return _verbs[which];   
}

char *MsgData::getCaseName(unsigned char which) const
{
   return _cases[which];   
}

char *MsgData::getTalkerName(unsigned char which) const
{
   return _talkers[which];   
}


const char *MsgData::getPhrase(unsigned short which) const
{
   unsigned short wreal = which;   
   
   if (_mlist->getItem(which) != 0)
      wreal = _mlist->getItem(which)->index;    
   if (_phrases->getItem(wreal) != 0)
      return _phrases->getItem(wreal)->getString();
      
   return 0;   
}

const char *MsgData::getTranslation(unsigned short which) const
{
   unsigned short wreal = which;   

   if (_mlist->getItem(which) != 0)
      wreal = _mlist->getItem(which)->index;
   if (_phrases->getItem(wreal) != 0)
      return _phrases->getItem(wreal)->getTranString();
      
   return 0;   
}

const char *MsgData::getComment(unsigned short which) const
{
   unsigned short wreal = which;   

   if (_mlist->getItem(which) != 0)
      wreal = _mlist->getItem(which)->index;
   if (_comments->getItem(wreal) != 0)
      return _comments->getItem(wreal)->getString();
      
   return 0;   
}    
    
bool MsgData::setPhrase(unsigned short which, const char *what)
{
   unsigned short wreal = which;   
   
   if (_mlist->getItem(which) != 0)         //if the item doesn't exist
      wreal = _mlist->getItem(which)->index;
   if (wreal==_phrases->getLast())           //NOTE: (_phrases->getItem(wreal) == 0) would do as well
   {   //if new item, let's create it
      StringItem *newItem= new StringItem;
      _phrases->addItem(newItem);
   }
   if (_phrases->getItem(wreal) != 0)        //now set the string
      return _phrases->getItem(wreal)->setString(what);
      
   return false;   
}

bool MsgData::setTranslation(unsigned short which, const char *what)
{
   unsigned short wreal = which;   
   
   if (_mlist->getItem(which) != 0)
      wreal = _mlist->getItem(which)->index;
   if (wreal==_phrases->getLast())
   {
      StringItem *newItem= new StringItem;
      _phrases->addItem(newItem);
   }
   if (_phrases->getItem(wreal) != 0)
      return _phrases->getItem(wreal)->setTranString(what);
      
   return false;   
}

bool MsgData::setComment(unsigned short which, const char *what)
{
   unsigned short wreal = which;   

   if (_mlist->getItem(which) != 0)
      wreal = _mlist->getItem(which)->index;
   if (wreal==_comments->getLast())
   {
      CommentItem *newItem= new CommentItem;
      _comments->addItem(newItem);
   }
   if (_comments->getItem(wreal) != 0)
      return _comments->getItem(wreal)->setString(what);
      
   return false;   
}
 

long MsgData::getFileIndex(unsigned short idx) const
{
    if (_mlist->getItem(idx) != 0)
       return  _mlist->getItem(idx)->index;

    return -1;
}
 
       
const unsigned char MsgData::getUnkByte(unsigned short phrase, unsigned char which) const
{
   unsigned short wreal = phrase;   

   if (_mlist->getItem(phrase) != 0)
      wreal = _mlist->getItem(phrase)->index;
   if (_phrases->getItem(wreal) != 0)
      return _phrases->getItem(wreal)->getUnkByte(which);
      
   return 0;   
}


void MsgData::setUnkByte(unsigned short phrase, unsigned char ub, unsigned char which)
{
   unsigned short wreal = phrase;   

   if (_mlist->getItem(phrase) != 0)
      wreal = _mlist->getItem(phrase)->index;
   if (wreal==_phrases->getLast())
   {
      StringItem *newItem= new StringItem;
      _phrases->addItem(newItem);
   }
   if (_phrases->getItem(wreal) != 0)
      _phrases->getItem(wreal)->setUnkByte(ub, which);        
}
    
const unsigned char MsgData::getComUnkByte(unsigned short comment, unsigned char which) const
{
   unsigned short wreal = comment;   

   if (_mlist->getItem(comment) != 0)
      wreal = _mlist->getItem(comment)->index;
   if (_comments->getItem(wreal) != 0)
      return _comments->getItem(wreal)->getUnkByte(which);
      
   return 0;   
}

void MsgData::setComUnkByte(unsigned short comment, unsigned char ub, unsigned char which)
{
   unsigned short wreal = comment;   
 
   if (_mlist->getItem(comment) != 0)
      wreal = _mlist->getItem(comment)->index;
   if (wreal==_comments->getLast())
   {
      CommentItem *newItem= new CommentItem;
      _comments->addItem(newItem);
   }
   if (_comments->getItem(wreal) != 0)
      _comments->getItem(wreal)->setUnkByte(ub, which);        
}

void MsgData::initSort()
{
     for (unsigned short i=0; i<_phrasesCount; i++)
     {
         SortedMsg smsg;
         smsg.index = i;
         smsg.noun = getNoun(i);
         smsg.verb = getVerb(i);
         smsg.ncase = getCase(i);
         smsg.seq = getSeq(i);
         smsg.talker = getTalker(i);
         _mlist->changeItem(i, &smsg);
     }
}


bool shouldSwap(SortedMsg msg1, SortedMsg msg2, char method)
{
     unsigned long tmpl1, tmpl2;
     
     switch (method)
     {
        case ByIndex:
             if (msg1.index > msg2.index)
                return true;
             break;
             
        case ByNoun:
             if (msg1.noun > msg2.noun)
                return true;
             
             
             if (msg1.noun == msg2.noun)
             {  
                tmpl1 = msg1.verb <<24 | msg1.ncase << 16 | msg1.seq << 8 | 0;//msg1.index;
                tmpl2 = msg2.verb <<24 | msg2.ncase << 16 | msg2.seq << 8 | 0;//msg2.index;   
                if (tmpl1 > tmpl2)                                          //NOTE index can't be used like that: it's not a byte but short
                   return true;                                             //      anyway it's not important here
             }
             
             break;
             
        case ByVerb:
             if (msg1.verb > msg2.verb)
                return true;
             
             
             if (msg1.verb == msg2.verb)
             {  
                tmpl1 = msg1.noun <<24 | msg1.ncase << 16 | msg1.seq << 8 | 0;
                tmpl2 = msg2.noun <<24 | msg2.ncase << 16 | msg2.seq << 8 | 0;   
                if (tmpl1 > tmpl2)
                   return true;
             }
             
             break;
             
        case ByCase:
             if (msg1.ncase > msg2.ncase)
                return true;
             
             
             if (msg1.ncase == msg2.ncase)
             {  
                tmpl1 = msg1.verb <<24 | msg1.noun << 16 | msg1.seq << 8 | 0;
                tmpl2 = msg2.verb <<24 | msg2.noun << 16 | msg2.seq << 8 | 0;   
                if (tmpl1 > tmpl2)
                   return true;
             }
             
             break;
             
        case ByTalker:
             if (msg1.talker > msg2.talker)
                return true;
             
             
             if (msg1.talker == msg2.talker)
             {  
                tmpl1 = msg1.noun <<24 | msg1.verb << 16 | msg1.ncase << 8 | msg1.seq;
                tmpl2 = msg2.noun <<24 | msg2.verb << 16 | msg2.ncase << 8 | msg2.seq;   
                if (tmpl1 > tmpl2)
                   return true;
             }     
             
             break;
     
     
     
     }


     return false;
}

void MsgData::sortBy(char type)
{
     //bubble sort
     unsigned short i, j;
     unsigned short tindex = getFileIndex(_stringIndex);

     for (i=0; i<_phrasesCount-1; i++)
         for (j=0; j<_phrasesCount-1-i; j++)
             if (shouldSwap(*_mlist->getItem(j), *_mlist->getItem(j+1),type))
                _mlist->swapItems(j,j+1);
            

     _sorting = type;
     _stringIndex= findListIndex(tindex);
      
}


unsigned short MsgData::findListIndex(unsigned short fileidx) const
{
    unsigned short i;
    for (i=0; i<_phrasesCount; i++)
       if ((_mlist->getItem(i)->index) == fileidx)
          break;
                 
    return i;
}

unsigned short MsgData::findListIndex(SortedMsg which) const
{
    unsigned short i;
    for (i=0; i<_phrasesCount; i++)
    {
       if (which.noun == getNoun(i) &&
       which.verb == getVerb(i) &&
       which.ncase == getCase(i) &&
       which.seq == getSeq(i))
           break;          
    }                

    return i; 
}

const SortedMsg *MsgData::getLabelFromIndex(unsigned short idx) const
{ 
    if (_mlist->getItem(idx) != 0)
       return _mlist->getItem(idx);     

    return 0;       
}

SortedMsg MsgData::DuplicateSeq(SortedMsg orig)
{   
    //we must convert the current item after adding new items
    unsigned short oldcurrent = getFileIndex(_stringIndex);

    //new object to return
    SortedMsg newseq;
    newseq.verb = orig.verb;
    newseq.ncase = orig.ncase;
    newseq.seq = 1;
          
    //we search for a new noun to use      
    for (newseq.noun = 255; newseq.noun>0; newseq.noun--)
       if (findListIndex(newseq)==_phrasesCount)
          break;  //no free occurence has been found  //TODO signal error?
    
    if (newseq.noun>0)
    { //NOTE unhandled errors follows where //** stands
      setNounName(newseq.noun, CLONES_UNCLONEDSEQLABEL); 
    
      unsigned short orid = findListIndex(orig);
      do {
        
         newseq.index = _phrasesCount;
         if (!_mlist->addItem(&newseq))
         {  //can't create new object (should not happen)
            newseq.noun = 0;
            return newseq;
         }
         
         _phrasesCount++;
         //copy phrase stuff
         setPhrase(_phrasesCount-1, (char *) getPhrase(orid));     //**
         setTranslation(_phrasesCount-1, (char *) getTranslation(orid));    //**
         for (char i=0; i<9; i++)
            setUnkByte(_phrasesCount-1, getUnkByte(orid,i), i);  
         //set new noun
         setNoun(_phrasesCount-1, newseq.noun);
         //copy comment stuff
         setComment(_phrasesCount-1, (char *) getComment(orid));     //**
         for (char i=0; i<6; i++) 
            setComUnkByte(_phrasesCount-1, getComUnkByte(orid,i), i);
         //next item in the seq is ready to go!    
         orig.seq++;
         newseq.seq++;

         orid = findListIndex(orig);
      } while(orid <(_phrasesCount-newseq.seq+1)); //CHECK
      
      initSort();
      sortBy(_sorting);
      
      //restore current item
      _stringIndex = findListIndex(oldcurrent);
      
    }
    
    return newseq;
}


    
