#include <stdint.h>
#include <stdio.h>

#include "rtos.h"

/*
 * Prototipos das tarefas
 */
void tarefa_1(void);
void tarefa_2(void);
void tarefa_3(void);

/*
 * Configuracao dos tamanhos das pilhas
 */
#define TAM_PILHA_1 (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_2 (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_OCIOSA (TAM_MINIMO_PILHA + 24)

// modo Cooperativo: 0(desligado)
// modo Preemptivo: 1 (ligado)
#define configUSE_PREEMPETION = 0

/* =================================================
	COMPARAÇÃO: MODO COOPERATIVO VS MODO PREEMPTIVO
	================================================
	No Modo Cooperativo, a troca de contexto só ocorre quando uma tarefa cede voluntariamente o controle da CPU chamando uma função
	de bloqueio (como a TarefaAtraso() de 100ms na tarefa3). Se uma tarefa
	entrar em um laço de cálculo pesado, as outras tarefas ficarão atrasadas, independentemente da prioridade.

	No Modo Preemptivo, o RTOS utiliza o temporizador de hardware (tick) para forçar a troca de contexto.
	Mesmo que uma tarefa de menor prioridade esteja no meio de um processamento pesado sem ceder a vez, quando o temporarizador
	atinge os 100ms, o RTOS a interrompe à força (preempção) para garantir que a tarefa_3 execute no prazo exato, mantendo o
	determinismo temporal exigido pelo sistema.
	================================================
	*/

/*
 * Declaracao das pilhas das tarefas
 */
uint32_t PILHA_TAREFA_1[TAM_PILHA_1];
uint32_t PILHA_TAREFA_2[TAM_PILHA_2];
uint32_t PILHA_TAREFA_3[TAM_PILHA_3];
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
	CriaTarefa(tarefa_3, "Tarefa 3", PILHA_TAREFA_3, TAM_PILHA_3, 3);

	/* Cria tarefa ociosa do sistema */
	CriaTarefa(tarefa_ociosa, "Tarefa ociosa", PILHA_TAREFA_OCIOSA, TAM_PILHA_OCIOSA, 0);

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
	for (;;)
	{
		a++;
		TarefaContinua(2);
	}
}

void tarefa_2(void)
{
	volatile uint16_t b = 0;
	for (;;)
	{
		b++;
		TarefaSuspende(2);
	}
}

void tarefa_3(void *pvParameters)
{
	volatile uint16_t c = 0;
	for (;;)
	{
		c++;
		// coloca a tarefa em sleep por 100ms
		tarefaAtrasa(100);
	}
}