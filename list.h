#pragma once
#include "globals.h"
#include "stdio.h"

#include <stdint.h>

typedef struct TLBFrame {
    struct TLBFrame * next;
    struct TLBFrame * prev;
    uint32_t page_number: BIT_FRAME;
    uint32_t frame_number: BIT_FRAME_NUMBER;
}TLBFrame;

typedef struct List {
    TLBFrame * head;
    TLBFrame * tail;
    int size;
} List;

List * initTLB();
void freeTLB(List * list);

int isEmpty(List * list);

//aggiunge in testa alla lista se non presente, 
//  se presente aggiorna l'ordine dei frame
void add(List * list, TLBFrame frame);

//rimuove il frame dalla lista se presente
void removeF(List * list, TLBFrame frame);

//rimuove il tail della lista
void removeFrame(List * list);

//restituisce il TLBframe in pos pos della lista
//TLBFrame * get(List * list, int pos);
