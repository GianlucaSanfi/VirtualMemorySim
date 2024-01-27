#pragma once
#include "globals.h"
#include "stdio.h"
#include "mmu.h"
typedef struct TLBFrame {
    uint32_t page_number: BIT_FRAME;
    uint32_t frame_number: BIT_FRAME_NUMBER;
}TLBFrame;

typedef struct List {
    TLBFrame * frames[NUM_TLB_FRAMES];
    int size;
} List;

void init(List * list);
int isEmpty(List * list);

//aggiunge in testa alla lista
int add(List * list, TLBFrame * frame);
//rimuove e restituisce la coda della lista
TLBFrame * removeFrame(List * list);
//restituisce il TLBframe in pos pos della lista
TLBFrame * get(List * list, int pos);
