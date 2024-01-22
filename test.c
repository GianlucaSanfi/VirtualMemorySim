#include "mmu.h"

int main(int argc, char * argv[]) {

    MMU mmu = initMemSystem();
    freeMemSystem(&mmu);

    return 0;
}