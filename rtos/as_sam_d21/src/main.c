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
void tarefa_heartbeat(void);
void tarefa_periodica_100ms(void);

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
#define TAM_PILHA_HEARTBEAT	(TAM_MINIMO_PILHA + 32)
#define TAM_PILHA_PERIODICA	(TAM_MINIMO_PILHA + 40)
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
uint32_t PILHA_TAREFA_HEARTBEAT[TAM_PILHA_HEARTBEAT];
uint32_t PILHA_TAREFA_PERIODICA[TAM_PILHA_PERIODICA];
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
	
	CriaTarefa(tarefa_3, "Tarefa 3", PILHA_TAREFA_3, TAM_PILHA_3, 3);
	
	CriaTarefa(tarefa_heartbeat, "Heartbeat", PILHA_TAREFA_HEARTBEAT, TAM_PILHA_HEARTBEAT, 1);
	
	CriaTarefa(tarefa_periodica_100ms, "Periodica 100ms", PILHA_TAREFA_PERIODICA, TAM_PILHA_PERIODICA, 2);
	
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

/* Nova tarefa customizada - Heartbeat Task */
/* Demonstra monitoramento de sistema e contagem de batimentos */
void tarefa_heartbeat(void)
{
	static uint32_t heartbeat_counter = 0;
	static uint8_t led_state = 0;
	
	for(;;)
	{
		heartbeat_counter++;
		
		/* Alterna estado do LED para mostrar atividade */
		led_state = !led_state;
		port_pin_set_output_level(LED_0_PIN, led_state ? LED_0_ACTIVE : !LED_0_ACTIVE);
		
		/* Implementa um pattern de heartbeat: 2 piscadas rapidas, pausa longa */
		if (heartbeat_counter % 4 == 1 || heartbeat_counter % 4 == 2) {
			/* Piscadas rapidas */
			TarefaEspera(100);  /* 100ms */
		} else {
			/* Pausa longa entre batimentos */
			TarefaEspera(800);  /* 800ms */
		}
		
		/* A cada 50 heartbeats, faz uma pausa mais longa (simula checagem de sistema) */
		if (heartbeat_counter % 50 == 0) {
			TarefaEspera(2000); /* Pausa de 2 segundos para "system check" */
		}
	}
}

/* Tarefa Periodica - Executa a cada 100ms */
/* Demonstra execucao periodica em modos cooperativo e preemptivo */
void tarefa_periodica_100ms(void)
{
	static uint32_t contador_execucoes = 0;
	static uint32_t tempo_total_ms = 0;
	
	for(;;)
	{
		contador_execucoes++;
		tempo_total_ms += 100; /* Incrementa o tempo total em 100ms a cada execucao */
		
		/* Simulacao de processamento periodico */
		/* Pode ser usado para: leitura de sensores, atualizacao de controles, etc. */
		
		/* Exemplo: toggle de um indicador visual a cada 10 execucoes (1 segundo) */
		if (contador_execucoes % 10 == 0) {
			/* A cada 1 segundo (10 * 100ms), faz algo especial */
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			TarefaEspera(50); /* Liga LED por 50ms */
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
		
		/* Espera 100ms ate a proxima execucao (100 ticks a 1ms cada) */
		/* Funciona tanto em modo cooperativo quanto preemptivo */
		TarefaEspera(100);
	}
}
