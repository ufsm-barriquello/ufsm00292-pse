/**
 * \file
 *
 * \brief Exemplos diversos de tarefas e funcionalidades de um sistema operacional multitarefas.
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
void tarefa_3(void);
void tarefa_4(void);
void tarefa_5(void);
void tarefa_6(void);
void tarefa_7(void);
void tarefa_8(void);

/*
 * Configuracao dos tamanhos das pilhas
 */
#define TAM_PILHA_1			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_2			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_3			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_4			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_5			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_6			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_7			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_8			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_OCIOSA	(TAM_MINIMO_PILHA + 24)

/*
 * Declaracao das pilhas das tarefas
 */
uint32_t PILHA_TAREFA_1[TAM_PILHA_1];
uint32_t PILHA_TAREFA_2[TAM_PILHA_2];
uint32_t PILHA_TAREFA_3[TAM_PILHA_3];
uint32_t PILHA_TAREFA_4[TAM_PILHA_4];
uint32_t PILHA_TAREFA_5[TAM_PILHA_5];
uint32_t PILHA_TAREFA_6[TAM_PILHA_6];
uint32_t PILHA_TAREFA_7[TAM_PILHA_7];
uint32_t PILHA_TAREFA_8[TAM_PILHA_8];
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Variaveis de contagem globais para inspecao no Watch do depurador
 */
volatile uint16_t a = 0;
volatile uint16_t b = 0;
volatile uint16_t c = 0;

/*
 * Funcao principal de entrada do sistema
 */
int main(int argc, char** argv)
{
#if 0
	system_init();
#endif	
	/* Criacao das tarefas */
	/* Parametros: ponteiro, nome, ponteiro da pilha, tamanho da pilha, prioridade da tarefa */
	
	CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 1);
	
	CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 2);
	
	/* Tarefa 3 adicionada */
	CriaTarefa(tarefa_3, "Tarefa 3", PILHA_TAREFA_3, TAM_PILHA_3, 3);
	
	/* Cria tarefa ociosa do sistema */
	CriaTarefa(tarefa_ociosa, "Tarefa ociosa", PILHA_TAREFA_OCIOSA, TAM_PILHA_OCIOSA, 0);
	
	/* Configura marca de tempo */
#if 0
    ConfiguraMarcaTempo();   
#endif	
	/* Inicia sistema multitarefas */
	IniciaMultitarefas();
	
	/* Nunca chega aqui */
    return (EXIT_SUCCESS);
	while (1)
	{
	}
}

/* Tarefas de exemplo com alternancia ciclica de contexto (1 -> 2 -> 3 -> 1) */
void tarefa_1(void)
{
	for(;;)
	{
		a++;
		TarefaContinua(2);
		TarefaSuspende(1);
	}
}

void tarefa_2(void)
{
	for(;;)
	{
		b++;
		TarefaContinua(3);	
		TarefaSuspende(2);
	}
}

void tarefa_3(void)
{
	for(;;)
	{
		c++;
		TarefaContinua(1);
		TarefaSuspende(3);
	}
}

/* Tarefas adicionais de exemplo do repositorio */
void tarefa_4(void)
{
	volatile uint16_t cont = 0;
	for(;;)
	{
		cont++;
		TarefaEspera(5);	/* tarefa se coloca em espera por 5 marcas de tempo (ticks) */
	}
}

/* Tarefas de exemplo que usam funcoes de semaforo */
semaforo_t SemaforoTeste = {0,0}; /* declaracao e inicializacao de um semaforo */

void tarefa_5(void)
{
	uint32_t cont = 0;			/* inicializacoes para a tarefa */
	
	for(;;)
	{
		cont++;				/* codigo exemplo da tarefa */
		TarefaEspera(3); 	/* tarefa se coloca em espera por 3 marcas de tempo (ticks) */
		SemaforoLibera(&SemaforoTeste); /* tarefa libera semaforo para tarefa que esta esperando-o */
	}
}

void tarefa_6(void)
{
	uint32_t cont = 0;	    /* inicializacoes para a tarefa */
	
	for(;;)
	{
		cont++; 			/* codigo exemplo da tarefa */
		SemaforoAguarda(&SemaforoTeste); /* tarefa se coloca em espera por semaforo */
	}
}

/* Solucao com buffer compartilhado */
#define TAM_BUFFER 10
uint8_t buffer[TAM_BUFFER]; /* declaracao de um buffer (vetor) ou fila circular */

semaforo_t SemaforoCheio = {0,0}; /* declaracao e inicializacao de um semaforo */
semaforo_t SemaforoVazio = {TAM_BUFFER,0}; /* declaracao e inicializacao de um semaforo */

void tarefa_7(void)
{
	uint8_t cont = 1;			/* inicializacoes para a tarefa */
	uint8_t i = 0;
	
	for(;;)
	{
		SemaforoAguarda(&SemaforoVazio);
		
		buffer[i] = cont++;
		i = (i+1)%TAM_BUFFER;
		
		SemaforoLibera(&SemaforoCheio); /* tarefa libera semaforo para tarefa que esta esperando-o */
		TarefaEspera(10); 	/* tarefa se coloca em espera por 10 marcas de tempo (ticks), equivale a 10ms */		
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
		
		(void)valor;	/* leitura da variavel para evitar aviso (warning) do compilador */
		
		SemaforoLibera(&SemaforoVazio);
	}
}