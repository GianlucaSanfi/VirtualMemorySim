#include "mmu.h"
void test_conversione_addr(MMU * mmu, int address){
    LogicAddress addr;
    addr.addr = address; 
    printf("TEST; conversione indirizzo logico: %x, a indirizzo fisico %x \n", addr.addr, getPhysicalAddr(mmu, addr));
}

int main(int argc, char * argv[]) {

    MMU mmu = initMemSystem();
    
    //test_conversione_addr(&mmu, 0xab5F81);
    // page 43871, offset 129 decimale
    
    freeMemSystem(&mmu);
    

    return 0;
}