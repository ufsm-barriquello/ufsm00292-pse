/**
 * \file main.c
 * \brief Exercicio 02 - Comparacao entre escalonamento cooperativo e preemptivo
 *
 * COMPARACAO ENTRE OS MODOS:
 * 
 * 1. Modo Preemptivo:
 *    - O SysTick gera interrupcoes a cada 1 ms (cfg_MARCA_TEMPO_HZ = 1000).
 *    - Dentro de SysTick_Handler(), TrocaContexto() eh chamada a cada tick.
 *    - A tarefa periodica de 100 ms (maior prioridade) acorda exatamente a cada
 *      100 ms, preemptando imediatamente a tarefa de carga (menor prioridade).
 *    - O determinismo e a precisao temporal sao garantidos por hardware.
 *
 * 2. Modo Cooperativo:
 *    - Se comentarmos TrocaContexto() em SysTick_Handler(), o escalonamento
 *      passa a depender de liberacao voluntaria da CPU pelas tarefas.
 *    - Caso a tarefa de menor prioridade execute um processamento demorado
 *      sem ceder a CPU voluntariamente, a tarefa de 100 ms sofre atraso (jitter)
 *      ou pode ser impedida de rodar no prazo previsto.
 */

#include <asf.h>
#include "stdint.h"
#include "rtos.h"

/* Prototipos das tarefas */
void tarefa_100ms(void);
void tarefa_carga(void);

/* Configuracao do tamanho das pilhas */
#define TAM_PILHA_1          (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_2          (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_OCIOSA     (TAM_MINIMO_PILHA + 24)

/* Declaracao das pilhas */
uint32_t PILHA_TAREFA_1[TAM_PILHA_1];
uint32_t PILHA_TAREFA_2[TAM_PILHA_2];
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/* Variaveis globais de inspecao */
volatile uint32_t contador_100ms = 0;
volatile uint32_t contador_carga = 0;

int main(int argc, char** argv)
{
#if 0
    system_init();
#endif

    /*
     * Criacao das tarefas:
     * - tarefa_100ms recebe prioridade 2 (maior)
     * - tarefa_carga recebe prioridade 1 (menor)
     * - tarefa_ociosa recebe prioridade 0 (minima)
     */
    CriaTarefa(tarefa_100ms, "Periodica 100ms", PILHA_TAREFA_1, TAM_PILHA_1, 2);
    CriaTarefa(tarefa_carga, "Carga CPU", PILHA_TAREFA_2, TAM_PILHA_2, 1);
    CriaTarefa(tarefa_ociosa, "Ociosa", PILHA_TAREFA_OCIOSA, TAM_PILHA_OCIOSA, 0);

    /* Habilita o timer SysTick para temporizacao do sistema */
    ConfiguraMarcaTempo();

    /* Inicia o kernel multitarefas */
    IniciaMultitarefas();

    /* O fluxo nunca atinge este ponto */
    return (EXIT_SUCCESS);
    while (1)
    {
    }
}

/**
 * \brief Tarefa periodica executada a cada 100 ms (100 ticks com tick de 1 ms)
 */
void tarefa_100ms(void)
{
    for(;;)
    {
        contador_100ms++;
        
        /* Coloca a tarefa em espera por 100 marcas de tempo (100 ms) */
        TarefaEspera(100);
    }
}

/**
 * \brief Tarefa continua concorrente para testar a preempcao
 */
void tarefa_carga(void)
{
    for(;;)
    {
        contador_carga++;
        
        /* 
         * No modo preemptivo, o SysTick interrompe este loop automaticamente.
         * No modo cooperativo, este laco reteria a CPU indefinidamente se
         * nao houvesse uma chamada explicita para ceder o controle.
         */
    }
}