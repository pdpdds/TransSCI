/* FotoSCIhop ecc ecc
 *		Enrico Rolfi
 */
#include "stdafx.h"

//#include "Immagina.h"
#include "p56files.h"

/* 0x8181 identifies a P56 file.*/                                  
#define P56PATCH      0x00000101   ///does it exists????
#define P56PATCH80    0x00008181
#define P56PATCHOLD	  0x00008081


P56file32::~P56file32(void)
{
	if (_palSCI != 0)
		delete _palSCI;

 	if (_vector)
		delete _vector;

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

bool P56file32::AddCell(Cell *newcell)
{
	_cells->addItem(&newcell);

	return true; //TODO cambiare!!!

}


int P56file32::LoadFile(HWND hwnd, LPSTR pszFileName)
{
	_myHwnd = hwnd;
    FILE *cfilebuf = fopen(pszFileName, "rb");
    if (cfilebuf)
    {
		//distruggere dalla memoria le strutture dati del vecchio file:
        if (_cells)
           delete _cells;
        _cells = 0;

		if (_palSCI)
           delete _palSCI;
        _palSCI = 0;
			
		//testare prima se nel file c'è l'header dei patches :
        unsigned char offset = 0;
        unsigned long patchID;
		unsigned short hsize = 0;
        

		fread((void *) &patchID, 4, 1, cfilebuf);
        if (patchID == P56PATCH80 || patchID == P56PATCH)
			offset = 4;
		if (patchID == P56PATCHOLD)
			offset = 26;

		fseek(cfilebuf, offset, SEEK_SET);

		fread((void *) &hsize, 2, 1, cfilebuf);

		switch (hsize)
		{
		case 14: //typical offset used
			_isOldFormat = false;
			break;
		case 38:
			_isOldFormat = true;
			break;
		default:
			return ID_WRONGHEADER;
		}
		
		if (!_isOldFormat) //TODO aggiungere un check anche per l'oldFormat
		{
			fseek(cfilebuf, offset+4, SEEK_SET);
			int tcellrecsize=0;
			fread(&tcellrecsize, 2, 1, cfilebuf);
			if (tcellrecsize != 0x2a)
				return ID_WRONGCELLRECSIZE;
		}
		

		fseek(cfilebuf, offset, SEEK_SET);

		P56HEAD pheader;
		
		
		fread(&pheader, (_isOldFormat ?P56HEADOLDSIZE:P56HEAD32SIZE), 1, cfilebuf);

		if (_isOldFormat)
		{
			_cellsCount = pheader.oldhead.NumCells;
			_cells = new List<Cell*>(_cellsCount);

			_isCompressed = true;
			_cellRecSize = 0x24;
			_maxWidth = pheader.oldhead.Width;
			_maxHeight = pheader.oldhead.Height;
			_zDepth = pheader.oldhead.ZDepth;
			_unkByte = pheader.oldhead.UnkByte;
			_unkShort = pheader.oldhead.UnkShort;
			_unkLong1 = pheader.oldhead.UnkLong1;
			_unkLong2 = pheader.oldhead.UnkLong2;
			_unkLong3 = pheader.oldhead.UnkLong3;
			for (int i=0; i<14; i++)
				_priBars[i] = pheader.oldhead.PriBars[i];

			_vectorSize = pheader.oldhead.VectorDataLenght;

			_vector = (unsigned char *) new char [_vectorSize];
			fseek(cfilebuf, offset+pheader.oldhead.VectorDataOffset, SEEK_SET); //TODO check 0500 bytes just before this!!!!!
			fread(_vector,_vectorSize,1,cfilebuf);
		}
		else
		{
			_cellsCount = pheader.newhead.NumCells;
			_cells = new List<Cell*>(_cellsCount);

			_isCompressed = pheader.newhead.IsCompressed;
			_cellRecSize = pheader.newhead.CellRecSize;
			_maxWidth = pheader.newhead.Width;
			_maxHeight = pheader.newhead.Height;
		}
	

		fseek(cfilebuf, offset -6 + (_isOldFormat ?pheader.oldhead.PaletteOffset:pheader.newhead.PaletteOffset), SEEK_SET);

		int ttag = 0;
		fread(&ttag, 2, 1, cfilebuf);
		if (ttag != 0x0300)
			return ID_WRONGPALETTELOC;

		unsigned long tpalsize=0;
		fread(&tpalsize, 4, 1, cfilebuf);

		_palSCI= new Palette;

		_palSCI->loadPalette(cfilebuf, tpalsize);

		//finito di caricare la palette, ora leggiamo le cells:

		if ((_isOldFormat) && _cellsCount)
		{
			short tshort;
			fseek(cfilebuf, offset + P56HEADOLDSIZE, SEEK_SET);
			fread(&tshort, 2,1,cfilebuf);
			_unkShort1 = tshort;
			fread(&tshort, 2,1,cfilebuf);
			_unkShort2 = tshort;
		}
		
		if (_cellsCount)
		{
			fseek(cfilebuf, offset+ (_isOldFormat ?pheader.oldhead.CellsOffset:pheader.newhead.CellsOffset), SEEK_SET);


			CellHeader *tcellheader = new CellHeader [_cellsCount];
			
			if (_isOldFormat)
			{
				ViewCellHeader tp;
				for (int i=0; i<_cellsCount; i++)
				{
					fread(&tp, ViewCellHeaderSIZE, 1, cfilebuf);
					memcpy(&(tcellheader[i]),&tp,ViewCellHeaderSIZE);
					tcellheader[i].zDepth=_zDepth;
					tcellheader[i].xPos=0;
					tcellheader[i].yPos=0;
				}

			}
			else
				fread(tcellheader, CellHeaderSIZE, _cellsCount, cfilebuf);


			for (int i=0; i<_cellsCount; i++)
			{
				unsigned char *tim=0;
				unsigned char *tpk=0;
				unsigned char *tln=0;

				unsigned long timsize=0;
				if (tcellheader[i].compression==0)
					//timsize = tcellheader[i].imageandPackSize; 
					//NOTE in certain files it is wrong, their tools writes this only on two bytes, and if the image is too big, it gets cut.
					timsize = tcellheader[i].width*tcellheader[i].height;
				else
					timsize = tcellheader[i].imageSize;

				fseek(cfilebuf, offset+tcellheader[i].imageOffs, SEEK_SET);
			
				tim = (unsigned char*)new char[timsize];
				fread(tim, timsize,1,cfilebuf);

				if (tcellheader[i].compression != 0)
				{
					fseek(cfilebuf, offset+tcellheader[i].packDataOffs, SEEK_SET);
					unsigned long tpksize = tcellheader[i].imageandPackSize-tcellheader[i].imageSize;
					tpk= (unsigned char* )new char[tpksize];
					fread(tpk, tpksize,1,cfilebuf);

					if (!_isOldFormat)
					{
						fseek(cfilebuf, offset+tcellheader[i].linesOffs, SEEK_SET);
						unsigned long tlnsize = tcellheader[i].height*4*2;
						tln= (unsigned char* )new char[tlnsize];
						fread(tln, tlnsize,1,cfilebuf);
					}
				}
			
				Cell *tempnewcell = new Cell;
				tempnewcell->MyPalette(&_palSCI);
				tempnewcell->LoadCell(&tcellheader[i],tim,tpk,tln,false);

				_cells->addItem(&tempnewcell);

			}
			delete [] tcellheader;
		}

		fclose(cfilebuf);

		return ID_NOERROR;		
	}

	return ID_CANTOPENFILE;
}

bool P56file32::SaveFile(HWND hwnd, LPSTR szFileName)
{
	FILE *cfilebuf = fopen(szFileName, "wb");
    if (cfilebuf)
    {
		unsigned long offset=0;
		unsigned long patchID=0;

		if (!_isOldFormat)
		{
			patchID = P56PATCH80;
			fwrite(&patchID,4,1,cfilebuf);

			offset = 4;
		}
		else
		{
			patchID =0;
			patchID += P56PATCHOLD;
			fwrite(&patchID,4,1,cfilebuf);

			unsigned short tshort=320;
			fwrite(&tshort,2,1,cfilebuf);
			tshort=200;
			fwrite(&tshort,2,1,cfilebuf);
			tshort=5;
			fwrite(&tshort,2,1,cfilebuf);
			tshort=6;
			fwrite(&tshort,2,1,cfilebuf);
			tshort=256;
			fwrite(&tshort,2,1,cfilebuf);
			tshort=0;
			for (int i=0;i<6;i++)
				fwrite(&tshort,2,1,cfilebuf);


			offset = 26;

		}

		P56HEAD pheader;

		unsigned long imageAllSize=0;
		unsigned long tagsTotalSize=0;
		for (int i=0; i<_cellsCount; i++)
		{
			Cell **tcell=_cells->getItem(i);
			imageAllSize += (*tcell)->ImageSize() + (*tcell)->PackSize();
			tagsTotalSize += (*tcell)->ImageSize();
		}

		unsigned long paletteSize = PalHeaderSIZE + (_palSCI->NumColors() * (_palSCI->HasFourEntries() ?4 :3));

		if (_isOldFormat)
		{
			pheader.oldhead.RestofHeaderSize = 38;
			pheader.oldhead.UnkShort=_unkShort;
			pheader.oldhead.NumCells = _cellsCount;
			pheader.oldhead.UnkByte=_unkByte;
			pheader.oldhead.Width = _maxWidth;
			pheader.oldhead.ZDepth =_zDepth;
			pheader.oldhead.Height = _maxHeight;
			pheader.oldhead.VectorDataLenght=_vectorSize;
			
					
			pheader.oldhead.UnkLong1=_unkLong1;
			pheader.oldhead.UnkLong2=_unkLong2;
			pheader.oldhead.UnkLong3=_unkLong3;
			

			
			pheader.oldhead.CellsOffset=68;
			if (_cellsCount)
				pheader.oldhead.CellsOffset+=4;

			for (int i=0;i<14;i++)
				pheader.oldhead.PriBars[i]=_priBars[i];


			pheader.oldhead.PaletteOffset=pheader.oldhead.CellsOffset +  ViewCellHeaderSIZE * _cellsCount + 6;
			
			if (imageAllSize)
				pheader.oldhead.PaletteOffset+= imageAllSize + 6;
			
				
			pheader.oldhead.VectorDataOffset=pheader.oldhead.PaletteOffset + paletteSize + 6;

		}
		else
		{
			pheader.newhead.CellsOffset=14;
			pheader.newhead.NumCells=_cellsCount;
			pheader.newhead.IsCompressed=_isCompressed;
			pheader.newhead.CellRecSize=_cellRecSize;
			pheader.newhead.PaletteOffset=pheader.newhead.CellsOffset + _cellsCount * _cellRecSize + 6;
			pheader.newhead.Width=_maxWidth;
			pheader.newhead.Height=_maxHeight;
		}

		fwrite(&pheader, (_isOldFormat ?P56HEADOLDSIZE:P56HEAD32SIZE), 1, cfilebuf);

		if (_isOldFormat && _cellsCount)
		{
			fwrite(&_unkShort1,2,1,cfilebuf);
			fwrite(&_unkShort2,2,1,cfilebuf);
		}


		//ora scriviamo le celle:
		CellHeader headcell;
		unsigned long imagepos;
		if (_isOldFormat)
			imagepos= pheader.oldhead.CellsOffset + _cellsCount*_cellRecSize +6;
		else
			imagepos = pheader.newhead.PaletteOffset +paletteSize +6; 
		unsigned long packpos = (_isCompressed ?imagepos + tagsTotalSize :0); //TODO check 0!!1
		unsigned long linespos=imagepos + imageAllSize +6;
		for (int i=0; i<_cellsCount; i++)
		{
			Cell **tcell=_cells->getItem(i);
			headcell = (*tcell)->RestoreCellHeader();
			
			headcell.imageOffs = imagepos;
			headcell.packDataOffs = packpos;

			imagepos += (_isCompressed ?headcell.imageSize :headcell.imageandPackSize);
			packpos += (_isCompressed ?headcell.imageandPackSize -headcell.imageSize :0);
			if (!_isOldFormat && _isCompressed)
			{	
				headcell.linesOffs = linespos;
				linespos += headcell.height*4*2;
			}

			headcell.paletteOffs = (_isOldFormat ?pheader.oldhead.PaletteOffset :0);
			
			fwrite(&headcell, (_isOldFormat ?ViewCellHeaderSIZE:CellHeaderSIZE),1,cfilebuf);
		}

		if (!_isOldFormat)	//scriviamo le palette
			_palSCI->WritePalette(cfilebuf, false);

		//ora scriviamo le immagini:

		if (_cellsCount)
		{
			unsigned short ttag = (_isOldFormat ?0x0401 :0x0400);
			fwrite(&ttag, 2,1,cfilebuf);
			fwrite(&imageAllSize, 4,1,cfilebuf);
			for (int i=0; i<_cellsCount; i++)
				(*_cells->getItem(i))->WriteImage(cfilebuf);
			for (int i=0; i<_cellsCount; i++)
				(*_cells->getItem(i))->WritePack(cfilebuf);

		}

		if (!_isOldFormat && _isCompressed)  //scriviamo le scan lines
		{
			unsigned short ttag = 0x0500;
			fwrite(&ttag, 2,1,cfilebuf);
			unsigned long tzero = 0;
			fwrite(&tzero, 4,1,cfilebuf);
			for (int i=0; i<_cellsCount; i++)
				(*_cells->getItem(i))->WriteScanLines(cfilebuf);
		}

		if (_isOldFormat)	//scriviamo le palette
			_palSCI->WritePalette(cfilebuf,false);

		if (_isOldFormat) //ora scriviamo i vettori della schermata comandi
		{
			unsigned short ttag = 0x0500;
			fwrite(&ttag, 2,1,cfilebuf);
			fwrite(&_vectorSize, 4,1,cfilebuf);
			fwrite(_vector, _vectorSize, 1,cfilebuf);
		}


		fclose(cfilebuf);

		return true;
	}

	return false;
}
