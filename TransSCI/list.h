/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	This generic list class is used both with StringItem and CommentItem
 *  classes to store multiple occurences of them sorted in memory.
 *
 *  List version 3: it's dynamic! (you can add and remove items)
 */


#ifndef LIST_H
#define LIST_H                   

template <class T>
class List {
private:
	//unsigned short _size;
    unsigned short _last;
          
	T **_item;

public:
    //List(void) : _size(0), _item(0), _last(0) { }
                               //NOTE: size is discarded: our list class is now dynamically allocated
	List(const unsigned short size) : /*_size(size), */_last(0), _item(new T*[0x0000FFFF]) { }

	~List() { if (_item) delete [] _item; }
    
	//unsigned short getSize() const { return _size; }
    unsigned short getLast() const { return _last; }
	T *getItem(unsigned short which) const;
	bool addItem(const T *it);
    bool changeItem(const unsigned short which, const T *it);
    bool insertItem(unsigned short loc, const T *it);
    bool swapItems(unsigned short item1, unsigned short item2);
    bool removeItem(unsigned short which);
};

 


template <class T>
bool List<T>::addItem(const T *it)
{
   if (it && (_last<0x0000FFFF))
   {
      _item[_last] = new T;
      (*_item[_last++])=*it;
      return true;
   }
   return false;
}

template <class T>  
T *List<T>::getItem(unsigned short which) const 
{
   if (which<_last)
      return _item[which];
   return 0;  
}

template <class T>
bool List<T>::swapItems(unsigned short item1, unsigned short item2)
{
    if ((item1<_last) && (item2<_last))
    {
       T *tmp = _item[item1];
       
       _item[item1] = _item[item2];
       _item[item2] = tmp;

       return true;
    }
    return false;
}

template <class T>
bool List<T>::changeItem(const unsigned short which, const T *it)
{
   if (which<_last)
   {
      (*_item[which])=*it;
      return true;
   }   
   return false;
}

template <class T>
bool List<T>::insertItem(unsigned short loc, const T *it)
{
   if (it && (_last<0x0000FFFF) && (loc<=_last))
   {
      for (unsigned short j=_last++; j>loc; j--)
          _item[j]=_item[j-1];

      _item[loc] = new T;
      (*_item[loc])=*it;

      return true;
   }
   return false; 
}

template <class T>
bool List<T>::removeItem(unsigned short which)
{
     if (_item[which]) delete _item[which];
     else return false;
     
     _last--;
     
     for (unsigned short j=which; j<_last; j++)
          _item[j]=_item[j+1];

     return true;
}

#endif 
