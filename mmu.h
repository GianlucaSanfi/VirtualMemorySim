#pragma once
#include <stdint.h>
#include "globals.h"

typedef uint32_t PhysicalAddress;
typedef uint32_t LogicAddress;



//MMU
typedef struct MMU {

} MMU;

//tabella delle pagine
typedef struct page_table {

} page_table;

void MMU_writeByte(MMU * mmu, int pos, char c);
char * MMU_readByte(MMU * mmu, int pos);

getPhysicalAddr(MMU * mmu, LogicAddress logicAddr);