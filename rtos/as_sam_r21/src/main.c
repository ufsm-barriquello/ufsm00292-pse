/**
 * \file
 *
 * \brief Exemplos diversos de tarefas e funcionalidades de um sistema operacional multitarefas.
 *
 */

/**
 * \mainpage Sistema operacional multitarefas
 *
 * \par Exemplso de tarefas
 *
 * Este arquivo contem exemplos diversos de tarefas e 
 * funcionalidades de um sistema operacional multitarefas.
 *
 *
 * \par Conteudo
 *
 * -# Inclui funcoes do sistema multitarefas (atraves de multitarefas.h)
 * -# Inicializa��o do processador e do sistema multitarefas
 * -# Cria��o de tarefas de demonstra��o
 *
 */

/*
 * Inclusao de arquivos de cabecalhos
 */
#include <asf.h>
#include "stdint.h"
#include "rtos.h"

/*
 * Prototipos das tarefas
 */
void tarefa_1(void);
void tarefa_2(void);
void tarefa_periodica_preemp(void); // Nova tarefa periódica
void tarefa_periodica_cooperativa(void); // Nova tarefa periódica
void RTT_Handler(void);
/*
 * Configuracao dos tamanhos das pilhas
 */
#define TAM_PILHA_1			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_2			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_OCIOSA	(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_PERIODICA_PREEMP (TAM_MINIMO_PILHA + 24) // Tamanho da pilha da tarefa periódica
#define TAM_PILHA_PERIODICA_COOP (TAM_MINIMO_PILHA + 24) // Tamanho da pilha da tarefa periódica
/*
 * Declaracao das pilhas das tarefas
 */
uint32_t PILHA_TAREFA_1[TAM_PILHA_1];
uint32_t PILHA_TAREFA_2[TAM_PILHA_2];
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];
uint32_t PILHA_PERIODICA_PREEMP[TAM_PILHA_PERIODICA_PREEMP]; // Pilha da tarefa periódica
uint32_t PILHA_PERIODICA_COOP[TAM_PILHA_PERIODICA_COOP]; // Pilha da tarefa periódica
/*
 * Funcao principal de entrada do sistema
 */
int main(void)
{
	system_init();
	
		/* Criacao das tarefas */
	/* Parametros: ponteiro, nome, ponteiro da pilha, tamanho da pilha, prioridade da tarefa */
	rtt_enable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_RTTINC(1000));
    NVIC_EnableIRQ(RTT_IRQn);

	CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 1);
	
	CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 2);

	 /* Criar tarefa periódica */
    CriaTarefa(tarefa_periodica_preemp, "Tarefa Periódica Preemptiva", PILHA_PERIODICA_PREEMP, TAM_PILHA_PERIODICA_PREEMP, 1);
	/* Criar tarefa periódica */
    CriaTarefa(tarefa_periodica_cooperativa, "Tarefa Periódica Cooperativa", PILHA_PERIODICA_COOP, TAM_PILHA_PERIODICA_COOP, 1);
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
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE); /* Liga LED. */
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
		port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE); 	/* Turn LED off. */
	}
}
/* Função de interrupção do RTT */
void RTT_Handler(void)
{
    milissegundos++; // Incrementa o contador de milissegundos
    TarefaContinua(1); // Ativa a tarefa periódica
}

/* Tarefa periódica executada a cada 100 ms, no modo preemptivo */
void tarefa_periodica_preemp(void)
{
      static uint32_t contador = 0;
    port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
    TarefaSuspende(1);
}

/* Tarefa periódica executada a cada 100 ms, no modo cooperativo */
void tarefa_periodica_cooperativa(void)
{
      static uint32_t contador = 0;

    while (1) {
        // Executa o código da tarefa

        // Incrementa o contador
        contador++;

        // Verifica se passaram 100 iterações (100 ms)
        if (contador == 100) {
            // Reseta o contador
            contador = 0;

            port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);

            // Ponto de cooperação: permite que outras tarefas executem
            TarefaSuspende(1); // Suspender a tarefa periódica
            TarefaContinua(2); // Continuar a tarefa 2
        }

        // Espera um curto período de tempo antes de executar novamente
        delay_ms(1);
    }
}