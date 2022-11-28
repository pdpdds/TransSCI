// stdafx.h : file di inclusione per file di inclusione di sistema standard
// o file di inclusione specifici del progetto utilizzati di frequente, ma
// modificati raramente
//

#pragma once

// Modificare le seguenti definizioni se ?necessario creare una piattaforma prima di quelle specificate di seguito.
// Fare riferimento a MSDN per informazioni aggiornate sui valori corrispondenti per le differenti piattaforme.
#ifndef WINVER				// Consente l'uso delle caratteristiche specifiche di Windows XP o versioni successive.
#define WINVER 0x0501		// Modificarlo con il valore appropriato per altre versioni di Windows.
#endif

#ifndef _WIN32_WINNT		// Consente l'uso delle caratteristiche specifiche di Windows XP o versioni successive.                   
#define _WIN32_WINNT 0x0501	// Modificarlo con il valore appropriato per altre versioni di Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Consente l'uso delle caratteristiche specifiche di Windows 98 o versioni successive.
#define _WIN32_WINDOWS 0x0410 // Modificarlo con il valore appropriato per Windows Me o versioni successive.
#endif

#ifndef _WIN32_IE			// Consente l'uso delle caratteristiche specifiche di IE 6.0 o versioni successive.
#define _WIN32_IE 0x0600	// Modificarlo con il valore appropriato per altre versioni di IE.
#endif

//#define WIN32_LEAN_AND_MEAN		// Escludere gli elementi utilizzati di rado dalle intestazioni di Windows
// File di intestazione di Windows:
#include <windows.h>



// TODO: fare riferimento qui alle intestazioni aggiuntive richieste dal programma
