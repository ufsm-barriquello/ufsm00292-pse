#include <stdint.h>
#include <stdio.h>

#include "rtos.h"

/*
 * Prototipos das tarefas
 */
void tarefa_1(void);
void tarefa_2(void);
void tarefa_periodica(void); // Prototipo da tarefa periodica

/*
 * Configuracao dos tamanhos das pilhas
 */
#define TAM_PILHA_1         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_2         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_PERIODICA (TAM_MINIMO_PILHA + 24) // Tamanho da pilha da tarefa periodica
#define TAM_PILHA_OCIOSA    (TAM_MINIMO_PILHA + 24)

/*
 * Declaracao das pilhas das tarefas
 */
uint32_t PILHA_TAREFA_1[TAM_PILHA_1];
uint32_t PILHA_TAREFA_2[TAM_PILHA_2];
uint32_t PILHA_TAREFA_PERIODICA[TAM_PILHA_PERIODICA]; // Declaracao da pilha
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Funcao principal de entrada do sistema
 */
int main(void)
{
    
    /* Criacao das tarefas */
    /* Parametros: ponteiro, nome, ponteiro da pilha, tamanho da pilha, prioridade da tarefa */
    
    CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 1);
    
    CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 2);

    /* Criacao da tarefa periodica de 100 ms */
    CriaTarefa(tarefa_periodica, "Tarefa Periodica", PILHA_TAREFA_PERIODICA, TAM_PILHA_PERIODICA, 3);
    
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
        TarefaSuspende(2);  
    }
}

/* 
 * Tarefa periodica que executa a cada 100 ms
 * 
 * Comparação de Modos - Cooperativo vs Preemptivo:
 * - Modo Cooperativo: O escalonamento depende de cada tarefa ceder o controle voluntariamente 
 *   ao processador. Se uma tarefa demorar mais do que o esperado, a periodicidade de 100 ms 
 *   pode ser comprometida e atrasada, pois o sistema nao interrompe a tarefa atual a força.
 * - Modo Preemptivo: O RTOS interrompe a tarefa atual de forma forcada por estouro de tempo 
 *   ou quando uma tarefa de maior prioridade fica pronta, garantindo uma  rigorosa e precisa execução temporal a cada 100 ms.
 */
void tarefa_periodica(void)
{
    volatile uint16_t contador = 0;
    for(;;)
    {
        contador++;
    }
}