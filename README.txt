# VirtualMemorySim
API and Implementation of a Virtual Memory system simulator.
_____________________________________________________________
spazio totale indirizzabile (16 MB): memoria virtuale  
memoria fisica (1 MB): lista di Frame  

indirizzi logici a 24 bit  
indirizzi fisici a 20 bit  
frame di dimensione:  4096 Byte

OTTIMIZZAZIONI: 
    TLB (translation lookaside buffer), dimensione: MAX 10 record
        politica di rimpiazzo: LRU

politica di page replacement (swap): second chance algorithm  


USO del programma: ./test [S]|[R] {VERBOSE}|{}
    * S : pattern di accesso sequenziale
    * R : pattern di accesso randomico 
    * Lanciare il programma di test con l'argomento VERBOSE attiva 
        i messaggi di log durante l'esecuzione (usarlo solo
        se si vuole avere un report definito di tutti i passi
        che svolge l'algoritmo, altrimenti si satura l'output della 
        shell e rende la simulazione meno leggibile);
COMPILAZIONE: make
    Crea la libreria statica e l'eseguibile 
        (i file .o vengono eliminati)
    "make clean" per rimuovere lobreria ed eseguibile

Il programma di test può eseguire:
    * uno stress test con pattern di accesso sequenziale
    * uno stress test con pattern di accesso randomico
Parametri di default:
    sequenziale:    1536 WRITE,     1280 READ
    randomico:      1536 WRITE,     1024 READ