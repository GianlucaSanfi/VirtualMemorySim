#pragma once
#include "globals.h"
#include "stdio.h"
#include "mmu.h"
struct Frame;
typedef struct List {
    struct Frame * frames[NUM_FRAMES];
    int size;
} List;

void init(List * list);
int isEmpty(List * list);

struct Frame * get(List * list, int pos);
int add(List * list, struct Frame * frame);
struct Frame * removeFrame(List * list, int pos);

