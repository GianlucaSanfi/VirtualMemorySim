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
        mmu.frame_memory_wrapper->frames[i].page_number = -1;
        mmu.frame_memory_wrapper->frames[i].frame_number = (i & 0xFFF);
        
        memset(mmu.frame_memory_wrapper->frames[i].info, 0, SIZE_PAGE);
        //azzero il frame  in memoria
        mmu.frame_memory_wrapper->frames[i].flags = Valid;
        //setto il Frame a libero
        add(mmu.frame_memory_wrapper->freeFrames, &(mmu.frame_memory_wrapper->frames[i]));
        //aggiungo il frame appena creato alla lista dei frame liberi
    }
    //devo allocare la tabella delle pagine all'inizio della memoria 
    for (int i = 0; i < (SIZE_PAGE_TABLE / SIZE_PAGE); i++){
        Frame * riservato = removeFrame(mmu.frame_memory_wrapper->freeFrames, 0);
        riservato->flags |= Unswappable;
    }
//PAGES
    for (int i = 0; i < NUM_PAGES; i++){
        mmu.page_table->pages[i].frame_number = 0;
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
    int page_number = (logicAddr.addr >> BIT_FRAME) & 0xFFFF;
    //logic addr 24 bit diviso in frame 8 bit => MSB primi 16 bit di logic addr (page number)
    //printf("page number: %x \n", page_number); 

    int offset = logicAddr.addr & 0xFF; // logicAddr & 1111 1111
    //LSB ultimi 8 bit di logic addr
    //printf("offset : %x \n", offset);
    PhysicalAddress * addrFisico = (PhysicalAddress *)malloc(sizeof(PhysicalAddress));

    if(!(mmu->page_table->pages[page_number].flags & Valid)){
        //la pagina richiesta (frame) non sta in mem fisica ma in swap
        //PAGE FAULT
        printf("PAGE FAULT \n");
        MMU_exception(mmu, page_number); 
        //fa lo swap in della pagina richiesta con politica SECOND CHANCE ALG
        //una volta completata l'operazione la tab pagine avrà invalid bit a false
        // e punterà al frame corretto in mem fisica
    }

    int frame_number = mmu->page_table->pages[page_number].frame_number;
    //printf("frame number : %x \n", frame_number);
    addrFisico->addr = (frame_number << (20-8) ) | offset;
    return *addrFisico;
}

void MMU_exception(MMU * mmu, int page_number) {
    //pos è la page_number alla quale si è tentato di accedere 
    //AGE FAULT da gestire
    //fare swap out di un frame in mem fisica se completa (no Reserved)
    // e swap in del frame richiesto (pagina) dallo spazio swap

    

    //1) in memoria ho frame liberi (no SWAP OUT)
    if(!isEmpty(mmu->frame_memory_wrapper->freeFrames)){
        //ho un frame libero in ram => carico il frame direttamente
        Frame * in;
        //rimuovo il primo frame libero
        in = removeFrame(mmu->frame_memory_wrapper->freeFrames, 0);

        //aggiorno la page table
        mmu->page_table->pages[page_number].flags |= Valid;
        //printf("page number %x :flags: %x \n", page_number, mmu->page_table->pages[page_number].flags);
        mmu->page_table->pages[page_number].frame_number =  in->frame_number;
        
        //devo settare il frame ad occupato e aggiornare la page number
        in->page_number = page_number;
        in->flags &= ~Valid;
        return;
    }
    //2) in memoria NON ho frame liberi (SWAP OUT)
    int choose = 0, i = 0;
    Frame * out;
    while(!choose){
        //SECOND CHANCE ALGORITHM
        out = &(mmu->frame_memory_wrapper->frames[i]);
        uint32_t out_flags = out->flags;

        if(out_flags & Unswappable)
            continue;
        if(((out_flags & Read_bit)== 0) &&((out_flags &Write_bit) == 0)){
            choose = 1;
            continue;
        }
        if(out_flags & Read_bit){
            out->flags &= ~Read_bit; //abbasso il flag Read
            continue;
        }
        if(out_flags & Write_bit){
            out->flags |= Read_bit; //alzo il flag Read
            out->flags &= ~Write_bit;
            //se è stato modificato devo copiare il contenuto in swap per poterlo salvare
            continue;
        }

        i++;
        if(i == NUM_FRAMES) i = 0;
    }
    //aggiorno la page table
    mmu->page_table->pages[page_number].flags |= Valid;
    //printf("page number %x :flags: %x \n", page_number, mmu->page_table->pages[page_number].flags);
    mmu->page_table->pages[page_number].frame_number =  out->frame_number;
        
    //devo settare il frame ad occupato e aggiornare la page number
    out->page_number = page_number;
    out->flags &= ~Valid;
    return;
}


void MMU_writeByte(MMU * mmu, int pos, char c) {
    //pos è un intero => 
    LogicAddress l;
    l.addr = pos & 0xFFFFFF; //24 bit

    PhysicalAddress phy = getPhysicalAddr(mmu, l);
    uint32_t frame_number = (phy.addr >> 8) & 0xFFF; //MSB 12 bit
    Frame * frame = &(mmu->frame_memory_wrapper->frames[frame_number]);

    int off = phy.addr & 0xFF;
    frame->info[off] = c;

    fseek(mmu->swap_file, frame->page_number, SEEK_SET); 
    //posiziono il cursore all'indice page_number
    for(int i = 0; i < 256; i++)
        fwrite(&(frame->info[i], sizeof(char), 1, mmu->swap_file));
    return;

}
char * MMU_readByte(MMU * mmu, int pos){

    return " ";
}
