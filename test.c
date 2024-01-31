#include "mmu.h"
#include "globals.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>

void printPageTable(MMU * mmu){
    for(int i = 0; i < NUM_PAGES; i++){
        printf("page: %d ; frame: %d ; flags: %d\n", i, mmu->pageTable->pages[i].frame_number, mmu->pageTable->pages[i].flags);
    }
}
void printMemory(MMU * mmu){
    for(int i = 0; i < NUM_FRAMES; i++){
        int pg = mmu->memory->frames[i].page_number;
        printf("frame: %d ;page: %d ;flags %d\n", i, pg, mmu->pageTable->pages[pg].flags);
    }
}

int main(int argc, char * argv[]) {
    int seq_or_rand = 0; //seq
    if(argc > 1){
        if(argv[1][0] == 'S')
            seq_or_rand = 0;
        else if(argv[1][0] == 'R')
            seq_or_rand = 1;
        else{
            printf("uso del programma: ./test [S]|[R] {VERBOSE}|{}\n");
            return 0;
        }
    } else{
        printf("uso del programma: ./test [S]|[R] {VERBOSE}|{}\n");
        return 0;
    }
    MMU * mmu = initMemSystem();
    
    if(argc > 2){ //verbose
        printf("|==================|\n");
        printf("|  VERBOSE = true  |\n");
        printf("|==================|\n");
        mmu->flags |= VERBOSE;
    } else {
        printf("|===================|\n");
        printf("|  VERBOSE = false  |\n");
        printf("|===================|\n");
    }
//TEST (di unità) CONVERSIONE INDIRIZZI
    /*LogicalAddress addr;
    addr.addr = 0xab5F851; 
    
    PhysicalAddress * res = getPhysicalAddr(mmu, addr);
    printf("TEST, (NO FLAGS) conversione indirizzo logico: %06x, in fisico: %06x \n", addr.addr, res->addr);
    free(res);
    mmu->flags |= READ;
    res = getPhysicalAddr(mmu, addr);
    printf("TEST, (READING)conversione indirizzo logico: %06x, in fisico: %06x \n", addr.addr, res->addr);
    mmu->flags &= ~READ;
    free(res);
    mmu->flags |= WRITE;
    res = getPhysicalAddr(mmu, addr);
    printf("TEST, (WRITING)conversione indirizzo logico: %06x, in fisico: %06x \n", addr.addr, res->addr);
    mmu->flags &= ~WRITE;
    free(res);*/

    int tot_accessi = 0;
    const int MAX_SCRITTURE = 1<<8; //tutte le pagine
    FILE * testo_fd = fopen("promessisposi.txt", "r");
    assert(testo_fd && "ERRORE LETTURA FILE PROMESSI SPOSI :( \n");
    char testo[4096];
    int size = 0;
    while(!feof(testo_fd)){
        fscanf(testo_fd,"%c", &testo[size]);
        size++;
    }
    testo[size] = '\0';
    fclose(testo_fd);

    if(seq_or_rand == 0){
//ACCESSO SEQUENZIALE ------------------------------------
        printf("ACCESSO SEQUENZIALE ------------------------------------\n");
        //printf("%s hh, size: %d \n", testo, size);

        for(int i = 0; i < MAX_SCRITTURE*6; i++){
            int address = (i & 0xFFFFFF) * 1000; // * 1000 per evitare una miriade di accessi seq non legittimi
            tot_accessi++;
    
            MMU_writeByte(mmu, address, testo[i%size]);
            printf("ho provato a scrivere il byte: %c \n", testo[i%size]);
           
        }
        for(int i = 0; i < MAX_SCRITTURE*5; i++){
            int address = (i & 0xFFFFFF) * 1000; // * 1000 per evitare una miriade di accessi seq non legittimi
            tot_accessi++;
            char * c = MMU_readByte(mmu, address);
            if(c != NULL)
                printf("ho letto il byte: %c \n", *c);
            else
                printf("lettura fallita \n");
        
        }      
        printf("\n----------\nSTATISTICHE (sequenziale):\n\tNUMERO DI ACCESSI: %d\n\tTOTALE PAGE FAULT: %d\n\tSWAP_OUT: %d\n\tSWAP_IN: %d\n", tot_accessi, mmu->stats->TOTAL_PAGE_FAULTS, mmu->stats->TOTAL_SWAP_OUT, mmu->stats->TOTAL_SWAP_IN);
    } else {
//ACCESSO RANDOMICO ------------------------------------
    
        printf("ACCESSO RANDOMICO ------------------------------------\n");
        srand(time(NULL));

        for(int i = 0; i < MAX_SCRITTURE*6; i++){
            int address = rand() % (0x1000000);
            tot_accessi++; 
            
            MMU_writeByte(mmu, address, testo[i%size]);
            printf("ho provato a scrivere il byte: %c \n", testo[i%size]);
            
        }
        for(int i = 0; i < MAX_SCRITTURE*4; i++){
            int address = rand() % (0x1000000);
            tot_accessi++; 
            char * c = MMU_readByte(mmu, address);
            if(c != NULL)
                printf("ho letto il byte: %c \n", *c);
            else
                printf("lettura fallita \n");
        }
        printf("\n----------\nSTATISTICHE (randomico):\n\tNUMERO DI ACCESSI: %d\n\tTOTALE PAGE FAULT: %d\n\tSWAP_OUT: %d\n\tSWAP_IN: %d\n", tot_accessi, mmu->stats->TOTAL_PAGE_FAULTS, mmu->stats->TOTAL_SWAP_OUT, mmu->stats->TOTAL_SWAP_IN);
    }
    freeMemSystem(mmu);
    return 0;
}