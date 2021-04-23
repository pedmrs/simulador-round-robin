#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PROCESSOS 10  // número máximo de processos
#define MAX_INICIO 20     // limite máximo para inicio de processo
#define MAX_DURACAO 29    // limite máximo para duração de processo
#define MAX_DURACAO_IO 14 // limite máximo para duração de operação de I/O

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