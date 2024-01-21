#pragma once
#include <stdint.h>
#include "globals.h"
#include "list.h"


typedef uint32_t PhysicalAddress;
typedef uint32_t LogicAddress;

typedef enum {
    Valid = 0x1,
    Unswappable = 0x2,
    Read_bit = 0x4,
    Write_bit = 0x8
} PageFlags;

//struttura di un Frame
typedef struct Frame {
    uint32_t frame_number;
    char info; // TODO // se frame size == 1 Byte 
} Frame;

//wrapper della struttura di frames della mem fisica
typedef struct FrameMemoryWrapper {
    Frame frames[NUM_FRAMES];
    uint32_t num_frames;
    List * freeFrames;
} FrameMemoryWrapper;

//elemento della tabella delle pagine
typedef struct PageEntry {
    uint32_t frame_number:  BIT_FRAME
    uint32_t flags:         BIT_PAGE_FLAGS
} PageEntry;

//tabella delle pagine
typedef struct page_table {
    PageEntry pages[NUM_PAGES];
    uint32_t num_pages;
} page_table;

// MMU
typedef struct MMU {
    page_table * page_table;
    FrameMemoryWrapper * frame_memory_wrapper;
} MMU;

void MMU_writeByte(MMU * mmu, int pos, char c);
char * MMU_readByte(MMU * mmu, int pos);

getPhysicalAddr(MMU * mmu, LogicAddress logicAddr);