// SCIdump.cpp : definisce il punto di ingresso dell'applicazione DLL.
//

#include "stdafx.h"
#include "SCIdump.h"
#include "volume.hpp"

HINSTANCE hInst;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	hInst = (HINSTANCE) hModule; 
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


SCIDUMP_API BOOL ExtractFromVolumeSkel(HWND hwnd, char *mapname, unsigned char type, char *exten, char *filename)
{
	return ExtractFromVolume(hwnd, mapname, type, exten, filename);
}

