#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "fila.h"

Processo *lista_processos;
FilaProcessos fila_baixa_prioridade, fila_alta_prioridade, fila_io_disco, fila_io_fita, fila_io_impressora, fila_concluidos;

unsigned tempo_corrente = 0; // tempo total de execução da simulação
unsigned qtd_processos_finalizados = 0; // quantidade de processos finalizados

char tipo_io[] = {'D', 'F', 'I'}; // Disco, Fita, Impressora

void imprime_fila_processos(FilaProcessos fila)
{
    int i;
    for (i = fila.head; i < fila.tail + 1; i++)
    {

        printf("Processo: %d", fila.processos[i]->pid);
        if (i == fila.head)
            printf("\t---> Primeiro da fila");
        if (i == fila.tail)
            printf("\t---> Último da fila");
        printf("\n");
    }
}

void criar_processos(int quantidade_processos)
{
    if (quantidade_processos > MAX_PROCESSOS)
    {
        printf("Número de processos maior que o limite estabelecido\n");
        exit(-1);
    }

    lista_processos = (Processo *)malloc(sizeof(Processo) * quantidade_processos);
    fila_baixa_prioridade = *(FilaProcessos *)malloc(sizeof(FilaProcessos) * quantidade_processos);
    fila_alta_prioridade = *(FilaProcessos *)malloc(sizeof(FilaProcessos) * quantidade_processos);
    fila_io_disco = *(FilaProcessos *)malloc(sizeof(FilaProcessos) * quantidade_processos);
    fila_io_fita = *(FilaProcessos *)malloc(sizeof(FilaProcessos) * quantidade_processos);
    fila_io_impressora = *(FilaProcessos *)malloc(sizeof(FilaProcessos) * quantidade_processos);
    fila_concluidos = *(FilaProcessos *)malloc(sizeof(FilaProcessos) * quantidade_processos);

    srand(time(NULL));

    // inicializa os processos
    for (int i = 0; i < quantidade_processos; i++)
    {
        lista_processos[i].pid = i + 1;

        // Determina de maneira aleatória o instante de início e duração total de cada processo
        lista_processos[i].inicio = rand() % MAX_INICIO;
        lista_processos[i].duracao = (rand() % MAX_DURACAO) + 1;
        lista_processos[i].tempo_executado = 0;
        lista_processos[i].status = (char *)malloc(sizeof(char) * 10);
        lista_processos[i].status = "NOVO";
        // NOVO, PRONTO, BLOQUEADO, EXECUTANDO, CONCLUIDO

        lista_processos[i].prioridade = 1; // define prioridade alta para todos os novos processos

        if (rand() % 2 == 0)
        { // atribui aleatoriamente se processo é IO ou não
            lista_processos[i].io.tipo = tipo_io[rand() % 3];
            lista_processos[i].io.inicio = (rand() % lista_processos[i].duracao) + lista_processos[i].inicio;

            switch (lista_processos[i].io.tipo)
            {
            case 'D':
                lista_processos[i].io.duracao = DURACAO_IO_DISCO;
                lista_processos[i].duracao += DURACAO_IO_DISCO;
                break;

            case 'F':
                lista_processos[i].io.duracao = DURACAO_IO_FITA;
                lista_processos[i].duracao += DURACAO_IO_FITA;
                break;

            case 'I':
                lista_processos[i].io.duracao = DURACAO_IO_IMPRESSORA;
                lista_processos[i].duracao += DURACAO_IO_IMPRESSORA;
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
    }
}

Processo *seleciona_processo_para_execucao()
{
    if (!fila_vazia(fila_alta_prioridade))
    {
        return desenfileira_processo(&fila_alta_prioridade);
    }

    if (!fila_vazia(fila_baixa_prioridade))
    {
        return desenfileira_processo(&fila_baixa_prioridade);
    }

    return NULL;
}

void comeca_io_de_processo(Processo *processo)
{
    switch (processo->io.tipo)
    {
    case 'D':
        printf("Processo PID=%d começou sua operação de I/O em disco\n", processo->pid);
        enfileira_processo(processo, &fila_io_disco);
        break;

    case 'F':
        printf("Processo PID=%d começou sua operação de I/O em fita magnética\n", processo->pid);
        enfileira_processo(processo, &fila_io_fita);
        break;

    case 'I':
        printf("Processo PID=%d começou sua operação de I/O em impressora\n", processo->pid);
        enfileira_processo(processo, &fila_io_impressora);
        break;

    default:
        break;
    }
}

void termina_io_de_processo(Processo *processo)
{
    Processo *primeiro_io;
    switch (processo->io.tipo)
    {
    case 'D':
        if (!fila_vazia(fila_io_disco))
        {
            primeiro_io = primeiro_da_fila(fila_io_disco);
            if (processo->pid == primeiro_io->pid)
            {
                if (primeiro_io->io.duracao == 0)
                {
                    printf("Processo PID=%d terminou sua operação de I/O em disco\n", primeiro_io->pid);
                    desenfileira_processo(&fila_io_disco);
                    primeiro_io->io.tipo = '\0';
                    enfileira_processo(processo, &fila_baixa_prioridade);
                }
            }
        }
        break;

    case 'F':
        if (!fila_vazia(fila_io_fita))
        {
            primeiro_io = primeiro_da_fila(fila_io_fita);
            if (processo->pid == primeiro_io->pid)
            {
                if (primeiro_io->io.duracao == 0)
                {
                    printf("Processo PID=%d terminou sua operação de I/O em fita magnética\n", primeiro_io->pid);
                    desenfileira_processo(&fila_io_fita);
                    primeiro_io->io.tipo = '\0';
                    enfileira_processo(processo, &fila_alta_prioridade);
                }
            }
        }
        break;

    case 'I':
        if (!fila_vazia(fila_io_impressora))
        {
            primeiro_io = primeiro_da_fila(fila_io_impressora);
            if (processo->pid == primeiro_io->pid)
            {
                if (primeiro_io->io.duracao == 0)
                {
                    printf("Processo PID=%d terminou sua operação de I/O em impressora\n", primeiro_io->pid);
                    desenfileira_processo(&fila_io_impressora);
                    primeiro_io->io.tipo = '\0';
                    enfileira_processo(processo, &fila_alta_prioridade);
                }
            }
        }
        break;

    default:
        break;
    }
}

void decrementa_duracao_io(FilaProcessos fila_io)
{
    for (int i = 0; i < fila_io.num_processos; i++)
    {
        if (fila_io.processos[i]->io.duracao > 0)
        {
            fila_io.processos[i]->io.duracao--;
        }
    }
}

void imprime_filas_prioridade(){
    if(!fila_vazia(fila_alta_prioridade))
    {
        printf("Fila de alta prioridade:\n");
        imprime_fila_processos(fila_alta_prioridade);
    }
    if(!fila_vazia(fila_baixa_prioridade))
    {
        printf("Fila de baixa prioridade:\n");
        imprime_fila_processos(fila_baixa_prioridade);
    }
}

void round_robin(unsigned quantum, unsigned quantidade_processos)
{
    int i = 0;
    Processo *processo_executando = NULL;
    unsigned tempo_execucao_corrente = 0;

    inicia_fila(&fila_alta_prioridade, quantidade_processos);
    inicia_fila(&fila_baixa_prioridade, quantidade_processos);
    inicia_fila(&fila_io_disco, quantidade_processos);
    inicia_fila(&fila_io_impressora, quantidade_processos);
    inicia_fila(&fila_io_fita, quantidade_processos);
    inicia_fila(&fila_concluidos, quantidade_processos);

    // Numero mágico para testar impressão. Precisa definir condições de término do programa
    //while (qtd_processos_finalizados < MAX_PROCESSOS)
    while (1)
    {
        printf("\nInstante t = %d\n", tempo_corrente);

        // Adiciona cada processo em sua devida fila
        while (i < quantidade_processos)
        {   
            Processo *processo_atual = &lista_processos[i];

            // Todos os processos são iniciados com alta prioridade
            if (processo_atual->inicio == tempo_corrente)
            {
                enfileira_processo(processo_atual, &fila_alta_prioridade);
                printf("Processo PID %d entrou na fila de alta prioridade\n", processo_atual->pid);
            }

            if (processo_atual->io.tipo)
            {
                // Para cada processo, confere se ele terminou sua operação de IO, caso esteja bloqueado
                termina_io_de_processo(processo_atual);
            }

            i++;
        }

        

        // Confere se há um processo nas filas de pronto que pode ser executado
        if (!processo_executando)
        {
            
            imprime_filas_prioridade();
            processo_executando = seleciona_processo_para_execucao();
            tempo_execucao_corrente = 0;
            // if(processo_executando)
            // {
                // processo_executando->tempo_executado++;
                // tempo_execucao_corrente++;
            // }
            // else
                // printf("Não há processo em execução\n");
        }

        // Caso nenhum processo novo seja colocado em execução o processo corrente continua executando
        else if (processo_executando->tempo_executado == processo_executando->duracao)
        {
            // Caso o processo já tenha executado toda sua duração, ele é movido para a saída e o próximo processo é selecionado.
            printf("Processo %d finalizado\n", processo_executando->pid);
            processo_executando->status = "CONCLUIDO";
            enfileira_processo(processo_executando, &fila_concluidos);
            qtd_processos_finalizados++;

            // Se todos os processo da foram concluídos, a simulação é encerrada
            if(qtd_processos_finalizados == quantidade_processos)
            {
                printf("Todos os processos finalizados.\n");
                exit(-1);
            }

            // Se houver outro processo em alguma das filas de pronto ele é executado, se não continua aguardando.
            processo_executando = seleciona_processo_para_execucao();
            if(processo_executando)
            {
                printf("Novo processo selecionado: %d\n", processo_executando->pid);
                tempo_execucao_corrente = 0;
            }
            else
            {
                printf("Não há processo nas listas de prontos. Aguardando.\n");
            } 
        }  

        // Caso o processo em execução tenha alguma operação de IO para realizar, testa se é o instante de início da mesma.
        else if (processo_executando->io.tipo != '\0')
        {
            if (processo_executando->tempo_executado == processo_executando->io.inicio)
            {
                comeca_io_de_processo(processo_executando);
                processo_executando = seleciona_processo_para_execucao();
                tempo_execucao_corrente = 0;
            }
        }
            

        // Caso o processo em execução já tenha esgotado seu time slice, seleciona um novo processo para execução
        // e o processo preemptado vai para a fila de baixa prioridade.
        else if (processo_executando->tempo_executado > processo_executando->duracao)
        {
            printf("ERRO: O tempo de execução do processo %d é maior que a duração máxima\n\tTempo Executado: %d | Duracao: %d\n", processo_executando->pid, processo_executando->tempo_executado, processo_executando->duracao);
            exit(-1);
        }

        if(processo_executando)
        {
            processo_executando->tempo_executado++;
            tempo_execucao_corrente++;
        }
        else 
        {
            printf("Não há processos nas listas de prontos. Aguardando. \n");   
        }
    
        if (processo_executando && tempo_execucao_corrente)
        {
            printf("Processo PID=%d está executando | Tempo de execução corrente: %d | Tempo total executado: %d\n", processo_executando->pid, tempo_execucao_corrente, processo_executando->tempo_executado);
            if (tempo_execucao_corrente >= quantum)
            {
                printf("Tempo de execucao do processo %d atingiu valor do quantum(%d)\n", processo_executando->pid, quantum);
                if(fila_vazia(fila_alta_prioridade) || fila_vazia(fila_baixa_prioridade))
                {
                    printf("Não há processos prontos para receber prioridade. Continuando execução do processo PID = %d", processo_executando->pid);
                }
                else
                {    
                    enfileira_processo(processo_executando, &fila_baixa_prioridade);
                    processo_executando = seleciona_processo_para_execucao();
                    tempo_execucao_corrente = 0;
                    if(processo_executando)
                    {
                        printf("Novo processo selecionado: %d\n", processo_executando->pid);
                        processo_executando->tempo_executado++;
                        tempo_execucao_corrente++;
                    }
                    else 
                    {
                        printf("Não há processos nas listas de prontos. Aguardando. \n");   
                    }
                }    
            }
        }
        
        i = 0;
        imprime_filas_prioridade();

        // Ao final de cada ciclo, reduz a duração da operação de IO para cada processo bloqueado.
        decrementa_duracao_io(fila_io_disco);
        decrementa_duracao_io(fila_io_fita);
        decrementa_duracao_io(fila_io_impressora);
        tempo_corrente++;
        printf("Processos concluidos: %d\n", qtd_processos_finalizados);
    }
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

int main(int argc, char *argv[])
{
    criar_processos(5);
    imprime_tabela_processos(5);

    round_robin(5, 5);
    printf("Fila de alta prioridade:\n");
    imprime_fila_processos(fila_alta_prioridade);
    return 0;
}