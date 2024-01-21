#include "list.h"
#include "globals.h"


void init(List * list) {
    for (int i = 0; i < NUM_FRAMES; i++){
        list->frames[i] = 0;
    }
    list->size = 0;
}

int isEmpty(List * list) {
    if(list->frames)
}

Frame * get(List * list, int pos) {

}

void add(List * list, Frame * frame) {

}

Frame * removeFrame(List * list, int pos) {

}