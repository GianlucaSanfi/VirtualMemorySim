#include "mmu.h"
#include <stdlib.h>

/* void test_conversione_addr(MMU * mmu, int address){
    LogicAddress addr;
    addr.addr = address; 
    printf("TEST; conversione indirizzo logico: %x, a indirizzo fisico %x \n", addr.addr, getPhysicalAddr(mmu, addr));
} */

int main(int argc, char * argv[]) {

    MMU mmu = initMemSystem();
    /* //test_conversione_addr(&mmu, 0xab5F81);
    // page 43871, offset 129 decimale

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
    
    freeMemSystem(&mmu);
    return 0;
}