# VirtualMemorySim
API and Implementation of a Virtual Memory system simulator.
_____________________________________________________________
spazio totale di indicizzazione (16 MB): memoria virtuale  
memoria fisica (1 MB): lista di Frame  

indirizzi logici a 24 bit  
indirizzi fisici a 20 bit  
frame di dimensione (frame size):  256 Byte

politica di page replacement (swap): second chance algorithm  

Memoria fisica implementata come lista (array) di Frame  
    init(List * list)    
    isEmpty(List * list)     
    get(List * list, int pos)    
    add(List * list, Frame * frame)  
    removeFrame(List * list, int pos)    

La MMU contiene i puntatori alla struttura:  
    tabella delle pagine     
    wrapper della memoria fisica (lista di frame)    
    spazio di swap (lista di frame)
e menatiene sincronizzate le strutture a seguito delle invocazioni dei metodi che fanno side effect sulla memoria (accessi in lettura/scrittura).   
Se il bit Valid nella tabela delle pagine è true allora il frame puntato dalla pagina si trova in swap space oppure non è ancora stato caricato => accesso a tale frame genera un page fault. 

La lista di Frame della memoria fisica ha due flag:
    ValidFrame settato a 0 se il Frame è libero
    Reserved settato a 1 se il Frame è del S.O. 
        (ES. allocazione della tab delle pagine) e NON può subire swap out.