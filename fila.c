#include "fila.h"

void inicia_fila(FilaProcessos* fila){
    fila->head = 0 ;
    fila->tail = -1;
    fila->num_processos = 0;
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

    printf("Processo %d entrando na posição %d\n", processo_ptr->pid, fila->tail);
    fila->processos[fila->tail] = processo_ptr;
    printf("fila->processos[fila->tail] = %d\n", fila->processos[fila->tail]->pid);

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