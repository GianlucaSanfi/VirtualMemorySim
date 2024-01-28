#include "mmu.h"
#include "globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

MMU * initMemSystem(){
    printf("avvio del sistema... inizializzazione \n");
//inizializzo le strutture dati
    MMU * mmu = malloc(sizeof(MMU));
    mmu->pageTable = (PageTable *) malloc(sizeof(PageTable));
    mmu->memory = (Memory *) malloc(sizeof(Memory));
    mmu->swap_file = fopen("swap_file.bin", "w+");
    mmu->flags = 0;
    mmu->stats = (Statistics *) malloc(sizeof(Statistics));

//SWAP
    char c = 0;
    fseek(mmu->swap_file, 0, SEEK_SET); //azzero l'indice di scorrimento del file
    for (int i = 0; i < SIZE_VIRTUAL_MEMORY; i++){ //creo il file di 16 MB
        fwrite(&c, sizeof(char), 1, mmu->swap_file);
    }

//MEM
    for(int i = 0; i < NUM_FRAMES; i++){
        mmu->memory->frames[i].page_number = -1;
        memset(mmu->memory->frames[i].info, 0, SIZE_PAGE);
        //azzero il frame  in memoria
    }
    mmu->memory->size = 0;
    
//PAGES
    for (int i = 0; i < NUM_PAGES; i++){
        mmu->pageTable->pages[i].frame_number = 0;
        mmu->pageTable->pages[i].flags = 0;
    }
    mmu->pageTable->size = 0;

    //devo allocare la tabella delle pagine all'inizio della memoria 
    int frames_for_pageTable = (sizeof(PageTable) / SIZE_PAGE);
    // = ((sizeof(PageEntry) * NUM_PAGES + sizeof(int))/ SIZE_PAGE);
    for (int i = 0; i < frames_for_pageTable; i++){
        mmu->pageTable->pages[i].flags |= Unswappable;
        mmu->pageTable->size++;
    }
//STATISTICS
    mmu->stats->TOTAL_PAGE_FAULTS = 0;
    
    printf("avvio del sistema... inizializzazione completata \n");
    return mmu;
}

void freeMemSystem(MMU * mmu){
    printf("chiusura del sistema... ");

    fclose(mmu->swap_file);
    free(mmu->memory);
    free(mmu->pageTable);
    free(mmu->stats);

    free(mmu);

    printf("...chiusura del sistema completata\n");
}

PhysicalAddress * getPhysicalAddr(MMU * mmu, LogicalAddress logicAddr){

    int page_number = (logicAddr.addr >> BIT_FRAME) & 0xFFF; 
    int offset = logicAddr.addr & 0xFFF;

    PhysicalAddress * addrFisico = (PhysicalAddress *) malloc(sizeof(PhysicalAddress));
    
    // SICUREZZA: controllo che l'addr sia nei range giusti
    if((page_number > NUM_PAGES) || (page_number < 0)){
        addrFisico->addr = NULL;
        return addrFisico;
    }
    
    if(!(mmu->pageTable->pages[page_number].flags & Valid)){
        //la pagina richiesta (frame) non è valida
        //PAGE FAULT
        if(MMU_exception(mmu, page_number) != 0){
            addrFisico->addr = NULL;
            return addrFisico;
        }
    }

    int frame_number = mmu->pageTable->pages[page_number].frame_number;
    //printf("frame number : %x \n", frame_number);
    addrFisico->addr = (frame_number << 12 ) | offset;
    return addrFisico;
}

//copia il contenuto del blocco in pos_base dello swap in memoria nel blocco frame_num
int _swap_in(MMU * mmu, int pos_base, int frame_num){

    fseek(mmu->swap_file, pos_base, SEEK_SET);
    for(int i = 0; i < SIZE_PAGE; i++){
        fread(&(mmu->memory->frames[frame_num].info[i]), sizeof(char), 1, mmu->swap_file);
    }
    //NON AZZERA IL CONTENUTO IN SWAP (blocco pos_base)
    return 0;
}

//copia il contenuto del blocco frame_num in memoria in pos_base dello swap
int _swap_out(MMU * mmu, int frame_num, int pos_base){

    fseek(mmu->swap_file, pos_base, SEEK_SET);
    for(int i = 0; i < SIZE_PAGE; i++){
        fwrite(&(mmu->memory->frames[frame_num].info[i]), sizeof(char), 1, mmu->swap_file);
    }
    //NON AZZERA IL CONTENUTO IN MEMORIA (blocco frame_num)
    return 0;
}

//ritorna il frame_number della vittima su cui fare lo swap out
int sca(MMU * mmu, int page_number){ 
    // page_number è quello che mi serve (forse inutile nello scope)
    printf("SECOND CHANCE ALGORITHM: start\n");
    //total number of frames in memory: NUM_FRAMES
    int i = 0; //indice dell'attuale frame vittima
    int control = 0; //trovato l'indice del frame vittima?
    int second_chance_rw = 0, second_chance_w = 0, second_chance_control = 0;
    while(!control){
        if(i >= NUM_FRAMES){
            i=0; //azzero l'indice
            if(((second_chance_rw + second_chance_w) == NUM_FRAMES) && (second_chance_w == 0))
                second_chance_control = 1;
            else if(((second_chance_rw + second_chance_w) == NUM_FRAMES) && (second_chance_w > 0))
                second_chance_control = 2;
            second_chance_rw = second_chance_w = 0;
        }
        
        int num_pag_frame = mmu->memory->frames[i].page_number;
        i++;

        //unswappable
        if(mmu->pageTable->pages[num_pag_frame].flags & Unswappable) {
            if(mmu->flags & VERBOSE) 
                printf("Unswappable \n");
            second_chance_rw++;
            continue;
        }
        //read = 0 & write = 0 => swap out
        if((!(mmu->pageTable->pages[num_pag_frame].flags & Read)) && (!(mmu->pageTable->pages[num_pag_frame].flags & Write))){ 
            printf("SWAP OUT page: %d \n", num_pag_frame);
            control = 1;
            continue;
        }
        //read = 1 & write = 0
        if((mmu->pageTable->pages[num_pag_frame].flags & Read) && (!(mmu->pageTable->pages[num_pag_frame].flags & Write))){
            if(mmu->flags & VERBOSE) 
                printf("Read = true \n");
            mmu->pageTable->pages[num_pag_frame].flags &= ~Read;
            continue;
        }
        //read = 0 & write = 1
        if((!(mmu->pageTable->pages[num_pag_frame].flags & Read)) && (mmu->pageTable->pages[num_pag_frame].flags & Write)){
            if(mmu->flags & VERBOSE) 
                printf("Write = true \n");
            second_chance_w++;
            if(second_chance_control == 2){ // ci sono 0,1
                control = 1;
                printf("SWAP OUT page: %d \n", num_pag_frame);
            }
            mmu->pageTable->pages[num_pag_frame].flags &= ~Write;
            mmu->pageTable->pages[num_pag_frame].flags |= Read;
            continue;
        }
        //read = 1 & write = 1
        if((mmu->pageTable->pages[num_pag_frame].flags & Read) && (mmu->pageTable->pages[num_pag_frame].flags & Write)){
            if(mmu->flags & VERBOSE) 
                printf("Read & Write = true \n");
            second_chance_rw++;
            if(second_chance_control == 1){ // sono tutti 1,1
                control = 1;
                printf("SWAP OUT page: %d \n", num_pag_frame);
            }
            mmu->pageTable->pages[num_pag_frame].flags &= ~Read;
            continue;
        }
    }
    printf("SECOND CHANCE ALGORITHM: end\n");
    return (i-1);
}

int MMU_exception(MMU * mmu, int page_number) {
    //pos è la page_number alla quale si è tentato di accedere     

    mmu->stats->TOTAL_PAGE_FAULTS++;
    printf("PAGE FAULT n*: %d \n", mmu->stats->TOTAL_PAGE_FAULTS);

//READ OPS
    if (mmu->flags & READ){
        if(mmu->pageTable->pages[page_number].flags & Unswappable){
            printf("ATTENZIONE: La locazione rihiesta è riservata al S.O.\n");
            return -1;
        }
        //valid == 0 and swapped = 0 => SEGMENTATION FAULT
        if(!(mmu->pageTable->pages[page_number].flags & Swapped))
            return -1; //KO

    //devo leggere su una pagina che era stata "swapped out"
        if (mmu->pageTable->pages[page_number].flags & Swapped){
            //posizione in swap 
            int swap_idx = page_number * SIZE_PAGE;
            
        //c'è un frame libero in memoria (dubito)
            if(mmu->memory->size < NUM_FRAMES){
                int i = 4;
                for(; i < NUM_FRAMES; i++ ){
                    if(mmu->memory->frames[i].page_number != -1)
                        continue;
                    else
                        break;            
                }
                //i = indice del frame libero
                mmu->pageTable->pages[page_number].frame_number =  i;
                mmu->pageTable->pages[page_number].flags |= Valid;
                mmu->pageTable->pages[page_number].flags &= ~Swapped;
                //devo settare il frame ad occupato e aggiornare la page number
                mmu->memory->frames[i].page_number = page_number;
                mmu->memory->size++;

                _swap_in(mmu, swap_idx, i);
                return 0;
            }
        //non c'è un frame libero in memoria => swap out, swap in
            int out_frame_num = sca(mmu, page_number); // trovo il frame vittima in memoria
            int out_page_num = mmu->memory->frames[out_frame_num].page_number;
            _swap_out(mmu, out_frame_num, (out_page_num * SIZE_PAGE));
            mmu->pageTable->pages[out_page_num].flags |= Swapped;
            mmu->pageTable->pages[out_page_num].flags &= ~Valid;

            _swap_in(mmu, swap_idx, out_frame_num);
            mmu->pageTable->pages[page_number].flags |= Valid;
            mmu->pageTable->pages[page_number].flags &= ~Swapped;
            
            mmu->memory->frames[out_frame_num].page_number = page_number;
            mmu->pageTable->pages[page_number].frame_number = out_frame_num;

            return 0;
        }
    }
//WRITE OPS
    if (mmu->flags & WRITE){
        //controllo se si tratta di un blocco di memoria riservata (Unswappable)
        if(mmu->pageTable->pages[page_number].flags & Unswappable){
            printf("ERRORE: si sta provando a scrivere su una locazione riservata, comando abortito \n");
            return -1;
        }

    //devo scrivere una pagina (frame) NUOVA e ho spazio in memoria
        if((!(mmu->pageTable->pages[page_number].flags & Swapped)) && (mmu->memory->size < NUM_FRAMES)){
            int i = 4;
            for(; i < NUM_FRAMES; i++ ){
                if(mmu->memory->frames[i].page_number != -1)
                    continue;
                else
                    break;            
            }
            //i = indice del frame libero
            mmu->pageTable->pages[page_number].frame_number =  i;
            mmu->pageTable->pages[page_number].flags |= Valid;
            //devo settare il frame ad occupato e aggiornare la page number
            mmu->memory->frames[i].page_number = page_number;
            mmu->memory->size++;
            
            return 0; //OK

        } else if (mmu->pageTable->pages[page_number].flags & Swapped){
    // devo scrivere una pagina che era stata "swapped out" 
            //posizione in swap 
            int swap_idx = page_number * SIZE_PAGE;
            
        //c'è un frame libero in memoria (dubito)
            if(mmu->memory->size < NUM_FRAMES){
                int i = 4;
                for(; i < NUM_FRAMES; i++ ){
                    if(mmu->memory->frames[i].page_number != -1)
                        continue;
                    else
                        break;            
                }
                //i = indice del frame libero
                mmu->pageTable->pages[page_number].frame_number =  i;
                mmu->pageTable->pages[page_number].flags |= Valid;
                mmu->pageTable->pages[page_number].flags &= ~Swapped;
                //devo settare il frame ad occupato e aggiornare la page number
                mmu->memory->frames[i].page_number = page_number;
                mmu->memory->size++;

                _swap_in(mmu, swap_idx, i);
                return 0;
            }
        //non c'è un frame libero in memoria => swap out, swap in
            int out_frame_num = sca(mmu, page_number); // trovo il frame vittima in memoria
            int out_page_num = mmu->memory->frames[out_frame_num].page_number;
            _swap_out(mmu, out_frame_num, (out_page_num * SIZE_PAGE));
            mmu->pageTable->pages[out_page_num].flags |= Swapped;
            mmu->pageTable->pages[out_page_num].flags &= ~Valid;

            _swap_in(mmu, swap_idx, out_frame_num);
            mmu->pageTable->pages[page_number].flags |= Valid;
            mmu->pageTable->pages[page_number].flags &= ~Swapped;
            
            mmu->memory->frames[out_frame_num].page_number = page_number;
            mmu->pageTable->pages[page_number].frame_number = out_frame_num;

            return 0;
        }
    }   

    return 0; //OK
}


void MMU_writeByte(MMU * mmu, int pos, char c) {

    mmu->flags |= WRITE;

    LogicalAddress logical;
    logical.addr = pos & 0xFFFFFF; //24 bit

    PhysicalAddress * physical = getPhysicalAddr(mmu, logical);
    if(physical->addr == NULL){
        printf("ERRORE: address fisico = NULL 'Possibile Segmentation Fault' \n");
        free(physical);
        return;
    }

    int frame_number = (physical->addr >> BIT_FRAME) & 0xFF; //MSB 8 bit
    int off = physical->addr & 0xFFF;

    mmu->memory->frames[frame_number].info[off] = c;
    int pag = mmu->memory->frames[frame_number].page_number;
    mmu->pageTable->pages[pag].flags |= Write;
    
    free(physical);
    mmu->flags &= ~WRITE;
    return;
}

char * MMU_readByte(MMU * mmu, int pos){
    
    mmu->flags |= READ;

    LogicalAddress logical;
    logical.addr = pos & 0xFFFFFF;
    PhysicalAddress * physical = getPhysicalAddr(mmu, logical);
    if(physical->addr == NULL){
        printf("ERRORE: address fisico = NULL 'Possibile Segmentation Fault' \n");
        free(physical);
        return NULL;
    }

    char * info = malloc(sizeof(char));
    int frame_number = (physical->addr >> BIT_FRAME) & 0xFF; //MSB 8 bit
    int off = physical->addr & 0xFFF;

    info = &(mmu->memory->frames[frame_number].info[off]);
    int pag = mmu->memory->frames[frame_number].page_number;
    mmu->pageTable->pages[pag].flags |= Read;
    
    free(physical);
    mmu->flags &= ~READ;
    return info;
}
