# VirtualMemorySim
API and Implementation of a Virtual Memory system simulator.
_____________________________________________________________
spazio totale di indicizzazione (16 MB): memoria virtuale
memoria fisica (1 MB): lista di Frame

indirizzi logici a 24 bit
indirizzi fisici a 20 bit
frame di dimensione (frame size): <!-- TODO -->
=> page size
politica di page replacement (swap): second chance algorithm

Memoria fisica implementata come lista (array) di Frame
    init(List * list)
    isEmpty(List * list)
    get(List * list, int pos)
    add(List * list, Frame * frame)
    removeFrame(List * list, int pos)

