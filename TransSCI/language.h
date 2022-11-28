/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	Used to define string costants and messages in different languages
 *	
 */
 
#ifndef LANGUAGE_H
#define LANGUAGE_H

//UNCOMMENT ONE OF THESE LINES IF YOUR COMPILER
//DOESN'T SUPPORT -D PRECOMPILER PARAMETER

//#define ITALIAN
//#define ENGLISH

//TODO add new english strings and fix old ones

#ifdef ITALIAN
#include "italian.h"
#endif //ITALIAN




#ifdef ENGLISH
#include "english.h"
#endif // English  


#ifdef SPANISH
#include "spanish.h"
#endif // Spanish  


#endif //LANGUAGE_H


