/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2021.
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
//#define SPANISH


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


