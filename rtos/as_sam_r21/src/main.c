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
...
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
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Declaracao de semaforos e variaveis auxiliares para os testes
 */
semaforo_t SemaforoTeste = {0,0}; /* declaracao e inicializacao de um semaforo */

/* Tarefas de produtor/consumidor usando semaforos */
#define TAM_BUFFER			16

uint8_t buffer[TAM_BUFFER];
semaforo_t SemaforoCheio = {0,0}; /* declaracao e inicializacao de um semaforo */
semaforo_t SemaforoVazio = {TAM_BUFFER,0}; /* declaracao e inicializacao de um semaforo */

volatile uint8_t contador = 0;

/*
 * Funcao principal
 */
int main(void)
{
	system_init();
	delay_init();
	board_init();
	ioport_init();
	
	/* Inicializa LED como saida */
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_0_PIN, &pin_conf);
	port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);

	/* Cria tarefas de exemplo — ative/desative conforme desejar */
	CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 2);
	
	//CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 1);
	
    //CriaTarefa(tarefa_4,"Tarefa 4",PILHA_TAREFA_4,TAM_PILHA_4,2);
    
    /* Mantive sua ordem: Tarefa 9 e, em seguida, a Tarefa 10 */
    CriaTarefa(tarefa_9,"Tarefa 9",PILHA_TAREFA_9,TAM_PILHA_9,3);
    CriaTarefa(tarefa_10,"Tarefa 10",PILHA_TAREFA_10,TAM_PILHA_10,2);
    
	/* Cria tarefa ociosa do sistema */
	CriaTarefa(tarefa_ociosa,"Tarefa ociosa", PILHA_TAREFA_OCIOSA, TAM_PILHA_OCIOSA, 0);
	
	/* Habilita interrupcoes globais no processador */
	sei();
	
	/* Inicia o escalonador */
	IniciaRTOS();
    
	/* O codigo nao devera alcancar este ponto */
	while(1);
}

/* Tarefas de exemplo que usam funcoes para ligar/desligar o LED de acordo com a alternancia entre as tarefas */
void tarefa_1(void)
{
	volatile uint16_t a = 0;
	for(;;)
	{
		a++;
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE); /* Liga LED. */
		TarefaEspera(1000);									/* tarefa se coloca em espera por 1000 marcas de tempo (ticks) */
		port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);/* Desliga LED. */
		TarefaEspera(1000);
	}
}

/* Exemplo de tarefa que usa yield (sem esperar ticks) */
void tarefa_2(void)
{
	volatile int a = 0;
	for(;;)
	{
		a++;			/* codigo exemplo da tarefa */
		
		TarefaEspera(0);	/* tarefa se coloca em espera sem delongas (equivalente a yield) */
	
		a++;			/* codigo exemplo da tarefa */
	}
}

/* Exemplo de tarefa executando operacoes que demandam mais tempo (simulando funcao demorada) */
void tarefa_3(void)
{
	volatile uint32_t a = 0;
	for(;;)
	{
		a++;				/* codigo exemplo da tarefa */
		{
			volatile uint32_t b;
			for(b=0;b<20000;b++);/* simula tarefa demorada */
		}
		a++;				/* codigo exemplo da tarefa */
	}
}

/* Exemplo de tarefa que usa contador (busy-wait simples) */
void tarefa_4(void)
{
	volatile uint16_t a = 0;
	for(;;)
	{
		a++;
	}
}

/* Exemplo de tarefa que espera por determinada condicao com timeout */
void tarefa_5(void)
{
	uint16_t a = 0;
	uint16_t b = 0;
	for(;;)
	{
		a++;			/* codigo exemplo da tarefa */

		/* espera ate a variavel b atingir zero. Se nao ocorrer, espera 5 ticks e tenta novamente */
		do
		{
			/* a leitura de b deve ser atomica */
			REG_ATOMICA_INICIO();
			if (b) b--;
			REG_ATOMICA_FIM();
			
			TarefaEspera(5);
		}
		while(b);
		
		a++;				/* codigo exemplo da tarefa */

		TarefaEspera(3); 	/* tarefa se coloca em espera por 3 marcas de tempo (ticks) */
		
		SemaforoLibera(&SemaforoTeste); /* tarefa libera semaforo para tarefa que esta esperando-o */
		
	}
}

/* Exemplo de tarefa que usa semaforo */
void tarefa_6(void)
{
	volatile uint16_t a = 0;
	for(;;)
	{
		a++;

		/* aguarda semaforo ser liberado (por outra tarefa) */
		SemaforoAguarda(&SemaforoTeste);

		a++;
	}
}

/* PRODUTOR: escreve no buffer circular, controlado por semaforos Vazio/Cheio */
void tarefa_7(void)
{

	uint8_t a = 1;			/* inicializacoes para a tarefa */
	uint8_t i = 0;
	
	for(;;)
	{
		SemaforoAguarda(&SemaforoVazio);
		
		buffer[i] = a++;
		i = (i+1) % TAM_BUFFER;
		
		SemaforoLibera(&SemaforoCheio);
	}
}

/* CONSUMIDOR: le do buffer circular */
void tarefa_8(void)
{	
	uint8_t i = 0;
	uint8_t dado;
	
	for(;;)
	{
		SemaforoAguarda(&SemaforoCheio);
		
		dado = buffer[i];
		i = (i+1) % TAM_BUFFER;

		(void)dado; /* use 'dado' conforme sua necessidade (ex.: enviar via UART) */
		
		SemaforoLibera(&SemaforoVazio);
	}
}

/* Tarefa simples (mantive como estava no seu arquivo) */
void tarefa_9(void){

    volatile uint16_t cont=0;
    
    for(;;){
    
        cont++;
        TarefaEspera(500);
    }

}

/* NOVA TAREFA 10: periodica de 100 ms (para comparacao coop x preemptivo) */
void tarefa_10(void)
{
    /* Tarefa periodica de 100 ms para comparar modos cooperativo x preemptivo */
    uint8_t led_on = 0;

    /* Caso necessario, configure o pino do LED como saida aqui */

    for (;;)
    {
        /* Alterna LED */
        led_on ^= 1u;
        port_pin_set_output_level(LED_0_PIN, led_on ? LED_0_ACTIVE : !LED_0_ACTIVE);

        /* Aguarda exatamente 100 ms (assumindo 1 ms por tick) */
        TarefaEspera(100);
    }
}

/* Tarefa ociosa do sistema */
void tarefa_ociosa(void)
{
	for(;;)
	{
		
}
