#pragma once
#include "globals.h"
#include "stdio.h"
#include "mmu.h"

typedef struct List {
    Frame frames[NUM_FRAMES];
    int size;
} List;

void init(List * list);
int isEmpty(List * list);

Frame * get(List * list, int pos);
void add(List * list, Frame * frame);
Frame * removeFrame(List * list, int pos);

