/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Unknown
 *
 */
 
 
#include "stdafx.h"
#include "delzw.h"

typedef struct {
  unsigned short        PrefixCode;
  unsigned char         Literal;
} DictEntry;

unsigned short GetBits (unsigned char **source, unsigned short *bitpos, unsigned short bits, bool msb) {
        unsigned long x32;
        unsigned char *s;
        unsigned short x;

        s = *source;
		if (msb)
		{
			x32 = (s[3] + (s[2]<<8) + (s[1]<<16) + (s[0]<<24));
			x = ((x32 >> (32-(*bitpos+bits))) & ((1 << bits)-1) );
		}
		else
		{
			x32 = (s[0] + (s[1]<<8) + (s[2]<<16) + (s[3]<<24));
			x = ((x32 >> *bitpos) & ((1 << bits)-1) );
		}
		*bitpos += bits;
        while (*bitpos >= 8) {
          s++;
          *bitpos -= 8;
        }
        *source = s;
        return (x);
}        

unsigned long lzw_Decompress (void *source, void *target, unsigned long outsize, bool msb) {
              unsigned char  *s, *t;
        unsigned short  BitPosition;
        unsigned long  BytesWritten;
        unsigned short  CodeLength, NextCode, MaxCode, ReadCode, ThisCode, PrefixCode;
        unsigned char   LastCharacter;
        unsigned char   stringBuf [256];
        unsigned short  stringCount;
        DictEntry  Dictionary [4096];

        s = (unsigned char *)source;
        t = (unsigned char *)target;
        BitPosition = 0; BytesWritten = 0;
        CodeLength = 9; NextCode = 0x0102; MaxCode = 0x1ff;
        stringCount = 0;

		if (msb)
		{
			PrefixCode = GetBits (&s, &BitPosition, CodeLength, msb);
			*t++ = PrefixCode; BytesWritten++;
		}

        while (BytesWritten <= outsize) {
           ReadCode = GetBits (&s, &BitPosition, CodeLength, msb);
           ThisCode = ReadCode;
           if (ReadCode == 0x101) return (BytesWritten);
           if (ReadCode == 0x100) {
              CodeLength = 9;
              NextCode = 0x0102;
              MaxCode = 0x1ff;
              PrefixCode = GetBits (&s, &BitPosition, CodeLength, msb);
              LastCharacter = PrefixCode;
              *t++ = PrefixCode; BytesWritten++;
              continue;
           }
           while (1==1) {       // examine code
              if (ThisCode > NextCode)
                 return (err_Decompress_InvalidCode);
              if (ThisCode == NextCode) {       // repeat last character
                 // put last character into string buffer
                 stringBuf [stringCount] = LastCharacter;
                 stringCount++;
                 ThisCode = PrefixCode;
                 continue;      // examine previous code
              }
              if (ThisCode >= 0x100) {          // compressed code
                 // put that code's literal into string buffer
                 stringBuf [stringCount] = Dictionary [ThisCode].Literal;
                 stringCount++;
                 ThisCode = Dictionary [ThisCode].PrefixCode;
                 continue;
              }
              // literal character: put into string buffer
              stringBuf [stringCount] = ThisCode;
              stringCount++;
              LastCharacter = ThisCode;
              // output string buffer
              while (stringCount > 0) {
                stringCount--;
                *t++ = stringBuf [stringCount]; BytesWritten++;
              }
              // update dictionary
              if (NextCode <= MaxCode) {        // prevent dictionary overflow
                 Dictionary [NextCode].Literal    = LastCharacter;
                 Dictionary [NextCode].PrefixCode = PrefixCode;
                 NextCode++;
                 // check if more bits needed
                 bool bitcheck = msb ? (NextCode >= MaxCode) : (NextCode > MaxCode);
                 if (bitcheck && CodeLength < 12) { 
                    CodeLength++;
                    MaxCode = ((1 << CodeLength) -1);
                 }
              }
              PrefixCode = ReadCode;
              break;
           }
        }
        // if here, outsize is not big enough
        return (err_Decompress_BufferOverflow);
}               
