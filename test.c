#include "mmu.h"
#include "globals.h"
#include <stdlib.h>

void printPageTable(MMU * mmu){

}
void printMemory(MMU * mmu){

}

int main(int argc, char * argv[]) {

    int tot_accessi = 1<<12;
    MMU * mmu = initMemSystem();
    if(argc >1){ //verbose
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
    LogicalAddress addr;
    addr.addr = 0xab5F81; 
    
    PhysicalAddress * res = getPhysicalAddr(mmu, addr);
    printf("TEST, (NO FLAGS) conversione indirizzo logico: %x, in fisico: %x \n", addr.addr, res->addr);
    free(res);
    mmu->flags |= READ;
    res = getPhysicalAddr(mmu, addr);
    printf("TEST, (READING)conversione indirizzo logico: %x, in fisico: %x \n", addr.addr, res->addr);
    mmu->flags &= ~READ;
    free(res);
    mmu->flags |= WRITE;
    res = getPhysicalAddr(mmu, addr);
    printf("TEST, (WRITING)conversione indirizzo logico: %x, in fisico: %x \n", addr.addr, res->addr);
    mmu->flags &= ~WRITE;
    free(res);
//ACCESSO SEQUENZIALE ------------------------------------
    printf("ACCESSO SEQUENZIALE ------------------------------------\n");
    /*

    LogicAddress logici[1<<10];
    for(int i = 0; i < (1<<10); i++){
        logici[i].addr = i & 0xFFFFFF;
        printf("frame in tab pagine: %d \n", mmu.page_table->pages[i].frame_number);
    }//ho creato 2^10 indirizzi logici

    //leggo tutti gli indirizzi logici (che sono vuoti)
    for (int i = 0; i < (1<<10); i++){
        char * c = MMU_readByte(&mmu, logici[i].addr);
        printf("letto il char: %c \n", *c);
        free(c);
    } 
    //scrivo agli indirizzi logici
    for (int i = 0; i < (1<<10); i++){
        //MMU_writeByte
    }  */
    printf("STATISTICHE (sequenziale):\n\tNUMERO DI ACCESSI: %d\n\tTOTALE PAGE FAULT: %d\n\tSWAP_OUT: %d\n\tSWAP_IN: %d\n", tot_accessi, mmu->stats->TOTAL_PAGE_FAULTS, mmu->stats->TOTAL_SWAP_OUT, mmu->stats->TOTAL_SWAP_IN);
    freeMemSystem(mmu);
//ACCESSO RANDOMICO ------------------------------------
    mmu = initMemSystem();
    if(argc >1){ //verbose
        printf("|==================|\n");
        printf("|  VERBOSE = true  |\n");
        printf("|==================|\n");
        mmu->flags |= VERBOSE;
    } else {
        printf("|===================|\n");
        printf("|  VERBOSE = false  |\n");
        printf("|===================|\n");
    }
    printf("ACCESSO RANDOMICO ------------------------------------\n");


    printf("STATISTICHE (randomico):\n\tNUMERO DI ACCESSI: %d\n\tTOTALE PAGE FAULT: %d\n\tSWAP_OUT: %d\n\tSWAP_IN: %d\n", tot_accessi, mmu->stats->TOTAL_PAGE_FAULTS, mmu->stats->TOTAL_SWAP_OUT, mmu->stats->TOTAL_SWAP_IN);
    freeMemSystem(mmu);
    return 0;
}