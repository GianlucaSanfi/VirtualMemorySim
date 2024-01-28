# VirtualMemorySim
API and Implementation of a Virtual Memory system simulator.
_____________________________________________________________
spazio totale di indicizzazione (16 MB): memoria virtuale  
memoria fisica (1 MB): lista di Frame  

indirizzi logici a 24 bit  
indirizzi fisici a 20 bit  
frame di dimensione:  4096 Byte

NOTA: TLB (translation lookaside buffer) non implementato

politica di page replacement (swap): second chance algorithm  


USO del programma: ./test {VERBOSE}|{}
    Lanciare il programma di test con un argomento attiva 
        i messaggi di log durante l'esecuzione (usarlo solo
        se si vuole avere un report definito di tutti i passi
        che svolge l'algoritmo);
COMPILAZIONE: make
    Crea la libreria statica e l'eseguibile 
        (i file .o vengono eliminati)
    "make clean" per rimuovere lobreria ed eseguibile

Il programma di test esegue:
    * prima uno stress test con pattern di accesso sequenziale
    * successivamente uno con pattern di accesso randomico