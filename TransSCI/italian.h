
#define INTERFACE_OPENAUDRESFILTER "Documenti Volume Audio (*.AUD)\0*.aud\0\0"
#define INTERFACE_OPENMAPFILTER "Documenti Audio Map (*.map)\0*.map\0Documenti Volume Map (*.map)\0*.map\0\0"
#define INTERFACE_SAVEMAPFILTER "Documenti Audio Map (*.map)\0*.map\0\0"
#define INTERFACE_OPENFILEFILTER "Documenti Message (*.msg)\0*.msg\0Tutti i tipi (*.*)\0*.*\0\0"
#define INTERFACE_SAVEFILEFILTER "Documenti Sierra Message (*.msg)\0*.msg\0Documenti TraduSCI2 Message (*.msg)\0*.msg\0\0"
#define INTERFACE_SAVEMSGFILEFILTER "Documenti Message (*.msg)\0*.msg\0\0"
#define INTERFACE_EXPORTTEXTFILTER "Documento Testo (*.txt)\0*.txt\0Tutti i tipi (*.*)\0*.*\0\0"
#define INTERFACE_LOADHEAPFILEFILTER "Documenti Heap (*.hep)\0*.hep\0Documenti Volume Map (*.map)\0*.map\0\0"
#define INTERFACE_GOBUTTON "Vai"
#define INTERFACE_FINDBUTTON "Cerca"
#define INTERFACE_REPLACEBUTTON "Sostituisci"
#define INTERFACE_CASESENSITIVE "con ABC =/= abc"
#define INTERFACE_ORIGINALBUTTON "in Messaggi"
#define INTERFACE_TRANSLATIONBUTTON "in Traduzioni"
#define INTERFACE_ORIGINAL "M\nE\nS\nS" //\nA\nG\nG\nI\nO"
#define INTERFACE_TRANSLATION "T\nR\nA\nD" //\nU\nZ\nI\nO\nN\nE"
#define INTERFACE_COMMENTS "N\nO\nT\nE"
#define INTERFACE_ITEMCOUNT " frasi"
#define INTERFACE_NOFILELOADED "Nessun file Message caricato"
#define INTERFACE_FILECOMPARETITLE "Selezionare un file da confrontare"
#define INTERFACE_SEARCHTITLE "Stato della Ricerca"
#define INTERFACE_ENDOFFILESSTR "La ricerca � ritornata sul primo file da cui � cominciata."
#define INTERFACE_INVALIDSEARCHHANDLE "La ricerca non pu� essere proseguita, il file potrebbe essere stato spostato o rimosso."
#define INTERFACE_SELECTOP "Scegliere un'operazione:"
#define INTERFACE_LABEL "Etichetta"
#define INTERFACE_CLONEOF "Clone di"
#define INTERFACE_SEARCH "in"
#define INTERFACE_INDEX "Indice"
#define INTERFACE_NOUN "Soggetto"
#define INTERFACE_VERB "Verbo"
#define INTERFACE_CASE "Caso"
#define INTERFACE_SEQUENCE "Sequenza"
#define INTERFACE_TALKER "Parlante"
#define INTERFACE_MESSAGE "Messaggio"
#define INTERFACE_MTRANSLATION "Traduzione"
#define INTERFACE_COMMENT "Note"
#define INTERFACE_UNCLONINGTITLE "Clonazione"
#define INTERFACE_PROPERTIESTITLE "Propriet�"
#define INTERFACE_PROPERTIESFORMATSTRING "\nVersione SCI: %s                   \nFormato file: %s\nNumero di frasi: %d\nParametro sconosciuto: %d\nCommenti: %s\nFile Map: %s\n"
#define INTERFACE_GENERICLABEL "<generico>"
#define INTERFACE_GENERICFLABEL "<generica>"

#define BUTTON_NEXTFILE "Prossimo file"
#define BUTTON_STOPSEARCH "Ferma"
#define BUTTON_FIRSTSTRING "Prima frase"

#define BUTTON_CANCEL "Annulla"
#define BUTTON_SELECTAUDMAP "Modifica MAP"
#define BUTTON_JUSTUNCLONE "Sclona e basta"

#define BUTTON_CLOSE "Chiudi"
#define BUTTON_LOADMSG "Apri MSG"
#define BUTTON_LOADRES "Apri Volume"
#define BUTTON_MANUAL "Manuale"

#define EXPORT_INFO "\n\n-- ATTENZIONE: La prima parte del file elenca le frasi tradotte per effettuare il controllo Ortografico. Se si aggiungono ritorni a capo in quella sezione, questi verranno inseriti nelle frasi. Inoltre, non modificare i numeri che identificano le frasi.\nI dati da questo punto in poi non sono sensibili e possono essere cancellati se non necessari. --\n\n\n"
#define EXPORT_ITEM "Frase %u:\n%s\n\n"
#define EXPORT_ENDSECTION "\n\n---- Fine della sezione ----\n"
#define EXPORT_COPYRIGHT "Questo file � stato generato da SCIaMano 1.3.1 di Enrico Rolfi.\n\n\n"
#define EXPORT_FILEPATH "Percorso:  %s\n"
#define EXPORT_FORMAT "Formato:  %s\n"
#define EXPORT_VERSION "Versione:  %s\n"
#define EXPORT_ITEMCOUNT "Numero di Frasi:  %u\n"
#define EXPORT_COMMENTS "Commenti disponibili:  %s\n"
#define EXPORT_YES "S�"
#define EXPORT_NO "No"
#define EXPORT_ITEM2 "- Frase %u - \n%s\n\n"
#define EXPORT_TRANSLATION "- Traduzione - \n%s\n\n"
#define EXPORT_COMMENTITEM " - Commento - \n%s\n\n"
#define EXPORT_ENDOFFILE "\n\n---- Fine del File ----\n\n"

#define EXPORT_DIFFTITLE "Differenze tra il file corrente e un secondo file\n\nFile 1 - %s\nFile 2 - %s\n\n"
#define EXPORT_MAPTITLE "Elenco delle frasi non riscontrate nel MAP selezionato!\n\n"

#define IMPORT_CANTFINDLABEL "Non trovo la frase %s nel file di testo, perch� al suo posto c'� %s"

#define ERR_TITLE "Errore"
#define ERR_CANTLOADFILE "Non posso caricare il file a causa di un errore."
#define ERR_FILEMOVED "Il file non esiste, � stato spostato o rimosso."
#define ERR_CANTSAVECHANGES "Non posso salvare le modifiche al file. Prova a salvare un nuovo file."
#define ERR_CANTSAVE "Non posso salvare il nuovo file a causa di un errore."
#define ERR_CANTEXPORTTEXT "Non posso esportare le frasi a causa di un errore."
#define ERR_NEWERFILEVERSION "Questo file � stato creato con una versione pi� recente di SCIaMano e non pu� essere utilizzato."
#define ERR_STRITEM1 "Si � verificato un problema di memoria in strItem->setString(tempStr) del formato Sierra."
#define ERR_STRITEM2 "Si � verificato un problema di memoria in strItem->setString(tempStr2) del formato TraduSCI."
#define ERR_STRITEM3 "Si � verificato un problema di memoria in strItem->setTranString(tempStr) del formato TraduSCI."
#define ERR_STRITEM4 "Si � verificato un problema di memoria in strItem->setString(tempStr) del formato TraduSCI."
#define ERR_COMITEM "Si � verificato un problema di memoria in comItem->setString(tempStr)."
#define ERR_UNKNOWNGAME "Il file selezionato sembra appartenere ad un gioco che non posseggo. Per favore, contattami cos� potro aggiungerne il supporto. (VersionTag = 0x%X)"
#define ERR_MESSAGEFROMAUTHOR "Messaggio da Enrico Rolfi (Endroz)"
#define ERR_FIELD1 "Non posso creare il campo delle frasi."
#define ERR_FIELD2 "Non posso creare il campo delle traduzioni."
#define ERR_FIELD3 "Non posso creare il campo dei commenti."
#define ERR_FIELD4 "Non posso creare il campo indice."
#define ERR_FIELD5 "Non posso creare il campo cerca."
#define ERR_FIELD6 "Non posso creare il campo sostituisci."
#define ERR_DIFFIMPFILE "Il file selezionato ha un numero diverso di frasi e non pu� essere importato."
#define ERR_DIFFCMPFILE "Il file selezionato ha un numero diverso di frasi e non pu� essere confrontato."
#define ERR_CANTCMPSAMEFILE "Il file selezionato � lo stesso che si sta gi� utilizzando!"
#define ERR_CANTALLOCSTRING "Si � verificato un problema di memoria. La frase tradotta non � stata salvata."
#define ERR_CANTLOADHEAPFILE "Errore nel caricamento del file Heap!"

#define ERR_CANTLOADDLL "Impossibile caricare la libreria SCIdump.DLL! Il programma si bloccher� qualora si tentasse di caricare un Volume."

#define WARN_ATTENTION "Attenzione!"
#define WARN_SIERRAFORMAT "Il file � in formato Sierra, e pu� memorizzare solo le frasi tradotte, al posto di quelle originali. Inoltre non supporta i dati speciali di TraduSCI (come i MAP e le etichette).\nSi desidera convertirlo in formato TraduSCI e conservare entrambe le versioni delle frasi (ed i dati speciali)?"
#define WARN_OLDFORMAT "Il file � nel vecchio formato TraduSCI, salvando le modifiche sar� convertito automaticamente nel formato TraduSCI2 e non potr� essere pi� utilizzato dalle versioni precedenti del programma. Continuare?"
#define WARN_UNSAVEDCHANGES "Le modifiche al file attuale non sono state salvate. Vuoi salvarle prima di aprire un'altro file?"
#define WARN_QUITONUNSAVED "Le modifiche al file attuale non sono state salvate. Vuoi salvarle prima di chiudere SCIaMano?"

#define WARN_TEMPORARYFILEEXISTS "E' stato trovato un file temporaneo relativo ad una sessione non chiusa correttamente, o tuttora in corso. Si desidera caricare questo file temporaneo?"  

#define WARN_ENDOFFILEDURINGSEARCH "Fine del file. \n\nE' possibile ricominciare dalla prima frase o passare al file successivo."

#define WARN_ENDTAGNAMESNOTFOUND "C'� un problema con questo file. La fine del blocco delle etichette non � stata riconosciuta! Procedere con cautela."
#define WARN_ENDTAGMAPSNOTFOUND "C'� un problema con questo file. La fine del blocco delle Audio MAP non � stata riconosciuta! Procedere con cautela."


#define MENU_MESSAGE "&Messaggi"
#define	MENU_LOAD "&Carica...\tCtrl+O"
#define	MENU_OPENVOL "&Apri da Volume..."
#define	MENU_SAVE "&Salva\tCtrl+S"
#define	MENU_SAVEAS "S&alva come..."

#define MENU_IMPORT "&Importa da file di testo..."
#define MENU_EXPORT "&Esporta su file di testo..."
#define	MENU_ABOUT "&Informazioni..."
#define	MENU_QUIT "&Esci\tCtrl+Q"
#define MENU_PROPERTIES "&Propriet�..."  
#define MENU_CLONE "Clonazione della frase corrente..."

#define MENU_NOUNSHEAP "Importa nomi Soggetti da file Heap..."
#define MENU_LOADLABELSTXT "Importa da file di testo..."
#define MENU_EXPORTLABELSTOTXT "Esporta su file di testo"

#define MENU_LABELS "Etichette"

#define MENU_STRING "&Frasi"
#define MENU_FIRST "&Prima\tCtrl+Home"
#define MENU_PREVIOUS "P&recedente\tCtrl+ Freccia S�"
#define MENU_NEXT "&Successiva\tCtrl+ Freccia Gi�"
#define MENU_LAST "&Ultima\tCtrl+Fine"
#define MENU_ENTER "&Vai\tCtrl+Invio"
#define MENU_SEARCHBAR "&Barra delle opzioni\tCtrl+B"
#define MENU_LISTBAR "&Elenco delle frasi\tCtrl+E"
#define MENU_FIND "&Cerca\tCtrl+F"
#define MENU_REPLACE "S&ostituisci\tCtrl+R"
#define MENU_IMPORTFILE "I&mporta da Message..."
#define MENU_COMPAREFILE "Co&nfronta con Message..."


#define MENU_SHOWUNKBYTES "&Mostra i dati speciali"

#define MENU_WORDL "Glossario"
#define MENU_ADDTOWL "Aggiungi selezione\tCtrl+A"
#define MENU_DOWORDSLOOKUP "Controlla parole\tCtrl+K"
#define MENU_SHOWWL "Visualizza Glossario"

#define MENU_FONT "&Dimensione caratteri"
#define MENU_FONT14 "14"
#define MENU_FONT16 "16"
#define MENU_FONT18 "18"
#define MENU_FONT20 "20"
#define MENU_FONT22 "22"
#define MENU_FONT24 "24"
#define MENU_FONT26 "26"

#define MENU_NAVIGATION "&Navigazione"
#define MENU_BYINDEX "per Indice"
#define MENU_BYNOUN "per Soggetto"
#define MENU_BYVERB "per Verbo"
#define MENU_BYCASE "per Caso"
#define MENU_BYTALKER "per Parlante"

#define MENU_GRAPH "Grafi"
#define MENU_DRAWGF "Disegna Grafo\tCtrl+G"
#define MENU_DRAWGFBW "Disegna Grafo senza colori"

#define MENU_OTHERGRAPH "...con altro ordinamento"
#define MENU_DRAWNOUNGF "Grafo dei Soggetti"
#define MENU_DRAWCASEGF "Grafo dei Casi"
#define MENU_DRAWVERBGF "Grafo dei Verbi"
#define MENU_DRAWTALKERGF "Grafo dei Parlanti"
#define MENU_DRAWINDEXGF "Grafo degli Indici"

#define MENU_AUDIO "Audio"
#define MENU_LOADMAP "Carica MAP..."
#define MENU_EXPORTMAP "Esporta MAP corrente..."
#define MENU_VERIFYMAP "Verifica messaggi nel MAP..."

#define MENU_PLAY "Riproduci frase corrente"
#define MENU_LOADAUDRES "Carica 'RESOURCE.AUD'..."
#define MENU_AUTOPLAY "Riproduci automaticamente"

#define MENU_NEXTCOMMENT "Vai alla prossima Nota\tCtrl+D"

#define MENU_HELP "Manuale di TraduSCI..."

#define WIN_FILEDESCRIPTION "Programma per tradurre giochi SCI1.1/SCI32.\0"
#define WIN_LEGALCOPYRIGHT "\251 Enrico Rolfi (Endroz), 2003-2014.\0"
#define WIN_LEGALTRADEMARKS "SCI (Sierra Creative Interpreter) � un marchio registrato di Sierra On-Line che ne detiene i diritti.\0"
#define WIN_FILEVERSION "1.3.1\0"
#define WIN_PRODUCTVERSION "2.1\0"
#define WIN_FILEVERSIONID 1,3,1,0
#define INTERFACE_ABOUTBOX "Questo programma � open source (licenza GNU GPL), pu� essere distribuito e modificato liberamente purch� resti open source. L'autore declina da ogni responsabilit� circa eventuali danni causati dal suo utilizzo.  SCI � un marchio registrato di Sierra On-Line."
#define INTERFACE_ABOUTTITLE "\251 Enrico Rolfi (Endroz), 2003-2014\n\nAggiornamenti su http://erolfi.wordpress.com\nPer problemi, suggerimenti o altro scrivimi a: \n erolfi@gmail.com\n\n"
#define INTERFACE_VERSION "Versione 1.3.1"

#define MANUAL_PATH "Manuale/istruzioni.html"


#define GRAPH_CLONEOF "\\fn\nClone di Soggetto %d, Verbo %d, Caso %d"
#define GRAPH_NOUN "Soggetto"
#define GRAPH_VERB "Verbo"
#define GRAPH_CASE "Caso"
#define GRAPH_TALKER "Parlante"
#define GRAPH_CANTCREATEFILE "Non posso creare il file del grafico (forse il disco � protetto?)."

#define MAPS_REPLACEEXISTING "Questo file Message ha gi� un Audio MAP caricato al suo interno, sei sicuro di volerlo sostituire?"
#define MAPS_MAPFILENOTFOUND "Questo file MSG necessita di un file MAP valido per poter funzionare nel gioco. Si vuole crearlo adesso (sovrascrivendo un eventuale file MAP omonimo presente in questa cartella)?"
#define MAPS_WRONGENDOFMAPFILE "Il file MAP non ha un formato riconosciuto quindi non � possibile caricarlo.\n(l'ultimo elemento non � -1 come atteso)"
#define MAPS_WRONGMAPFILESIZE "La dimensione del file MAP non � corretta quindi non � possibile caricarlo."
#define MAPS_STRANGESCIPATCHINMAPFILE "Il file MAP non ha un formato riconosciuto quindi non � possibile caricarlo.\n(non � presente l'indicatore di PATCH SCI corretto all'inizio del file)"
#define MAPS_CANTACCESSMAPFILE "Non � stato possibile accedere al file MAP!"
#define MAPS_ITEMALREADYEXISTS "Non � stato possibile inserire l'elemento nel MAP! Esiste gi� un elemento con la stessa etichetta."
#define MAPS_LISTERRORDURINGINSERT "Non � stato possibile inserire l'elemento nel MAP!\n(errore durante l'inserimento nella lista)"
#define MAPS_CANTFINDORIGINALTOCOPYFROM "Non � stato possibile inserire l'elemento nel MAP! La frase originale da cui copiare il riferimento all'audio non � stata trovata."
#define MAPS_NOMAPINMEMORY "Non � presente in memoria una struttura MAP sulla quale effettuare le modifiche richieste!"
#define MAPS_MAPCHECKFORMATSTRING "Indice: %d - Soggetto: %d  Verbo: %d  Caso: %d  Sequenza: %d%s\n"
#define MAPS_BUTITISACLONE " (ma � un CLONE)"
#define MAPS_CANTFINDENTRY "Il riferimento all'audio non � stato riscontrato nel MAP (potrebbe trattarsi di un clone)!"

#define MAPS_WASNTREMOVEDMIGHTBEUSEFUL "La voce nel MAP audio non � stata eliminata perch� fa riferimento ad un suono ancora esistente, che potrebbe tornare utile in futuro se si decider� di sclonarla nuovamente."
#define MAPS_ENTRYREMOVED "La voce nel MAP audio � stata correttamente rimossa."
#define MAPS_TITLE "Audio MAP"

#define MAPS_NOTLOADED "non caricato"

#define AUDIO_CANTFINDRESAUD "Non trovo il volume RESOURCE.AUD! Caricarlo mediante l'apposita voce di questo stesso men�. Se � gi� stato fatto in precedenza, allora controllare che non sia stato spostato o rimosso."
#define AUDIO_NOTASOLFILE "Il file RESOURCE.AUD o l'audio MAP in uso potrebbero essere errati. L'indirizzo presente nella mappa AUDIO non punta ad un file audio di tipo SOL!"
#define AUDIO_MUSTLOADMAPFIRST "Per poter riprodurre questa frase � necessario caricare la mappa AUDIO di questo file message (es. file 100000.map)."
 
#define CLONES_UNCLONEBTN "Sclona"
#define CLONES_CLONEBTN "Clona"
#define CLONES_SEQLABEL "SEQ"
#define CLONES_HIERARCHICAL "Si ricorda che frasi appartenenti a liste o altre interfacce potrebbero essere gestite negli script, e se sclonate potrebbero bloccare il gioco! (Nota: sclonando questa frase si otterr� ancora un clone, in quanto la frase di origine � a sua volta un clone; quindi sar� necessario ripetere l'operazione di sclonazione fino ad ottenere una frase modificabile.)" 
#define CLONES_DUPLICATESEQ "Questa frase clona un'intera sequenza! Piuttosto che sclonare la frase (ottenendo una copia SOLO del primo elemento della sequenza), si vuole mantenerla clonata e DUPLICARE l'intera sequenza a cui fa riferimento?"
#define WARN_UNCLONINGAREYOUSURE "Si ricorda che frasi appartenenti a liste o altre interfacce potrebbero essere gestite negli script, e se sclonate potrebbero bloccare il gioco!\n\nIn presenza di doppiaggio vocale, � ESSENZIALE modificare anche il file di mappatura dell'audio(.MAP), cliccando il tasto qui sotto (consultare il manuale per maggiori informazioni sui cloni):" 
#define CLONES_CANTUNCLONESEQ "Non ho potuto creare un duplicato della sequenza clonata!"
#define CLONES_UNCLONINGSUCCEDED "Operazione di sclonazione riuscita."
#define CLONES_CANTCLONESEQINTERMEDIATE "Non si pu� clonare una frase intermedia di una sequenza!"
#define CLONES_UNCLONEDSEQLABEL "SEQ SCLONATA"
#define CLONES_LABELFORMAT "%sClone di Soggetto %d, Verbo %d, Caso %d"
#define CLONES_CLONINGENTERINDEX "Con questa procedura, la frase corrente verr� trasformata in un clone!\n Digitare l'INDICE del primo messaggio della sequenza di riferimento da cui creare il clone:"
#define CLONES_CLONINGREMARK "Nota: qualora nel file MAP fosse presente un riferimento al messaggio corrente, la procedura di clonazione non lo eliminer� (a meno che questa frase non fosse in origine un clone modificato da SCIaMano). Si ricorda che, in presenza di un clone, il gioco user� SEMPRE l'audio relativo al messaggio originale."
#define CLONES_CANTCLONEWRONGINDEX "L'INDICE inserito non corrisponde a nessun messaggio!"

#define WORDS_WORDALREADYEXISTS "La parola esiste gi� nel prontuario. Si vuole aggiungere un'ulteriore definizione?"
#define WORDS_CANTACCESSWORDSFILE "Non posso aprire il prontuario. Verificare che il file \"wordlookup.txt\" si trovi nella stessa directory del file MSG correntemente aperto."
#define WORDS_NOSELECTIONTOADD "Non c'� nessuna selezione da aggiungere al prontuario!"
#define WORDS_CANTCREATEREPORT "Non posso creare il file contenente il rapporto!"

#define WORDS_WFHEADERROW1 "# File di prontuario dei termini comuni per i file MSG\n#\n"
#define WORDS_WFHEADERROW2 "#  La sintassi di ogni riga del file deve essere del tipo:\n#    <parola originale> = <traduzione1> | <traduzione2> | ...\n#\n"
#define WORDS_WFHEADERROW3 "#  Le parole possono contenere spazi nel mezzo, quindi\n#  si possono memorizzare anche traduzioni di intere frasi\n"
#define WORDS_WFHEADERROW4 "#  Per inserire dei commenti usare il carattere # \n"

#define WORDS_MISSEDMATCHFORMAT "%d mancata corrispondenza nella frase %d\n"
#define WORDS_MISSEDMATCHSFORMAT "%d mancate corrispondenze nella frase %d\n"
#define WORDS_MISSEDWORDFORMAT "per la parola \"%s\" da tradurre come \"%s\""
#define WORDS_MISSEDALTERNATEWORDFORMAT " o \"%s\""
#define WORDS_NOTESFORMAT "note dal prontuario: %s"
#define WORDS_DEFSCOUNTF "\nNel prontuario ci sono %d definizioni."

#define LABELS_REDEFINENAMEFROMSCRIPT "%s %d � gi� stato chiamato '%s' in precedenza, si preferisce rinominarlo '%s'?"

#define IMPORT_WOULDYOULIKEORIGINALSTRING "Si vogliono importare le frasi originali al posto delle traduzioni?"
#define IMPORT_WOULDYOULIKELABELSEARCH "Importare le frasi identificando mediante le etichette piuttosto che per indice?"

#define WARNING_FUTUREBOXDATA "Questo file � stato salvato con una versione pi� recente di TraduSCI, e contiene dei dati aggiuntivi che verranno lasciati inalterati. Si raccomanda di non modificare questo file usando una versione di SCIaMano precedente alla 1.2, od i suddetti dati andranno persi."
