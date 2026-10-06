#include <stdint.h>
#include <stdio.h>

#include "rtos.h"

/*
 * Prototipos das tarefas
 */
void tarefa_1(void);
void tarefa_2(void);
void tarefa_3(void); // NOVA TAREFA

/*
 * Configuracao dos tamanhos das pilhas
 */
#define TAM_PILHA_1     (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_2     (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_3     (TAM_MINIMO_PILHA + 24) // NOVA TAREFA
#define TAM_PILHA_OCIOSA    (TAM_MINIMO_PILHA + 24)

/*
 * Declaracao das pilhas das tarefas
 */
uint32_t PILHA_TAREFA_1[TAM_PILHA_1];
uint32_t PILHA_TAREFA_2[TAM_PILHA_2];
uint32_t PILHA_TAREFA_3[TAM_PILHA_3]; // NOVA TAREFA
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Funcao principal de entrada do sistema
 */
int main(void)
{
    /* Criacao das tarefas */
    CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 1);
    CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 2);
    CriaTarefa(tarefa_3, "Tarefa 3", PILHA_TAREFA_3, TAM_PILHA_3, 3); // NOVA TAREFA
    
    /* Cria tarefa ociosa do sistema */
    CriaTarefa(tarefa_ociosa,"Tarefa ociosa", PILHA_TAREFA_OCIOSA, TAM_PILHA_OCIOSA, 0);
    
    /* Configura marca de tempo */
    ConfiguraMarcaTempo();   
    
    /* Inicia sistema multitarefas */
    IniciaMultitarefas();
    
    /* Nunca chega aqui */
    while (1)
    {
    }
}

/* Tarefas de exemplo que usam funcoes para suspender/continuar as tarefas */
void tarefa_1(void)
{
    volatile uint16_t a = 0;
    for(;;)
    {
        a++;
        TarefaContinua(2);
    }
}

void tarefa_2(void)
{
    volatile uint16_t b = 0;
    for(;;)
    {
        b++;
        TarefaContinua(3); // Continua a tarefa 3
        TarefaSuspende(2);  
    }
}

// IMPLEMENTAÇÃO DA NOVA TAREFA
void tarefa_3(void)
{
    volatile uint16_t c = 0;
    for(;;)
    {
        c++;
        TarefaSuspende(3); // Suspende a si mesma para devolver o controle
    }
}