#pragma once
#include <stdint.h>
#include "globals.h"
#include "list.h"

typedef struct PhysicalAddress{
    uint32_t addr:  20;
} PhysicalAddress;
typedef struct LogicAddress{
    uint32_t addr: 24;
} LogicAddress;

//flags supportati: valid, unswappable, read, write
typedef enum PageFlags{
    Valid = 0x1,
    Unswappable = 0x2,
    Read_bit = 0x4,
    Write_bit = 0x8
} PageFlags;
typedef enum FrameFlags{
    ValidFrame = 0x1,
    Reserved = 0x2
} FrameFlags;

//struttura di un Frame
typedef struct Frame {
    char info[SIZE_PAGE]; //ogni pagina/frame ha 256 char di info
    uint32_t page_number: 16;
    uint32_t frame_number: 12;
    uint32_t flags: 2; 
    //bit ValidFrame = 0 se frame è libero
    //bit Reserved = 1 se frame è del S.O.
} Frame;

//spazio di swap come lista di frame di dimensione massima: NUM_PAGES
/*typedef struct swap_space {
    Frame * swap_frames[NUM_PAGES];
    uint32_t size;
} swap_space;*/

//wrapper della struttura di frames della mem fisica
typedef struct FrameMemoryWrapper {
    Frame frames[NUM_FRAMES];
    uint32_t num_frames;
    struct List * freeFrames;
} FrameMemoryWrapper;

//elemento della tabella delle pagine
typedef struct PageEntry {
    uint32_t frame_number:  BIT_FRAME;
    uint32_t flags:   BIT_PAGE_FLAGS;
} PageEntry;

//tabella delle pagine
typedef struct Page_Table {
    PageEntry pages[NUM_PAGES];
    uint32_t num_pages;
} Page_Table;

// MMU
/* 
Tiene sincronizzata la tabella delle pagine 
    con lo swap_file e la lista di frame in mem fisica.
Se il bit di invalidità della pagina è 1 
    allora la pagina(il frame) si trova in swap space
*/
typedef struct MMU {
    Page_Table * page_table;
    FrameMemoryWrapper * frame_memory_wrapper;
    FILE * swap_file;
} MMU;

//scrive un Byte (char c) data la pos
void MMU_writeByte(MMU * mmu, int pos, char c);
//legge un Byte (char) data la pos
char * MMU_readByte(MMU * mmu, int pos);
//PAGE FAULT HANDLER 
//(pos è il numero di pagina alla quale si è tentato di accedere)
void MMU_exception(MMU * mmu, int pos);

//traduzione indirizzo logico -> fisico
PhysicalAddress getPhysicalAddr(MMU * mmu, LogicAddress logicAddr);

//init delle strutture di gestione memoria MMU
MMU initMemSystem();
//libero le risorse allocate con la init
void freeMemSystem(MMU * mmu);