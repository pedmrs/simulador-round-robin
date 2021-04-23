#include "processo.h"

typedef struct
{
    int         head;
    int         tail;
    unsigned    num_processos;  
    Processo**   processos; 
} FilaProcessos;

void inicia_fila(FilaProcessos* fila, unsigned quantidade_processos);
bool fila_cheia(FilaProcessos fila);
bool fila_vazia(FilaProcessos fila);
bool enfileira_processo(Processo* processo_ptr, FilaProcessos* fila);
Processo* desenfileira_processo(FilaProcessos* fila);
Processo* primeiro_da_fila(FilaProcessos fila);