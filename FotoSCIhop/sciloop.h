
#ifndef SCILOOP_H
#define SCILOOP_H

#include "list.h"
#include "scicell.h"
//#include "palette.h"


#pragma pack(1)



struct LOOPHEADER {
     char  BasedOnLoop;   //Number of loop
     bool  Mirror;        //IsMirror
     unsigned char  NumberOfCells; //NumCells -> 0 if based on another loop (?)
     long  UnkLong1;		//sembra sempre FF //TODO Brian Provinciano's sources: pointer to mirrored cells
     char  UnkByte;		//sembra sempre 3
     long  UnkLong2;		//sembra sempre 0
     unsigned long  CellsOffs;
};

#define LOOPHEADERSIZE (sizeof(LOOPHEADER))

#pragma pack()

class Loop
{
	List<Cell*> *_cells;
	unsigned char _cellsCount;
	unsigned char _selectedCell;

	char  _basedOnLoop;   //Number of loop
    bool  _mirror;        //IsMirror


	long  _unkLong1;		//sembra sempre FF
    char  _unkByte;		//sembra sempre 3
    long  _unkLong2;		//sembra sempre 0

public:
	
	Loop(void) : _cellsCount(0), _cells(0), _selectedCell(0) {}
	~Loop(void);

	List<Cell*> *Cells() const { return _cells; }
	void Cells(List<Cell*> *tl) { _cells = tl; }

	unsigned char CellsCount() const { return _cellsCount; }
	void CellsCount(unsigned char value) { _cellsCount = value; }

	unsigned char SelectedCell() const { return _selectedCell; }
	void SelectedCell(unsigned char value) { _selectedCell = value; }

	bool Mirrored() const { return _mirror; }
	void Mirrored(bool value) { _mirror = value; }

	char BasedOn() const { return _basedOnLoop; }
	void BasedOn(char value) { _basedOnLoop = value; }

	bool AddCell(Cell *newcell) { return _cells->addItem(&newcell); }

	LOOPHEADER RestoreLoopHeader();

	bool LoadLoop(LOOPHEADER *plh);
};




#endif
