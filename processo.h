#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PROCESSOS 10        // número máximo de processos
#define MAX_INICIO 7           // limite máximo para inicio de processo
#define MAX_DURACAO 5          // limite máximo para duração de processo
#define DURACAO_IO_DISCO 3      // limite máximo para duração de operação de I/O em disco
#define DURACAO_IO_FITA 4       // limite máximo para duração de operação de I/O em fita magnética
#define DURACAO_IO_IMPRESSORA 5 // limite máximo para duração de operação de I/O em impressora


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
    unsigned duracao;
    unsigned tempo_executado;
    char *status;
    IO io;
} Processo;