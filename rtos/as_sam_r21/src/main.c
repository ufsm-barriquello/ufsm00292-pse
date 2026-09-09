/**
 * \file
 * \brief Atividade 8 - Escalonamento Preemptivo Encadeado de Tarefas com Contadores
 */

#include <asf.h>
#include "stdint.h"
#include "rtos.h"
#include <stdio.h>

/*
 * Identificadores numéricos das tarefas para o RTOS
 */
#define ID_THREAD0 1
#define ID_THREAD1 2
#define ID_THREAD2 3
#define ID_THREAD3 4
#define ID_THREAD4 5
#define ID_MONITOR 6

/*
 * Protótipos das Tarefas
 */
void thread0(void);
void thread1(void);
void thread2(void);
void thread3(void);
void thread4(void);
void tarefa_monitor_30s(void);

/*
 * Configuração dos Tamanhos das Pilhas
 */
#define TAM_PILHA (TAM_MINIMO_PILHA + 24)

uint32_t PILHA_THREAD0[TAM_PILHA];
uint32_t PILHA_THREAD1[TAM_PILHA];
uint32_t PILHA_THREAD2[TAM_PILHA];
uint32_t PILHA_THREAD3[TAM_PILHA];
uint32_t PILHA_THREAD4[TAM_PILHA];
uint32_t PILHA_MONITOR[TAM_PILHA];
uint32_t PILHA_OCIOSA[TAM_PILHA];

/*
 * Contadores individuais de execução de cada tarefa
 */
volatile uint32_t count0 = 0;
volatile uint32_t count1 = 0;
volatile uint32_t count2 = 0;
volatile uint32_t count3 = 0;
volatile uint32_t count4 = 0;

/*
 * Função principal de entrada do sistema
 */
int main(int argc, char** argv)
{
#if 0
    system_init();
#endif	

    /* 
     * Criacao das 5 Tarefas em ordem decrescente de prioridade (5 é a maior prioridade):
     * - Thread0 (Maior prioridade) inicia suspensa
     * - Thread1 a Thread3 iniciam suspensas
     * - Thread4 (Menor prioridade) inicia rodando
     */
    CriaTarefa(thread0, "Thread0", PILHA_THREAD0, TAM_PILHA, 5);
    CriaTarefa(thread1, "Thread1", PILHA_THREAD1, TAM_PILHA, 4);
    CriaTarefa(thread2, "Thread2", PILHA_THREAD2, TAM_PILHA, 3);
    CriaTarefa(thread3, "Thread3", PILHA_THREAD3, TAM_PILHA, 2);
    CriaTarefa(thread4, "Thread4", PILHA_THREAD4, TAM_PILHA, 1);

    /* 6ª Tarefa: Monitora e imprime a soma a cada 30s (Prioridade alta para garantir impressão) */
    CriaTarefa(tarefa_monitor_30s, "Monitor30s", PILHA_MONITOR, TAM_PILHA, 6);

    /* Cria tarefa ociosa do sistema */
    CriaTarefa(tarefa_ociosa, "Tarefa ociosa", PILHA_OCIOSA, TAM_PILHA, 0);

    /* Suspende inicialmente Thread0 a Thread3 conforme especificação */
    TarefaSuspende(ID_THREAD0);
    TarefaSuspende(ID_THREAD1);
    TarefaSuspende(ID_THREAD2);
    TarefaSuspende(ID_THREAD3);

#if 0
    ConfiguraMarcaTempo();   
#endif	

    /* Inicia sistema multitarefas */
    IniciaMultitarefas();

    return (EXIT_SUCCESS);
}

/* 
 * Thread 0: Maior Prioridade 
 */
void thread0(void)
{
    for (;;)
    {
        count0++;
        TarefaSuspende(ID_THREAD0); /* Suspende a si mesma */
    }
}

/* 
 * Thread 1: Acorda Thread 0 e suspende a si mesma 
 */
void thread1(void)
{
    for (;;)
    {
        count1++;
        TarefaContinua(ID_THREAD0); /* Resume Thread0 */
        TarefaSuspende(ID_THREAD1); /* Suspende a si mesma */
    }
}

/* 
 * Thread 2: Acorda Thread 1 e suspende a si mesma 
 */
void thread2(void)
{
    for (;;)
    {
        count2++;
        TarefaContinua(ID_THREAD1); /* Resume Thread1 */
        TarefaSuspende(ID_THREAD2); /* Suspende a si mesma */
    }
}

/* 
 * Thread 3: Acorda Thread 2 e suspende a si mesma 
 */
void thread3(void)
{
    for (;;)
    {
        count3++;
        TarefaContinua(ID_THREAD2); /* Resume Thread2 */
        TarefaSuspende(ID_THREAD3); /* Suspende a si mesma */
    }
}

/* 
 * Thread 4: Menor Prioridade - Acorda Thread 3 (Reinicia a cadeia) 
 */
void thread4(void)
{
    for (;;)
    {
        count4++;
        TarefaContinua(ID_THREAD3); /* Resume Thread3 */
        /* Nao precisa se suspender pois a Thread3 preempta imediatamente por ter prioridade maior */
    }
}

/* 
 * 6ª Tarefa: Imprime a soma dos contadores no terminal a cada 30 segundos (30000 ms/ticks)
 */
void tarefa_monitor_30s(void)
{
    for (;;)
    {
        TarefaEspera(30000); /* Aguarda 30 segundos */

        REG_ATOMICA_INICIO();
        uint32_t soma = count0 + count1 + count2 + count3 + count4;
        REG_ATOMICA_FIM();

        /* Imprime a soma total dos contadores via terminal */
        printf("\n[RTOS 30s] Soma dos contadores das tarefas: %lu\n", (unsigned long)soma);
        printf("Count0: %lu | Count1: %lu | Count2: %lu | Count3: %lu | Count4: %lu\n",
               (unsigned long)count0, (unsigned long)count1, 
               (unsigned long)count2, (unsigned long)count3, (unsigned long)count4);
    }
}