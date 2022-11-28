/* FotoSCIhop 
 *
 *
 */


#ifndef SCICELL_H
#define SCICELL_H

#include "stdafx.h"
#include "palette.h"



#pragma pack(1)

struct CellHeader
{
	short width;
	short height;
	short xShift;
	short yShift;
	char  transparentClr;   // Transparent color idx (Skip)
	char  compression;      // 0 - none, 8A - rle
	short flags;            // &0x80 -> &1 - UseSkip, &2 - remap_status
							// Note: if hibit is clear, UseSkip must be rechecked for uncompr., set 4 compr
	unsigned long imageandPackSize;      //Size of compressed image + size of pack image
				//if uncompressed, size of image block only
	unsigned long imageSize;       	//Size of compressed image
				//if uncompressed, 0x06 (for some displacement matter, I believe)

	unsigned long  paletteOffs;    //0x00000000		nu in later SCI
							//IMPORTANT WHEN EDITING OR LOADING CHECK IF != 0
	unsigned long  imageOffs;        //Offset of Cell's image or pack data tags (if compressed)
	unsigned long  packDataOffs;    //Offset of Pack data image 
	unsigned long  linesOffs;     //Offset of Scan Lines Table (if compressed)
	short zDepth; //-> -1000 - normal bg
	short xPos;
	short yPos;

};
#define CellHeaderSIZE (sizeof(CellHeader))

struct ViewCellHeader
{
	short width;
	short height;
	short xShift;
	short yShift;
	char  transparentClr;   // Transparent color idx (Skip)
	char  compression;      // 0 - none, 8A - rle
	short flags;            // &0x80 -> &1 - UseSkip, &2 - remap_status
							// Note: if hibit is clear, UseSkip must be rechecked for uncompr., set 4 compr
	unsigned long imageandPackSize;      //Size of compressed image + size of pack image
				//if uncompressed, size of image block only
	unsigned long imageSize;       	//Size of compressed image
				//if uncompressed, 0x06 (for some displacement matter, I believe)

	unsigned long  paletteOffs;    //0x00000000		nu in later SCI32
							//IMPORTANT WHEN EDITING OR LOADING CHECK IF != 0
	unsigned long  imageOffs;        //Offset of Cell's image or pack data tags (if compressed)
	unsigned long  packDataOffs;    //Offset of Pack data image 
	unsigned long  linesOffs;     //Offset of Scan Lines Table (if compressed)
};
#define ViewCellHeaderSIZE (sizeof(ViewCellHeader))

union CELLS {
	CellHeader newcell;
	ViewCellHeader oldcell;
};

#pragma pack()

class Cell		//size 2A
{
	unsigned short _width;
	unsigned short _height;
	short _left;
	short _top;

	unsigned char _skipColor;
	unsigned char _compression; 
	unsigned short _flags;
	
	unsigned char *_image;
	unsigned long _imageSize;
	unsigned char *_pack;
	unsigned long _packSize;
	unsigned char *_lines;

	BITMAPINFO *_cachedHeader;
	unsigned char *_cached;

	bool _changed;
	//if set to true, the new image is the one referenced by _cached
	//that was the initial Idea, but now it is suddenly converted

	short _zDepth;
	unsigned short _xPos;
	unsigned short _yPos;
 
    Palette *_palette;
	
public:
	Cell(void) : _image(0), _pack(0), _lines(0), _cached(0), _cachedHeader(0), _changed(false), _palette(0) {}
    ~Cell(void) { if (_image) delete _image; if (_pack) delete _pack;
					if (_lines) delete _lines; 
					if (_cached && (_image != _cached)) delete _cached;
					if (_cachedHeader) delete _cachedHeader;}

	unsigned short Width() const { return _width; }
	void Width(unsigned short value) { _width = value; }
	unsigned short Height() const { return _height; }
	void Height(unsigned short value) { _height = value; }
	short Left() const { return _left; }
	void Left(short value) { _left = value; }
	short Top() const { return _top; }
	void Top(short value) { _top = value; }
	
	unsigned char SkipColor() const { return _skipColor; }
	void SkipColor(unsigned char value) { _skipColor = value; }
	
	unsigned char Compression() const { return _compression; }
	void Compression(unsigned char value) { _compression = value; }
	
	unsigned short Flags() const { return _flags; }
	void Flags(unsigned short value) { _flags = value; }

	void GetImage(BITMAPINFO **imhd, unsigned char **im) { if (_cached==0)
															makeBitmap();
															*imhd=_cachedHeader;
															*im=_cached;}
	void SetImage(BITMAPINFO *imhd, unsigned char *im) { if (_cached && (_image != _cached))
														 	delete _cached;
														 if (_cachedHeader)
															delete _cachedHeader;
														_cached = im; 
														_cachedHeader = imhd;
														_changed = true;
														makeSCI();
														}

	long makeBitmap();	//TODO aggiungere il msgbox sul risultato di ritorno!!!
	void makeSCI();


	short ZDepth() const { return _zDepth; }
	void ZDepth(short value) { _zDepth = value; }

	unsigned short XPos() const { return _xPos; }
	void XPos(unsigned short value) { _xPos = value; }
	unsigned short YPos() const { return _yPos; }
	void YPos(unsigned short value) { _yPos = value; }

	void LoadCell(CellHeader *chead, unsigned char *im, unsigned char *pk, unsigned char *lin, bool isView);

	Palette **MyPalette() { return &_palette; }
	void MyPalette(Palette **value) {
									_palette = *value;
									if (_cached && (_image != _cached))
										delete _cached;
									if (_cachedHeader)
										delete _cachedHeader;

										_cachedHeader = 0;
										_cached=0;
									}

	unsigned long ImageSize() const { return _imageSize; }
	void ImageSize(unsigned long value) { _imageSize = value; }

	unsigned long PackSize() const { return _packSize; }
	void PackSize(unsigned long value) { _packSize = value; }

	bool HasLines() const { return (_lines!=0); }

	bool Changed() const { return _changed; }

	CellHeader RestoreCellHeader();

	void WriteImage(FILE *cfb);
	void WritePack(FILE *cfb);

	void WriteScanLines(FILE *cfb);

};

#endif
