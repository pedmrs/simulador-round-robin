#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSOS 10 // número máximo de processos
#define MAX_INICIO 20    // limite máximo para inicio de processo
#define MAX_DURACAO 30   // limite máximo para duração de processo

typedef struct
{
    char tipo;
    unsigned inicio;
    unsigned duracao;
} IO;

typedef struct
{
    unsigned pid;
    unsigned prioridade;
    unsigned inicio;
    unsigned duracao_restante;
    IO io;
} Processo;

Processo *lista_processos, *fila_baixa_prioridade, *fila_alta_prioridade, *fila_io;

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

        if (rand() % 2 == 0)
        { // atribui aleatoriamente se processo é IO ou não
            lista_processos[i].io.tipo = tipo_io[rand() % 3];
        }
        else
        {
            lista_processos[i].io.tipo = '\0';
        }

        // TO DO: Atribuir duração aleatoria para operações de IO de cada processo

        // Determina de maneira aleatória o instante de início e duração total de cada processo
        lista_processos[i].inicio = rand() % MAX_INICIO;
        lista_processos[i].duracao_restante = rand() % MAX_DURACAO;

        lista_processos[i].prioridade = 1; // define prioridade alta para todos os novos processos

        if (lista_processos[i].io.tipo == 'F' || lista_processos[i].io.tipo == 'I')
        {
            lista_processos[i].prioridade += 1; // aumenta a prioridade
        }
    }
}

void round_robin(unsigned quantum, unsigned quantidade_processos)
{
    int i = 0;

    fila_alta_prioridade = (Processo *)malloc(sizeof(Processo) * quantidade_processos);
    fila_baixa_prioridade = (Processo *)malloc(sizeof(Processo) * quantidade_processos);
    fila_io = (Processo *)malloc(sizeof(Processo) * quantidade_processos);

    while (1)
    {
        while (i < quantidade_processos)
        {
            if (lista_processos[i].prioridade > 0)
            {
                fila_alta_prioridade[i] = lista_processos[i];
                if (lista_processos[i].io.tipo != '\0')
                {
                    fila_io[i] = lista_processos[i];
                }
            }
            else
            {
                fila_baixa_prioridade[i] = lista_processos[i];
            }

            if (lista_processos[i].inicio <= tempo_corrente)
            {
                if (lista_processos[i].duracao_restante > 0)
                {
                    if (lista_processos[i].duracao_restante <= quantum)
                    { // processo sem preempção
                        lista_processos[i].duracao_restante = 0;
                        tempo_corrente += lista_processos[i].duracao_restante;
                    }
                    else
                    {
                        lista_processos[i].duracao_restante -= quantum;
                        if (lista_processos[i].io.tipo == 'D')
                        {
                            fila_baixa_prioridade[i] = lista_processos[i];
                        }
                        else
                        {
                            fila_alta_prioridade[i] = lista_processos[i];
                        }

                        tempo_corrente += quantum;
                    }
                }
            }

            i++;
        }
        i = 0;
    }
}

void imprime_tabela_processos(int qtd)
{
    printf("-------------------------------------------------------\n");
    printf("| PID | Prioridade | Tempo de Início | Duração |  IO  |\n");
    printf("-------------------------------------------------------\n");
    int i = 0;
    for(i = 0; i < qtd; i++){
        Processo processo_atual = lista_processos[i];
        printf("|  %d  |      %d     |        %d        |    %d    |   %c  |\n", 
                processo_atual.pid, processo_atual.prioridade, processo_atual.inicio, processo_atual.duracao_restante, processo_atual.io.tipo);
        printf("-------------------------------------------------------\n");

    }
}

/*  -------------------------------------------------------
    | PID | Prioridade | Tempo de Início | Duração |  IO  |
    -------------------------------------------------------
    |  1  |      0     |        0        |    5    |   D  |
    */

int main(int argc, char *argv[]) {
    criar_processos(5);
    imprime_tabela_processos(5);

    return 0;
}