/* READMAP.C - reads resource.map, and sorts it if necessary

 resource.map contains an index to all the data used by the game. */



#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include <stdlib.h>



#include "readmap.h"

extern int version;

extern char *gamedir;



struct mapel *map, *mapptr, *mapend;

unsigned long maplen;

int ismessage = 0; //added by Drigo



/*********** sorting routines *************/



static int maporder(const void *left, const void *right)

/* qsort ordering function for map elelments */

{

  struct mapel a = *(struct mapel *)left, b = *(struct mapel *)right;

  if (a.dtype != b.dtype) return (a.dtype < b.dtype)? -1 : 1;

  if (a.dnum != b.dnum) return (a.dnum < b.dnum)? -1 : 1;

  if (a.location != b.location) return (a.location < b.location)? -1 : 1;

  return 0;

}



static struct mapel *oldreadmap(int maph)

/* read resource.map for first version of SCI */

{

  unsigned int id;

  version = 1;

  lseek(maph, 0, SEEK_SET);

  maplen = filelength(maph)/6;  /* map elements are 6 bytes in version 1 */

  map = (struct mapel *)malloc(sizeof(struct mapel)*(maplen+1));

  map[maplen].dtype = 0xff;

  map[maplen].dnum = 0xffff;

  map[maplen].location = 0xffffffff;

  mapend = map + maplen;

  /* old-style resource.map consists entirely of map entries, not sorted,

     with the data type encoded in each entry. */

  for (mapptr = map; mapptr < mapend; mapptr++) {

    read(maph, &id, 2);

    mapptr->dtype = id>>11;    /* stretch map element to fit new format */

    mapptr->dnum = id & 0x7ff;

    read(maph, &(mapptr->location), 4);

    if (mapptr->location == 0xffffffff) {

      mapend = mapptr;

      maplen = mapend-map;

      break;

    }

    if (mapptr->location & 0x04000000) version = 0;

  }

  qsort(map, maplen, sizeof(struct mapel), &maporder);

  return map;

}



struct mapel *readmap()

/* reads resource.map and returns pointer to start

   also alters globals related to map */

{

  int maph, dtype = 0, nexttype, locbytes = 4; /* size of location field varies */

  char fullname[80];


  unsigned index[39] = {0};

  int dtypestore[39] = {0}; //added by Drigo

  unsigned dtypebytes = 0; //added by drigo

  sprintf(fullname, "%s\\resource.map", gamedir);

  maph = open(fullname, O_RDONLY | O_BINARY);

  if (maph == -1) {

	sprintf(fullname, "%s\\message.map", gamedir);

	maph = open(fullname, O_RDONLY | O_BINARY);

	ismessage = 1;

  }

  if (maph == -1) return NULL;

  read(maph, &dtype, 1);

  /* new style resource.map starts with table of contents, indicating

     the starting locations of different data types in the map */

  while (dtype != 0xff) {

    if (dtype < 0x80) return oldreadmap(maph); /* invalid data type 

						  indicates old style */

    else if (dtype > 0x8f) {  /* later version has both more types and

				 smaller location fields */

     // locbytes = 3;     //drigo

     // version = 3;      //drigo

    }

    read(maph, index+dtypebytes/*(dtype & 0x7f)*/, 2); /* strip hi bit to get type # */

    dtypestore[dtypebytes] = dtype & 0x7f;

    read(maph, &dtype, 1);

    dtypebytes += 1;
  }

  dtypebytes += 1;

  dtypebytes *= 3;

  if (index[2] == 0 && !ismessage ) return oldreadmap(maph);  // drigo

  read(maph, index+dtypebytes-1, 2); /* entry for type 0xff gives total file size */

  mapptr = map = (struct mapel *)malloc(sizeof(struct mapel)*(index[dtypebytes-1] -index[0]));

  /* After the index come the map entries. Data type is not included, as

     it is given in the table of contents just read. 

     Entries are already sorted. */

  for (dtype=0; dtype<dtypebytes-1; dtype++)

    if (index[dtype]) { /* Some data types will not be included! */

      lseek(maph, index[dtype], SEEK_SET);

      for (nexttype=dtype+1; index[nexttype] == 0; nexttype++);

	/* find next non-blank entry for start of next data type */

      while (tell(maph) < index[nexttype]) {

	mapptr->dtype = dtypestore[dtype]; //dtype;

	read(maph, &(mapptr->dnum), 2);

	mapptr->location = 0; /* to clear the hi byte when locbytes = 3 */

	read(maph, &(mapptr->location), locbytes);

	if (version > 2) mapptr->location <<= 1;

	mapptr++;

      }

    }

  mapend = mapptr;

  maplen = mapend-map;

  close(maph);

  return map;

}