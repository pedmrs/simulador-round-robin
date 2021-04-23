#include "fila.h"

void inicia_fila(FilaProcessos* fila, unsigned quantidade_processos){
    fila->head = 0 ;
    fila->tail = -1;
    fila->num_processos = 0;
    fila->processos = (Processo **) malloc(quantidade_processos * sizeof(Processo *));
}

bool fila_cheia(FilaProcessos fila){
    if(fila.num_processos == MAX_PROCESSOS){
        return true;
    }
    return false;
}

bool fila_vazia(FilaProcessos fila){
    if(fila.num_processos == 0){
        return true;
    }
    return false;
}

bool enfileira_processo(Processo* processo_ptr, FilaProcessos* fila){
    if(fila_cheia(*fila))
        return false;
    

    fila->tail++;
    fila->num_processos++;

   
    fila->processos[fila->tail] = processo_ptr;
 

    return true;
}

Processo* desenfileira_processo(FilaProcessos* fila){
    if(fila_vazia(*fila))
        return NULL;

    Processo* processo_ptr = fila->processos[fila->head];
    fila->head++;
    fila->num_processos--;

    return processo_ptr;
}

Processo* primeiro_da_fila(FilaProcessos fila){
    return fila.processos[fila.head];
}