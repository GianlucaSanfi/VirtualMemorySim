#pragma once
#include <stdint.h>
#include <stdio.h>
#include "globals.h"
#include "list.h"

typedef struct PhysicalAddress{
    uint32_t addr:  20;
} PhysicalAddress;

typedef struct LogicalAddress{
    uint32_t addr: 24;
} LogicalAddress;

//flags supportati: valid, unswappable, read, write, swapped
typedef enum PageFlags{
    Valid = 0x1,
    Unswappable = 0x2,
    Read = 0x4,
    Write = 0x8,
    Swapped = 0x10
} PageFlags;

//struttura di un Frame
typedef struct Frame {
    char info[SIZE_PAGE];
    uint32_t page_number: BIT_FRAME;
} Frame;

typedef struct TLB {
    List * tlbFrames;
} TLB;

typedef struct Memory {
    Frame frames[NUM_FRAMES];
    int size;
} Memory;

//elemento della tabella delle pagine
typedef struct PageEntry {
    uint32_t frame_number:  BIT_FRAME_NUMBER;
    uint32_t flags:   BIT_PAGE_FLAGS;
} PageEntry;

//tabella delle pagine
typedef struct PageTable {
    PageEntry pages[NUM_PAGES];
    int size;
} PageTable;

typedef struct Statistics {
    int TOTAL_PAGE_FAULTS;
    int TOTAL_SWAP_IN;
    int TOTAL_SWAP_OUT;
} Statistics;

typedef enum MMUFLAGS {
    READ = 0x1,
    WRITE = 0x2,
    VERBOSE = 0x4
} MMUFLAGS;

// MMU
typedef struct MMU {
    PageTable * pageTable;
    Memory * memory;
    FILE * swap_file;
    uint8_t flags: 3;
    TLB * tlb;
    Statistics * stats;
} MMU;

//scrive un Byte (char c) data la pos
void MMU_writeByte(MMU * mmu, int pos, char c);

//legge un Byte (char) data la pos
char * MMU_readByte(MMU * mmu, int pos);

//PAGE FAULT HANDLER 
//(pos è il numero di pagina alla quale si è tentato di accedere)
//ritorna 0 se andato a buon fine e -1 se errore
int MMU_exception(MMU * mmu, int pos);

//traduzione indirizzo logico -> fisico
PhysicalAddress * getPhysicalAddr(MMU * mmu, LogicalAddress logicAddr);

//controllo del TLB in address translation (prima di accedere la page_table)
//se non trova il page_number cercato ritorna -1, 
//  altrimenti ritorna il frame_number corrispondente e aggiorna il TLB
int checkTLB(MMU * mmu, int page_number);
//aggiornamento esplicito del TLB in caso di lettura della page_table
void explicitUpdateTLB(MMU * mmu, int page_num, int frame_num);
//invalida il record obsoleto nel TLB a seguito di swap out (se presente in TLB)
void invalidateRecordTLB(MMU * mmu, int page_num, int frame_num);

//init delle strutture di gestione memoria MMU
MMU * initMemSystem();

//libero le risorse allocate con la init
void freeMemSystem(MMU * mmu);