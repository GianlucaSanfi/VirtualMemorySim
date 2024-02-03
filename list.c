#include "list.h"
#include "globals.h"

// max elements = NUM_TLB_FRAMES
//inizializza a 0 la lista passata
List * initTLB() {
    printf("-----INIT TLB-----:");
    List * list = (List *) malloc(sizeof(List));
    list->size = 0;
    list->head = 0;
    list->tail = 0;
    printf("  -----completato-----\n");
    return list;
}

void freeTLB(List * list){
    free(list);
    printf("-----FREE TLB-----: completato-----\n");
}

//ritorna 0 se lista non vuota
int isEmpty(List * list) {
    if(list->size == 0)
        return 1;
    return 0;
}

//ritorna il Frame in pos oppure 0 se errore
/* TLBFrame * get(List * list, int pos) {
    //check bounds
    if((pos < 0) || (pos >= list->size)) return NULL;
    return NULL;
} */

void add(List * list, TLBFrame frame) {
    //controllo se è presente
    int found = 0;
    TLBFrame * head = list->head;

    if(list->size !=0){
        while(head){
            if(head->page_number == frame.page_number){
                head->frame_number = frame.frame_number;
                //aggiorno il frame number in caso fosse obsoleto quello che ho
                found = 1;
                break;
            }
            head = head->next;
        }
    }
    
    if(!found){
        //non lo avevo in tlb => devo aggiungerlo in testa
        
        if(list->size == NUM_TLB_FRAMES){
            //se sono pieno devo eliminare il meno recente LRU
            removeFrame(list);
        }
        TLBFrame * newFrame = (TLBFrame *) malloc(sizeof(TLBFrame));
        newFrame->prev = NULL;
        newFrame->next = list->head;
        if(list->head)
            list->head->prev = newFrame;
        list->head = newFrame;
        list->size++;

    } else {
        //se lo ho gia devo solo aggiornare la sua posizione
        if(list->size == 1)
            return; //c'è solo lui
        if((head->prev) && (!(head->next))){ //ho predecessore ma sono ultimo
            list->tail = head->prev; //aggiorno puntatore tail
            head->prev->next = NULL; //aggiorno l' ex penultimo => ultimo
            list->head->prev = head; //aggiorno l' ex head => secondo
            
            head->next = list->head; //aggiorno il mio next
            list->head = head; //aggiorno puntatore head
            head->prev = NULL; //aggiorno il mio prev => sono primo

        } else if((head->next) && (!(head->prev))){ //ho successore ma sono primo
            return; //perfetto, sono il primo
        }else{
            //sto in mezzo, devo scollegare prev e next, 
            //      e poi ricollegarli e aggiornare list->head e l'ex primo
            head->prev->next = head->next;
            head->next->prev = head->prev;

            head->prev = NULL;
            head->next = list->head;
            list->head->prev = head;
            list->head = head;
        }
    }
}

void removeF(List * list, TLBFrame frame){
    if(list->size == 0)
        return;
    //controllo se è presente
    TLBFrame * head = list->head;
    int found = 0;
    while(head){
        if(head->page_number == frame.page_number){
            found = 1;
            break;
        }
        head = head->next;
    }
    if(found){
        if(list->size == 1){
            list->head = NULL;
            list->tail = NULL;
        } else if((head->prev) && (!(head->next))){ //ho predecessore ma sono ultimo
            list->tail = head->prev;
            head->prev->next = NULL;
        } else if((head->next) && (!(head->prev))){ //ho successore ma sono primo
            list->head = head->next;
            head->next->prev = NULL;
        }else{
            head->prev->next = head->next;
            head->next->prev = head->prev;
        }
        free(head);
        list->size--;
    }
}

void removeFrame(List * list) {
    if(list->size == 0) 
        return;
    TLBFrame * victim = list->tail;
    if(list->size > 1){
        victim->prev->next = NULL;
        list->tail = victim->prev;
    } else {
        list->tail = NULL;
        list->head = NULL;
    }
    free(victim);
    list->size--;
}