/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Unknown
 *
 */

 #ifndef DELZW_H
 #define DELZW_H
 
#define err_None 0
#define err_Decompress_InvalidCode -0x2000
#define err_Decompress_BufferOverflow -0x2001

unsigned long lzw_Decompress (void *source, void *target, unsigned long outsize, bool msb);

#endif
