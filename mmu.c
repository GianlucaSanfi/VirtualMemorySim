#include "mmu.h"
#include "globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

MMU initMemSystem(){
    printf("avvio del sistema... inizializzazione \n");
//inizializzo le strutture dati
    MMU mmu;
    mmu.pageTable = (PageTable *) malloc(sizeof(PageTable));
    mmu.memory = (Memory *) malloc(sizeof(Memory));
    mmu.swap_file = fopen("swap_file.bin", "w+");

//SWAP
    char c = 0;
    fseek(mmu.swap_file, 0, SEEK_SET); //azzero l'indice di scorrimento del file
    for (int i = 0; i < SIZE_VIRTUAL_MEMORY; i++){ //creo il file di 16 MB
        fwrite(&c, sizeof(char), 1, mmu.swap_file);
    }

//MEM
    for(int i = 0; i < NUM_FRAMES; i++){
        mmu.memory->frames[i].page_number = -1;
        memset(mmu.memory->frames[i].info, 0, SIZE_PAGE);
        //azzero il frame  in memoria
    }
    mmu.memory->size = 0;
    
//PAGES
    for (int i = 0; i < NUM_PAGES; i++){
        mmu.pageTable->pages[i].frame_number = 0;
        mmu.pageTable->pages[i].flags = 0;
    }
    mmu.pageTable->size = 0;

    //devo allocare la tabella delle pagine all'inizio della memoria 
    int frames_for_pageTable = (sizeof(PageTable) / SIZE_PAGE);
    // = ((sizeof(PageEntry) * NUM_PAGES + sizeof(int))/ SIZE_PAGE);
    for (int i = 0; i < frames_for_pageTable; i++){
        mmu.pageTable->pages[i].flags |= Unswappable;
        mmu.pageTable->size++;
    }
//STATISTICS
    mmu.stats->TOTAL_PAGE_FAULTS = 0;
    
    printf("avvio del sistema... inizializzazione completata \n");
    return mmu;
}

void freeMemSystem(MMU * mmu){
    printf("chiusura del sistema... ");

    fclose(mmu->swap_file);
    free(mmu->memory);
    free(mmu->pageTable);

    printf("% % % % % % ");
    printf("...chiusura del sistema completata\n");
}

PhysicalAddress getPhysicalAddr(MMU * mmu, LogicalAddress logicAddr){

    int page_number = (logicAddr.addr >> BIT_FRAME) & 0xFFF; 
    int offset = logicAddr.addr & 0xFFF;
    
    if(!(mmu->pageTable->pages[page_number].flags & Valid)){
        //la pagina richiesta (frame) non è valida
        //PAGE FAULT
        printf("PAGE FAULT \n");
        if(MMU_exception(mmu, page_number) != 0) return NULL; 
        
    }
    PhysicalAddress * addrFisico = (PhysicalAddress *) malloc(sizeof(PhysicalAddress));

    int frame_number = mmu->pageTable->pages[page_number].frame_number;
    //printf("frame number : %x \n", frame_number);
    addrFisico->addr = (frame_number << 12 ) | offset;
    return *addrFisico;
}

int _swap_in(MMU * mmu){

    return 0;
}
int _swap_out(MMU * mmu){

    return 0;
}

int MMU_exception(MMU * mmu, int page_number) {
    //pos è la page_number alla quale si è tentato di accedere     

    mmu->stats->TOTAL_PAGE_FAULTS++;

    //in memoria ho frame liberi (NO SWAP)
    if(mmu->memory->size < NUM_FRAMES){
        int i = 4;
        for(; i < NUM_FRAMES; i++ ){
            if(mmu->memory->frames[i].page_number != -1)
                continue;
            else
                break;            
        }
        //i = indice del frame libero
        mmu->pageTable->pages[page_number].frame_number =  i;
        mmu->pageTable->pages[page_number].flags = Valid;
        //devo settare il frame ad occupato e aggiornare la page number
        mmu->memory->frames[i].page_number = page_number;
        return 0; //OK
    }

    //in memoria NON ho frame liberi (SWAP) SECOND CHANCE
    int choose = 0, i = 0;
    Frame * out;
    while(!choose){
        //SECOND CHANCE ALGORITHM
        out = &(mmu->memory->frames[i]);
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
    return 0; //OK
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
        fwrite(&(frame->info[i]), sizeof(char), 1, mmu->swap_file);
    return;

}
char * MMU_readByte(MMU * mmu, int pos){
    char * info = malloc(sizeof(char));
    LogicAddress l;
    l.addr = pos & 0xFFFFFF;
    PhysicalAddress phy = getPhysicalAddr(mmu, l);
    uint32_t frame_number = (phy.addr >> 8) & 0xFFF; //MSB 12 bit
    Frame * frame = &(mmu->frame_memory_wrapper->frames[frame_number]);
    int off = phy.addr & 0xFF;

    info = frame->info[off];
    frame->flags |= Read_bit;
    return info;
}
