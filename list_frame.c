#include "list.h"
#include "globals.h"

//inizializza a 0 la lista passata
void init(List * list) {
    for (int i = 0; i < NUM_FRAMES; i++){
        list->frames[i] = 0;
    }
    list->size = 0;
}

//ritorna 0 se lista non vuota
int isEmpty(List * list) {
    if(list->size == 0)
        return 1;
    return 0;
}

//ritorna il Frame in pos oppure 0 se errore
struct Frame * get(List * list, int pos) {
    //check bounds
    if((pos < 0) || (pos >= list->size)) return 0;
    return list->frames[pos];
}

//ritorna 0 se success, 1 se errore
int add(List * list, struct Frame * frame) {
    //check bounds
    if( list->size == NUM_FRAMES){
        printf("limit of Frames reached %d \n", NUM_FRAMES);
        return 1;
    }
    list->frames[list->size] = frame;
    list->size++;
    return 0;
}

//ritorna il Frame eliminato oppure 0 se errore
struct Frame * removeFrame(List * list, int pos) {
    //check bounds
    if((pos < 0) || (pos >= list->size)) return 0;
    Frame * aux  = list->frames[pos];
    for (int i = pos; i < (list->size -1); i++){
        list->frames[i] = list->frames[i+1];
    }
    list->size--;
    return aux;
}