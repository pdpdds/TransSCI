
#include "StdAfx.h"
#include "sciloop.h"

Loop::~Loop() 
{ 	
	if (_cells != 0)
	{
		for (unsigned short i=0; i<_cells->getLast(); i++)
		{
			Cell **texcell = _cells->getItem(i);
			delete (*texcell);		
		}
		
		delete _cells;
	}


}

bool Loop::LoadLoop(LOOPHEADER *plh)
{
	_cellsCount=plh->NumberOfCells;

	if (_cells)
		delete _cells;

	_cells = new List<Cell*>(_cellsCount);

	_basedOnLoop = plh->BasedOnLoop;
	_mirror = plh->Mirror;
	_unkByte = plh->UnkByte;
	_unkLong1=plh->UnkLong1;
	_unkLong2=plh->UnkLong2;

	return (_cells != 0);
}

LOOPHEADER Loop::RestoreLoopHeader()
{
	LOOPHEADER tlh;

	tlh.BasedOnLoop=_basedOnLoop;
	tlh.CellsOffs=0;
	tlh.Mirror=_mirror;
	tlh.NumberOfCells=_cellsCount;
	tlh.UnkByte=_unkByte;
	tlh.UnkLong1=_unkLong1;
	tlh.UnkLong2=_unkLong2;


	return tlh;

}
