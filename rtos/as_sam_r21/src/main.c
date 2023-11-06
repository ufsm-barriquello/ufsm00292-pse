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
	
	CriaTarefa(tarefa_1, "Tarefa 7", PILHA_TAREFA_7, TAM_PILHA_7, 1);
	
	CriaTarefa(tarefa_2, "Tarefa 8", PILHA_TAREFA_8, TAM_PILHA_8, 2);
	
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



#define TAM_BUFFER 5
uint8_t buffer[TAM_BUFFER]; 

semaforo_t SemaforoCheio = {0,0};
semaforo_t SemaforoVazio = {TAM_BUFFER,0}; 
void tarefa_7(void)
{

	uint8_t a = 1;			
	uint8_t i = 0;
	
	for(;;)
	{
		SemaforoAguarda(&SemaforoVazio);
		buffer[i] = a++;
		i = (i+1)%TAM_BUFFER;
		SemaforoLibera(&SemaforoCheio); 
		TarefaEspera(10); 		
	}
}


void tarefa_8(void)
{
	static uint8_t f = 0;
	volatile uint8_t valor;
		
	for(;;)
	{
		volatile uint8_t contador;
		do{
			REG_ATOMICA_INICIO();			
				contador = SemaforoCheio.contador;			
			REG_ATOMICA_FIM();
			if (contador == 0)
			{
				TarefaEspera(100);
			}
				
		} while (!contador);
		
		SemaforoAguarda(&SemaforoCheio);
		valor = buffer[f];
		f = (f+1) % TAM_BUFFER;	
		(void)valor;
		SemaforoLibera(&SemaforoVazio);
	}
}
