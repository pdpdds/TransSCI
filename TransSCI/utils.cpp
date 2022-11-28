/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	These are functions used for I/O of strings. We need to use them
 *	instead of the stdio ones cause these ones allows any control
 *  character to be read without terminating string. Also it converts
 *  OEM chars to ANSI.
 *
 */


#include "utils.h"
#include"common.h"

char* my_fgets(char* caBuffer, int nBufferSize, FILE* fileRead)
{
	if (nBufferSize <= 0)
		return 0;

	//char tBuffer[nBufferSize];
	//WCHAR tBuffer2[nBufferSize * 2];

	char* tBuffer = new char[nBufferSize];
	WCHAR* tBuffer2 = new WCHAR[nBufferSize];

	int k = 0;
	nBufferSize--; //we should save an extra byte for termination
	for (int c = fgetc(fileRead); c > 0; c = fgetc(fileRead), k++) {
		if (k == nBufferSize)
			break;
		tBuffer[k] = (unsigned char)c;
	}
	tBuffer[k] = '\0';

	//we replace  OemToAnsi(tBuffer, caBuffer);   with a safer technique

	MultiByteToWideChar(
		CP_OEMCP, //437,         //IBM437
		0,
		tBuffer,
		-1,
		tBuffer2,
		nBufferSize
	);

	WideCharToMultiByte(
		CP_ACP,
		0,
		tBuffer2,
		-1,
		caBuffer,
		nBufferSize * 2,
		"?UNK?",
		0
	);

	delete tBuffer;
	delete tBuffer2;


	return caBuffer;
}

int	my_fputs(const char* szOutput, FILE* fileWrite)
{
	int nBufferSize = strlen(szOutput) + 1;
	
	//char tBuffer[nBufferSize];
	//WCHAR tBuffer2[nBufferSize * 2];

	char* tBuffer = new char[nBufferSize];
	WCHAR* tBuffer2 = new WCHAR[nBufferSize];

	//we replace AnsiToOem(szOutput, tBuffer);   with a safer technique

	MultiByteToWideChar(
		CP_ACP,
		0,
		szOutput,
		-1,
		tBuffer2,
		nBufferSize
	);

	WideCharToMultiByte(
		CP_OEMCP, //437,       //IBM437
		0,
		tBuffer2,
		-1,
		tBuffer,
		nBufferSize * 2,
		"?UNK?",
		0
	);



	nBufferSize--; //we should save an extra byte for termination
	unsigned char c = tBuffer[0];
	for (int k = 0; k < nBufferSize; k++, c = tBuffer[k]) {
		if (c == '\0')
			break;
		fputc(c, fileWrite);
	}
	fputc('\0', fileWrite);

	delete tBuffer;
	delete tBuffer2;

	return 0; //TODO add error signals
}

bool findText(char* text, int& pos, char* word, bool wcase)
{
	int len = strlen(text);
	int wlen = strlen(word);
	if (wcase) {
		for (; pos < len; pos++)
			if (!strncmp(word, (char*)text + pos, wlen))
				return true;
	}
	else {
		for (; pos < len; pos++)
			if (!strncasecmp(word, (char*)text + pos, wlen))      //NOTE: not case sensitive compare
				return true;
	}

	return false;
}
