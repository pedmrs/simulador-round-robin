#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "fila.h"

Processo *lista_processos;
FilaProcessos  fila_baixa_prioridade, fila_alta_prioridade, fila_io_disco, fila_io_fita, fila_io_impressora;

unsigned tempo_corrente = 0; // tempot total de execução da simulação

char tipo_io[] = {'D', 'F', 'I'}; // Disco, Fita, Impressora


void criar_processos(int quantidade_processos)
{
    if (quantidade_processos > MAX_PROCESSOS)
    {
        printf("Número de processos maior que o limite estabelecido\n");
        exit(-1);
    }

    lista_processos = (Processo *)malloc(sizeof(Processo) * quantidade_processos);

    srand(time(NULL));

    // inicializa os processos
    for (int i = 0; i < quantidade_processos; i++)
    {
        lista_processos[i].pid = i + 1;

        // Determina de maneira aleatória o instante de início e duração total de cada processo
        lista_processos[i].inicio = rand() % MAX_INICIO;
        lista_processos[i].duracao = (rand() % MAX_DURACAO) + 1;
        lista_processos[i].tempo_executado = 0;

        lista_processos[i].prioridade = 1; // define prioridade alta para todos os novos processos

        if (rand() % 2 == 0)
        { // atribui aleatoriamente se processo é IO ou não
            lista_processos[i].io.tipo = tipo_io[rand() % 3];
            lista_processos[i].io.inicio = rand() % lista_processos[i].duracao;
            
            switch (lista_processos[i].io.tipo)
            {
            case 'D':
                lista_processos[i].io.duracao =  DURACAO_IO_DISCO;
                break;
            
            case 'F':
                lista_processos[i].io.duracao = DURACAO_IO_FITA;
                break;
            
            case 'I':
                lista_processos[i].io.duracao = DURACAO_IO_IMPRESSORA;
                break;
            
            default:
                break;
            } 
        }  

        else
        {
            lista_processos[i].io.tipo = '\0';
            lista_processos[i].io.inicio = 0;
            lista_processos[i].io.duracao = 0;
        }



        //     Prioridade dos processos muda apenas quando ele retorna da operação de IO
        //     if (lista_processos[i].io.tipo == 'F' || lista_processos[i].io.tipo == 'I')
        //     {
        //         lista_processos[i].prioridade++; // aumenta a prioridade
        //     }
    }
}

Processo* seleciona_processo_para_execucao()
{
    if(!fila_vazia(fila_alta_prioridade))
    {
        return  desenfileira_processo(&fila_alta_prioridade);
    } 
    
    if(!fila_vazia(fila_baixa_prioridade))
    {
        return desenfileira_processo(&fila_baixa_prioridade);
    }

    return NULL;
}

void comeca_io_de_processo(Processo* processo)
{
    switch (processo->io.tipo)
    {
    case 'D':
        enfileira_processo(processo, &fila_io_disco);
        break;

    case 'F':
        enfileira_processo(processo, &fila_io_fita);
        break;

    case 'I':
        enfileira_processo(processo, &fila_io_impressora);
        break;

    default:
        break;
    }
}

void termina_io_de_processo(Processo* processo)
{   
    Processo* primeiro_io;
    switch(processo->io.tipo){
        case 'D':
            primeiro_io = primeiro_da_fila(fila_io_disco);
            if(processo->pid == primeiro_io->pid){
                if(primeiro_io->io.duracao == 0){
                    desenfileira_processo(&fila_io_disco);
                    enfileira_processo(processo, &fila_baixa_prioridade);
                }
            }
            break;
        
        case 'F':
            primeiro_io = primeiro_da_fila(fila_io_fita);
            if(processo->pid == primeiro_io->pid){
                if(primeiro_io->io.duracao == 0){
                    desenfileira_processo(&fila_io_fita);
                    enfileira_processo(processo, &fila_alta_prioridade);
                }
            }
            break;
        
        case 'I':
            primeiro_io = primeiro_da_fila(fila_io_impressora);
            if(processo->pid == primeiro_io->pid){
                if(primeiro_io->io.duracao == 0){
                    desenfileira_processo(&fila_io_impressora);
                    enfileira_processo(processo, &fila_alta_prioridade);
                }
            }
            break;
        
        default:
            break;
    }
}

void decrementa_duracao_io(FilaProcessos fila_io){
    for(int i = 0; i < fila_io.num_processos; i++){
        if(fila_io.processos[i]->io.duracao > 0){
            fila_io.processos[i]->io.duracao--;
        }
    }
}

void round_robin(unsigned quantum, unsigned quantidade_processos)
{
    int i = 0;
    Processo* processo_executando = NULL;
    unsigned  tempo_execucao_corrente = 0;

    inicia_fila(&fila_alta_prioridade, quantidade_processos);
    inicia_fila(&fila_baixa_prioridade, quantidade_processos);
    inicia_fila(&fila_io_disco, quantidade_processos);
    inicia_fila(&fila_io_impressora, quantidade_processos);
    inicia_fila(&fila_io_fita, quantidade_processos);


    // fila_alta_prioridade = (Processo *)malloc(sizeof(Processo) * quantidade_processos);
    // fila_baixa_prioridade = (Processo *)malloc(sizeof(Processo) * quantidade_processos);
    // fila_io = (Processo *)malloc(sizeof(Processo) * quantidade_processos);

    while (1)
    {      
        printf("Instante t = %d", tempo_corrente);
        
        // No começo de cada "ciclo" conferimos se há um processo nas filas de pronto que pode ser executado
        if(!processo_executando)
        {
            processo_executando = seleciona_processo_para_execucao();
        }

        // Caso o processo em execução já tenha esgotado seu time slice, seleciona um novo processo para execução
        // e o processo preemptado vai para a fila de baixa prioridade.
        else if(tempo_execucao_corrente >= quantum)
        {   
            enfileira_processo(processo_executando, &fila_baixa_prioridade);
            processo_executando = seleciona_processo_para_execucao();
        } 
        else
        // Caso nenhum processo novo seja colocado em execução o processo corrente continua executando 
        {   
            // Incrementa o quanto do processo já executou
            processo_executando->tempo_executado++;
            
            // Caso o processo já tenha executado toda sua duração, ele é movido para a saída e o próximo processo é selecionado.
            if(processo_executando->tempo_executado == processo_executando->duracao)
            {
                processo_executando = seleciona_processo_para_execucao();
            }

            // Testa se o processo atual executa I/O
            if(processo_executando->io.tipo != '\0')
            {   
                // Testa se é o instante no qual a operação de I/O é chamada
                if(processo_executando->tempo_executado == processo_executando->io.inicio)
                {
                    comeca_io_de_processo(processo_executando);
                    processo_executando = seleciona_processo_para_execucao();
                }
            }
            
            
            tempo_execucao_corrente++;
        }

        // Adiciona cada processo em sua devida fila
        while(i < quantidade_processos)
        {   
            Processo processo_atual = lista_processos[i];
            

            // Todos os processos são iniciados com alta prioridade
            if(processo_atual.inicio == tempo_corrente)
            {
                enfileira_processo(&processo_atual, &fila_alta_prioridade);
            } 
            
            if(processo_atual.io.tipo)
            {
                termina_io_de_processo(&processo_atual);
            }
            
            // TO DO: 
            //      * Colocar evento em espera quando pede um evento de IO
            //      * Mover processos na fila de IO para fila de prontos 
            //      * Finalizar execução dos processos

            i++;
        
        }
        
        i = 0;

        // Ao final de cada ciclo, reduz a duração da operação de IO para cada processo bloqueado.
        decrementa_duracao_io(fila_io_disco);
        decrementa_duracao_io(fila_io_fita);
        decrementa_duracao_io(fila_io_impressora);
        tempo_corrente++;
    }
    //     while (i < quantidade_processos)
    //     {
    //         if (lista_processos[i].prioridade > 1)
    //         {
    //             fila_alta_prioridade[i] = lista_processos[i];
    //             if (lista_processos[i].io.tipo != '\0')
    //             {
    //                 fila_io[i] = lista_processos[i];
    //             }
    //         }
    //         else
    //         {
    //             fila_baixa_prioridade[i] = lista_processos[i];
    //         }

    //         if (lista_processos[i].inicio <= tempo_corrente)
    //         {
    //             if (lista_processos[i].duracao_restante > 0)
    //             {
    //                 if (lista_processos[i].duracao_restante <= quantum)
    //                 { // processo sem preempção
    //                     lista_processos[i].duracao_restante = 0;
    //                     tempo_corrente += lista_processos[i].duracao_restante;
    //                 }
    //                 else
    //                 {
    //                     lista_processos[i].duracao_restante -= quantum;
    //                     if (lista_processos[i].io.tipo == 'D')
    //                     {
    //                         fila_baixa_prioridade[i] = lista_processos[i];
    //                     }
    //                     else
    //                     {
    //                         fila_alta_prioridade[i] = lista_processos[i];
    //                     }

    //                     tempo_corrente += quantum;
    //                 }
    //             }
    //         }

    //         i++;
    //     }
    //     i = 0;
        
    
}
void imprime_tabela_processos(int qtd)
{
    printf("--------------------------------------------------------------------------------\n");
    printf("| PID | Prioridade | Tempo de Inicio | Duraçao |  IO  | Inicio IO | Duracao IO |\n");
    printf("--------------------------------------------------------------------------------\n");
    int i = 0;
    for (i = 0; i < qtd; i++)
    {
        Processo _p = lista_processos[i];
        printf("|  %d  |      %d     |        %d        |    %d    |   %c  |     %d     |    %d     |\n",
               _p.pid, _p.prioridade, _p.inicio, _p.duracao, _p.io.tipo, _p.io.inicio, _p.io.duracao);
        printf("--------------------------------------------------------------------------------\n");
    }
}

void imprime_fila_processos(FilaProcessos fila){
    int i;
    for(i = fila.head; i < fila.tail + 1; i++){
        
        printf("Processo: %d", fila.processos[i]->pid);
        if(i == fila.head)
            printf("\t---> Primeiro da fila");
        if(i == fila.tail)
            printf("\t---> Último da fila");
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    criar_processos(5);
    imprime_tabela_processos(5);

    FilaProcessos fila_de_prontos;
    inicia_fila(&fila_de_prontos, 5);
    int i;
    for(i = 0; i < 5; i++){
        enfileira_processo(&lista_processos[i], &fila_de_prontos);  
    }

    imprime_fila_processos(fila_de_prontos);

    desenfileira_processo(&fila_de_prontos);
    printf("\n");
    desenfileira_processo(&fila_de_prontos);

    
    imprime_fila_processos(fila_de_prontos);
    return 0;
}