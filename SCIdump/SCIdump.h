// Il seguente blocco ifdef rappresenta il modo standard di creare macro che semplificano 
// l'esportazione da una DLL. Tutti i file all'interno della DLL sono compilati con il simbolo SCIDUMP_EXPORTS
// definito nella riga di comando. Tale simbolo non deve essere definito in un progetto
// che utilizza questa DLL. In questo modo qualsiasi altro progetto i cui file di origine includono questo file vedranno le funzioni 
// SCIDUMP_API come importate da una DLL, mentre la DLL vedr?i simboli
// definiti con questa macro come esportati.
#ifdef SCIDUMP_EXPORTS
#define SCIDUMP_API __declspec(dllexport)
#else
#define SCIDUMP_API __declspec(dllimport)
#endif

