/**
 * \file
 *
 * \brief Exemplos diversos de tarefas e funcionalidades de um sistema operacional multitarefas.
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Este arquivo contem exemplos diversos de tarefas e 
 * funcionalidades de um sistema operacional multitarefas.
 *
 *
 * \par Conteudo
 *
 * -# Inclui funcoes do sistema multitarefas (atraves de multitarefas.h)
 * -# Inicizalizao do processador e do sistema multitarefas
 * -# Criacao de tarefas de demonstracao
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
void tarefa_9(void);
void tarefa_10(void);
void tarefa_11(void);
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
#define TAM_PILHA_9			(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_10		(TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_11		(TAM_MINIMO_PILHA + 24)
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
uint32_t PILHA_TAREFA_9[TAM_PILHA_9];
uint32_t PILHA_TAREFA_10[TAM_PILHA_10];
uint32_t PILHA_TAREFA_11[TAM_PILHA_11];
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Funcao principal de entrada do sistema
 */
int main(void)
{
    
#if 0
	system_init();
#endif
	
	/* Criacao das tarefas */
	/* Parametros: ponteiro, nome, ponteiro da pilha, tamanho da pilha, prioridade da tarefa */
    
	CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 2);
	
	CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 1);
	
	CriaTarefa(tarefa_9, "Monitor", PILHA_TAREFA_9, TAM_PILHA_9, 3);

	CriaTarefa(tarefa_10, "Tarefa 10", PILHA_TAREFA_10, TAM_PILHA_10, 2);

	CriaTarefa(tarefa_11, "Tarefa 11", PILHA_TAREFA_11, TAM_PILHA_11, 1);
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

/* Tarefas de exemplo que usam funcoes para suspender as tarefas por algum tempo (atraso/delay) */
void tarefa_3(void)
{
	volatile uint16_t a = 0;
	for(;;)
	{
		a++;	
			
		/* Liga LED. */
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		TarefaEspera(1000); 	/* tarefa 1 se coloca em espera por 3 marcas de tempo (ticks) */
		
		/* Desliga LED. */
		port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		TarefaEspera(1000); 	/* tarefa 1 se coloca em espera por 3 marcas de tempo (ticks) */
	}
}

void tarefa_4(void)
{
	volatile uint16_t b = 0;
	for(;;)
	{
		b++;
		TarefaEspera(5);	/* tarefa se coloca em espera por 5 marcas de tempo (ticks) */
	}
}

/* Tarefas de exemplo que usam funcoes de semaforo */

semaforo_t SemaforoTeste = {0,0}; /* declaracao e inicializacao de um semaforo */

void tarefa_5(void)
{

	uint32_t a = 0;			/* inicializacoes para a tarefa */
	
	for(;;)
	{
		
		a++;				/* codigo exemplo da tarefa */

		TarefaEspera(3); 	/* tarefa se coloca em espera por 3 marcas de tempo (ticks) */
		
		SemaforoLibera(&SemaforoTeste); /* tarefa libera semaforo para tarefa que esta esperando-o */
		
	}
}

/* Exemplo de tarefa que usa semaforo */
void tarefa_6(void)
{
	
	uint32_t b = 0;	    /* inicializacoes para a tarefa */
	
	for(;;)
	{
		
		b++; 			/* codigo exemplo da tarefa */
		
		SemaforoAguarda(&SemaforoTeste); /* tarefa se coloca em espera por semaforo */

	}
}

/* solucao com buffer compartihado */
/* Tarefas de exemplo que usam funcoes de semaforo */

#define TAM_BUFFER 10
uint8_t buffer[TAM_BUFFER]; /* declaracao de um buffer (vetor) ou fila circular */

semaforo_t SemaforoCheio = {0,0}; /* declaracao e inicializacao de um semaforo */
semaforo_t SemaforoVazio = {TAM_BUFFER,0}; /* declaracao e inicializacao de um semaforo */

void tarefa_7(void)
{

	uint8_t a = 1;			/* inicializacoes para a tarefa */
	uint8_t i = 0;
	
	for(;;)
	{
		SemaforoAguarda(&SemaforoVazio);
		
		buffer[i] = a++;
		i = (i+1)%TAM_BUFFER;
		
		SemaforoLibera(&SemaforoCheio); /* tarefa libera semaforo para tarefa que esta esperando-o */
		
		TarefaEspera(10); 	/* tarefa se coloca em espera por 10 marcas de tempo (ticks), equivale a 10ms */		
	}
}

/* Exemplo de tarefa que usa semaforo */
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
		
		SemaforoLibera(&SemaforoVazio);
	}
}

/* Tarefa 9 - Logger do sistema */
void tarefa_9(void) {
    static uint32_t ciclo = 0;

    for(;;) {
        ciclo++;

        /* log periódico do buffer compartilhado */
        if (ciclo % 200 == 0) {
            printf("Logger: Estado do buffer = [ ");
            for (uint8_t i = 0; i < TAM_BUFFER; i++) {
                printf("%d ", buffer[i]);
            }
            printf("]\r\n");
        }

        /* log do estado dos semáforos */
        if (ciclo % 500 == 0) {
            printf("Logger: SemaforoCheio = %d, SemaforoVazio = %d\r\n",
                   SemaforoCheio.contador, SemaforoVazio.contador);
        }

        /* Heartbeat no LED */
        if (ciclo % 20 == 0) {
            #ifdef LED_0_PIN
            port_pin_toggle_output_level(LED_0_PIN);
            #endif
        }

        TarefaEspera(50); // Executa a cada 50ms
    }
}


/* Tarefa 10: Cooperativa */
void tarefa_10(void)
{
    static uint32_t contador = 0;
    static uint32_t produto = 1;

    for(;;)
    {
        contador++;

        for(uint32_t i = 1; i <= 20; i++) {
            produto = (produto * i) % 100000; 
        }

        printf("Tarefa 10 (Cooperativa) executando, contador = %lu, produto = %lu\r\n", contador, produto);

		TarefaEspera(100); // Espera 100ms
        
    }
}

/*
Só libera a cpu quando chama TarefaEspera(100)
Nenhuma outra tarefa roda durante o loop
Se você aumentar o “trabalho” dentro do loop, 
as tarefas de menor prioridade podem não executar
até a cooperativa liberar a CPU.
*/

/* Tarefa 11: Preemptiva */
void tarefa_11(void)
{
    static uint32_t contador = 0;
    static uint32_t produto = 1;

    for(;;)
    {
        contador++;

        for(uint32_t i = 1; i <= 20; i++) {
            produto = (produto * i) % 100000; 
        }

        printf("Tarefa 11 (Preemptiva) executando, contador = %lu, produto = %lu\r\n", contador, produto);

    }
}
/*
O kernel interrompe a tarefa automaticamente 
para rodar tarefas de maior prioridade.
Mesmo durante o loop de multiplicação, 
se houver uma tarefa pronta com prioridade maior, ela será executada.

*/
