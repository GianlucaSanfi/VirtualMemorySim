# VirtualMemorySim
API and Implementation of a Virtual Memory system simulator.
_____________________________________________________________
spazio totale di indicizzazione (16 MB): memoria virtuale \n
memoria fisica (1 MB): lista di Frame \n

indirizzi logici a 24 bit \n
indirizzi fisici a 20 bit \n
frame di dimensione (frame size):  1Byte <!-- TO confirm --> \n

politica di page replacement (swap): second chance algorithm \n

Memoria fisica implementata come lista (array) di Frame \n
    init(List * list)   \n
    isEmpty(List * list)    \n
    get(List * list, int pos)   \n
    add(List * list, Frame * frame) \n
    removeFrame(List * list, int pos)   \n

La MMU contiene i puntatori alla struttura: \n
    tabella delle pagine    \n
    wrapper della memoria fisica (lista di frame)   \n
    spazio di swap (lista di frame) \n
e menatiene sincronizzate le strutture a seguito delle invocazioni dei metodi che fanno side effect sulla memoria (accessi in lettura/scrittura).   \n
Se il bit Invalid è true allora il frame puntato dalla pagina si trova in swap space => accesso a tale frame genera un page fault. \n