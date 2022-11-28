#include "StdAfx.h"
#include "scicell.h"
#include "p56files.h"
//#include "palette.h"

//#include "immagina.h"

void Cell::makeSCI()
{	
	if (_cached && _cachedHeader)
	{
		if (_image)
			delete _image;

		_image = 0;

		if (_pack)
			delete _pack;

		_pack = 0;

		bool hasLines = (_lines!=0);

		if (hasLines)
			delete _lines;

		_lines = 0; 
		
		_width = (unsigned short) _cachedHeader->bmiHeader.biWidth;
		_height = (unsigned short) -_cachedHeader->bmiHeader.biHeight;

		_imageSize = _width*_height;
		_packSize = 0;

	    _image = (unsigned char *) new char [_imageSize];


		int dwremainder = _width%4;
		if (dwremainder)
			dwremainder = 4-dwremainder; //bmp vuole il DWORD align per ogni scanline

		unsigned short bmpwidth = _width +dwremainder;


		if (!_compression)
			for (unsigned short i=0; i<_height; i++)
				for (unsigned short j=0; j<_width; j++)
					_image[i*_width +j] = _cached[i*bmpwidth +j];

		else	//compressed
		{
			_pack = (unsigned char *) new char [_imageSize];
			if (hasLines)
				_lines = (unsigned char *) new char [_height*4*2];

			unsigned short i=0;
			unsigned short j=0, oldj=0;

			unsigned char *ppack = _pack;
			unsigned char *pimage = _image;

			unsigned long *ptaglines;
			unsigned long *pdatalines;
			if (hasLines)
			{
				ptaglines = (unsigned long *) _lines;
				pdatalines = ((unsigned long *) _lines) +_height;		
			}
			unsigned char *pcached = _cached;
						
			for (i=0; i<_height; i++)
			{
				j = 0;
				pcached = _cached + (i*bmpwidth);

				if (hasLines)
				{
					ptaglines[i]=(unsigned long)(pimage-_image);
					pdatalines[i]=(unsigned long)(ppack-_pack);
				}

				unsigned char b1, b2, b3, cont;

				
			
				do
				{
					if (_width-j>2)
					{
						b1 = pcached[0];
						b2 = pcached[1];
						b3 = pcached[2];
						if ((b1==b2) || (b1==255))
						{
							pcached +=1;
							j+=1;
							cont =1;
							while ((j<_width) && (cont<0x3F))
							{
								b2=pcached[0];
								if (b2!=b1)
									break;
								else
								{
									cont++;
									pcached++;
									j++;
								}

							}
							if (b1==255)
							{
								pimage[0]=0xC0 + cont;
								pimage++;
							}
							else
							{
								pimage[0]=0x80 + cont;
								pimage++;
								ppack[0]=b1;
								ppack++;
							}
						}
						else	//b1!=b2 || b2!=b3
						{
							cont =1;

							while ((j+cont<_width-2) && (cont<0x3F)) //NOTE WAS 0x7F in Gabriel Knight, when scanlines are used, the max value allowed is 3F even here.
							{                                        //TODO fixare, mettere uno switch se HasLines magari, perché questo fatto
								b1=b2;                               // ingrandisce la grandezza del file rispetto all'originale
								b2=b3;
								b3=pcached[cont+2];
								
								if ((b1==b2) && ((b2==b3) || (b2 ==255)))
									break;
								else
									cont++;
							}
                            if ((j+cont==_width-2) && (cont<0x3E) && (b3!=b2)) //NOTE as in the upper note
                               cont+=2;
							
							pimage[0]=cont;
							pimage++;
							for(int z=0;z<cont;z++)
							{
								ppack[0]=pcached[0];
								ppack++;
								pcached++;
							}
							j+=cont;
						}
				
					}
					else		//_width-j <3
					{
						cont =_width-j;
						b1=pcached[0];
                        if (cont==2)
                           b2=pcached[1];
                        
                        if ((b1==b2) || (cont ==1))
                        {
                           if (b1==255)
                           {
	                           pimage[0]=0xC0 + cont;
							   pimage++;  
             			   }
						   else
						   {
							   pimage[0]=0x80 + cont;
							   pimage++;
							   ppack[0]=b1;
							   ppack++;
						   }    
						}
						else
                        {	
						    pimage[0]=cont;
						    pimage++;

							ppack[0]=b1;
							ppack[1]=b2;
                            ppack+=cont;
							
						}
      
                        pcached+=cont;   
						j+=cont;
							
					}
				} while (j<_width);
			}
	
			_packSize= (unsigned long) (ppack -_pack);
			_imageSize = (unsigned long) (pimage -_image);
			
		}

		
	}
}

long Cell::makeBitmap()
{
	if (_cached)
		delete _cached;

	if (_cachedHeader)
		delete _cachedHeader;

	unsigned long imsize = _width*_height;
	int dwremainder = _width%4;
	if (dwremainder)
		imsize+= _height*(4-dwremainder); //bmp vuole il DWORD align per ogni scanline
	unsigned char *initdata;

	long tsizep = ((sizeof(BITMAPINFO)) + 256*(sizeof(RGBQUAD)));

	BITMAPINFO *binfo = (BITMAPINFO *) new char[tsizep];
	

	binfo->bmiHeader.biBitCount=8;
	binfo->bmiHeader.biClrImportant=256;
	binfo->bmiHeader.biClrUsed=256;//TODO forse cambiare
	binfo->bmiHeader.biCompression=BI_RGB;
	binfo->bmiHeader.biHeight=-_height; //so that it will be a top-down DIB
	binfo->bmiHeader.biPlanes=1;
	binfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);//boh
	binfo->bmiHeader.biSizeImage=imsize;//was 0;
	binfo->bmiHeader.biWidth=_width;
	binfo->bmiHeader.biXPelsPerMeter=0; //nu
	binfo->bmiHeader.biYPelsPerMeter=0; //nu

	RGBQUAD tquad;
	for (int i=0; i<256; i++)
	{
		PalEntry *tpal =_palette->GetPalEntry(i);
        
		if (tpal != NULL)
		{
			tquad.rgbBlue=tpal->blue;
			tquad.rgbGreen=tpal->green;
			tquad.rgbRed=tpal->red;
			tquad.rgbReserved=0;
		}
		else
		{
			tquad.rgbBlue=0;
			tquad.rgbGreen=0;
			tquad.rgbRed=0;
			tquad.rgbReserved=0;
		}
		binfo->bmiColors[i]= tquad;

	}

	
	
	if (_compression==0)
	{
		if (!dwremainder)
			initdata =  _image;
		else
		{
			initdata = (unsigned char *) (new char[imsize]);
			
			unsigned char *pdata = _image;

			unsigned char *pinit = initdata;
			for (int i=0; i<_height; i++)
			{
				memcpy(pinit, pdata, _width);
				pinit += _width;
				pdata += _width;
				for (int j=0; j<4-dwremainder;j++)
				{
					pinit[0] = 0;
					pinit++;	
				}
			}

		}
	}
	else
	{
		initdata = (unsigned char *) (new char[imsize]);

		unsigned char *ptags = _image;
		unsigned char *pdata = _pack;

		unsigned char *pinit = initdata;
  
        unsigned long *plines = (unsigned long *)_lines;
	
		//TODO inserire verifica delle posizioni tramite le scan lines!!
		int i=0;
		do
		{
            
            if (HasLines())
			{
                //HWND thwnd = ((P56file32*)_parent)->MyHWnd();
				if (plines[i] != (unsigned long)(ptags-_image))
                    MessageBox(hWnd, "lines tags errate", "Eccolo",
                                MB_OK | MB_ICONEXCLAMATION);
				if (plines[i+_height] != (unsigned long)(pdata-_pack))
                    MessageBox(hWnd, "lines colori errate", "Eccolo",
                                MB_OK | MB_ICONEXCLAMATION);
		    }
            
			int curwidth=0;
			do
			{
				switch ((*ptags)>>6)
				{
				case 2:	//80
				{
					unsigned char color = *pdata;
					pdata++;
					for (int j=0; j<((*ptags)-0x80); j++)
					{
						pinit[0] = color; 
						pinit++;
						curwidth++;
					}
					break;
				}
				case 3:	//C0
					for (int j=0; j<((*ptags)-0xC0); j++)
					{
						pinit[0] = 255; 
						pinit++;
						curwidth++;
					}
					break;
				default:
					for (int j=0; j<*ptags; j++)
					{
						pinit[0] = *pdata;
						pdata++;
						pinit++;	
						curwidth++;
					}

				}

				ptags++;
			
			} while (curwidth<_width);
			//TODO controllare qui la linea!!!

			if (dwremainder) //se non divis per 4
				for (int j=0; j<4-dwremainder;j++)
				{
					pinit[0] = 0;
					pinit++;	
				}

			i++;

		} while (i<_height);
		
		//HWND thwnd = ((P56file32*)_parent)->MyHWnd();
		if (imsize!=(long)(pinit-initdata))
			MessageBox(hWnd, "grandezza decompressa diversa da quella attesa", "Eccolo",
            MB_OK | MB_ICONEXCLAMATION);
		imsize = imsize-((long)(pinit-initdata));

	}



//if (tbmap)
	_cached = initdata;
	_cachedHeader = binfo;

	return imsize;
}


void Cell::LoadCell(CellHeader *chead, unsigned char *im, unsigned char *pk, unsigned char *lin, bool isView)
{
	if (_image)
		delete _image;

	_image = im;

	if (_pack)
		delete _pack;

	_pack = pk;

	if (_lines)
		delete _lines;

	_lines = lin;


	_width = chead->width;
	_height = chead->height;
	_left = chead->xShift;
	_top = chead->yShift;
	_skipColor = chead->transparentClr;
	_compression = chead->compression;      // 0 - none, 8A - rle
	_flags = chead->flags;            // &0x80 -> &1 - UseSkip, &2 - remap_status
	
	if (isView)
	{
		_zDepth = 0;		 
		_xPos = 0;
		_yPos = 0;
	}
	else
	{
		_zDepth = chead->zDepth;
		_xPos = chead->xPos;
		_yPos = chead->yPos;
	}

	if (_compression)
	{
		_imageSize = chead->imageSize;
		_packSize = chead->imageandPackSize-_imageSize;
	}
	else
	{
		//_imageSize = chead->imageandPackSize;
		//non affidabile!!!!
		_imageSize = chead->height*chead->width;
		_packSize = 0;
	}

}

CellHeader Cell::RestoreCellHeader()
{
	CellHeader tch;

	tch.compression=_compression;
	tch.flags=_flags;
	tch.height=_height;
	tch.imageandPackSize=(_compression ?_imageSize+_packSize :_imageSize);
	tch.imageOffs=0;
	tch.imageSize=(_compression ?_imageSize: 0); //TODO CHECK NON SO se è 0 o 6... in GK2 è 0
	tch.linesOffs=0;
	tch.packDataOffs=0;
	tch.paletteOffs=0;
	tch.transparentClr=_skipColor;
	tch.width=_width;
	tch.xPos=_xPos;
	tch.xShift=_left;
	tch.yPos=_yPos;
	tch.yShift=_top;
	tch.zDepth=_zDepth;

	return tch;


}

	
void Cell::WriteImage(FILE *cfb)
{
	if (cfb)
	{
		fwrite(_image, _imageSize, 1, cfb);
		_changed = false;
	}
}

void Cell::WritePack(FILE *cfb)
{
	if (cfb)
	{
		if (_compression)
			fwrite(_pack, _packSize, 1, cfb);


	}
}

void Cell::WriteScanLines(FILE *cfb)
{
	if (cfb)
	{
		if (_lines)
			fwrite(_lines, _height*4*2, 1, cfb);


	}


}
