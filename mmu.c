#include "mmu.h"
#include "globals.h"
#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

MMU initMemSystem(){
//inizializzo le strutture dati
    MMU mmu;
    mmu.page_table = (Page_Table *) malloc(sizeof(Page_Table));
    mmu.frame_memory_wrapper = (FrameMemoryWrapper *) malloc(sizeof(FrameMemoryWrapper));
    mmu.swap_file = fopen("swap_file.bin", "w");

//SWAP
    char c = 0;
    fseek(mmu.swap_file, 0, SEEK_SET); //azzero l'indice di scorrimento del file
    for (int i = 0; i < SIZE_VIRTUAL_MEMORY; i++){ //creo il file di 16 MB
        fwrite(&c, sizeof(char), 1, mmu.swap_file);
    }
//MEM
    //mmu.frame_memory_wrapper->frames          MEMORIA FISICA !!
    mmu.frame_memory_wrapper->freeFrames = malloc(sizeof(List));
    mmu.frame_memory_wrapper->num_frames = NUM_FRAMES; 
    //inizialmente ci sono NUM_FRAMES frame liberi in ram
    init(mmu.frame_memory_wrapper->freeFrames); //struttura di gestione dei frame liberi
    //printf("debug: ho creato la memoria ram 0");
    for(int i = 0; i < NUM_FRAMES; i++){
        //mmu.frame_memory_wrapper->frames[i].offset            NON IMPLEMETATO
        memset(mmu.frame_memory_wrapper->frames[i].info, 0, SIZE_PAGE);
        //azzero il frame  in memoria
        mmu.frame_memory_wrapper->frames[i].flags = 0;
        //setto il Frame a libero
        add(mmu.frame_memory_wrapper->freeFrames, &(mmu.frame_memory_wrapper->frames[i]));
        //aggiungo il frame appena creato alla lista dei frame liberi
    }
    //printf("debug: ho creato la memoria ram");
    //devo allocare la tabella delle pagine all'inizio della memoria 
    for (int i = 0; i < (SIZE_PAGE_TABLE / SIZE_PAGE); i++){
        Frame * riservato = removeFrame(mmu.frame_memory_wrapper->freeFrames, 0);
        riservato->flags |= ValidFrame;
        riservato->flags |= Reserved;
    }
//PAGES
    for (int i = 0; i < NUM_PAGES; i++){
        mmu.page_table->pages[i].frame_number = -1;
        mmu.page_table->pages[i].flags = 0;
    }
    mmu.page_table->num_pages = NUM_PAGES;
    
    printf("avvio del sistema... inizializzazione \n");
    return mmu;
}
void freeMemSystem(MMU * mmu){
    fclose(mmu->swap_file);
    
    free(mmu->frame_memory_wrapper->freeFrames);
    free(mmu->frame_memory_wrapper);

    free(mmu->page_table);
    printf("chiusura del sistema \n");
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
    //fare swap out di un frame in mem fisica se completa (no Reserved)
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