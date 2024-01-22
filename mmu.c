#include "mmu.h"
#include "globals.h"
#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//MMU initMemSystem(){


//}
void freeMemSystem(MMU * mmu){


}

PhysicalAddress getPhysicalAddr(MMU * mmu, LogicAddress logicAddr){
    int page_number = (logicAddr.addr >>(24-8));
    //logic addr 24 bit diviso in frame 8 bit => MSB primi 16 bit di logic addr (page number)
    printf("page number: %d \n", page_number); 

    int offset = logicAddr.addr & 0xFF; // logicAddr & 1111 1111
    //LSB ultimi 8 bit di logic addr
    PhysicalAddress * addrFisico = (PhysicalAddress *)malloc(sizeof(PhysicalAddress));

    if(!(mmu->page_table->pages[page_number].flags & Valid)){
        //la pagina richiesta (frame) non sta in mem fisica ma in swap
        //PAGE FAULT
        MMU_exception(mmu, page_number); 
        //fa lo swap in della pagina richiesta con politica SECOND CHANCE ALG
        //una volta completata l'operazione la tab pagine avrà invalid bit a false
        // e punterà al frame corretto in mem fisica
    }

    int frame_numer = mmu->page_table->pages[page_number].frame_number;
    addrFisico->addr = (frame_numer << (20-8) ) | offset;
    return *addrFisico;
}

void MMU_exception(MMU * mmu, int pos) {
    //pos è la page_number alla quale si è tentato di accedere 
    //ma risiedeva in swap anzichè in mem fisica => PAGE FAULT da gestire
    //fare swap out di un frame in mem fisica se completa
    // e swap in del frame richiesto (pagina) dallo spazio swap

    Frame * in;
    if(!isEmpty(mmu->frame_memory_wrapper->freeFrames)){
        //ho un frame libero in ram => carico il frame direttamente

        //rimuovo il primo frame libero
        in = removeFrame(mmu->frame_memory_wrapper->freeFrames, 0);

        //aggiorno la page table e carico il frame in memoria fisica
        mmu->page_table->pages[pos].flags |= Valid;
        //printf("page number %d :flags: %d \n", pos, mmu->page_table->pages[pos].flags);
        mmu->page_table->pages[pos].frame_number = in->frame_number;


    }



}