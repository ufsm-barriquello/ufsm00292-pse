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

/* --- ADICIONADO PARA O EXEMPLO PERIÓDICO --- */
void tarefa_periodica(void);
void tarefa_cpu_intensiva(void);


/*
 * Configuracao dos tamanhos das pilhas
 */
#define TAM_PILHA_1         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_2         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_3         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_4         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_5         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_6         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_7         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_8         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_9         (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_10        (TAM_MINIMO_PILHA + 24)

/* --- ADICIONADO PARA O EXEMPLO PERIÓDICO --- */
#define TAM_PILHA_PERIODICA (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_INTENSIVA (TAM_MINIMO_PILHA + 24)

#define TAM_PILHA_OCIOSA    (TAM_MINIMO_PILHA + 24)

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

/* --- ADICIONADO PARA O EXEMPLO PERIÓDICO --- */
uint32_t PILHA_TAREFA_PERIODICA[TAM_PILHA_PERIODICA];
uint32_t PILHA_TAREFA_INTENSIVA[TAM_PILHA_INTENSIVA];

uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Semaforos e variaveis globais
 */
semaforo_t SemaforoTeste = {0,0}; /* declaracao e inicializacao de um semaforo */

#define TAM_BUFFER 10
uint8_t buffer[TAM_BUFFER]; /* declaracao de um buffer (vetor) ou fila circular */
semaforo_t SemaforoCheio = {0,0}; /* declaracao e inicializacao de um semaforo */
semaforo_t SemaforoVazio = {TAM_BUFFER,0}; /* declaracao e inicializacao de um semaforo */

/* Variaveis para o exemplo de mutex (tarefas 9 e 10) */
volatile uint32_t recurso_compartilhado = 0; /* Variavel compartilhada entre tarefas */
semaforo_t MutexRecurso = {1,0}; /* Mutex (semaforo binario com valor inicial 1) para proteger o recurso */

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
    
    /* Tarefas originais comentadas para focar no novo exemplo */
    // CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 2);
    // CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 1);
    // CriaTarefa(tarefa_3, "Tarefa 3", PILHA_TAREFA_3, TAM_PILHA_3, 3);
    // CriaTarefa(tarefa_4, "Tarefa 4", PILHA_TAREFA_4, TAM_PILHA_4, 4);
    // CriaTarefa(tarefa_5, "Tarefa 5", PILHA_TAREFA_5, TAM_PILHA_5, 3);
    // CriaTarefa(tarefa_6, "Tarefa 6", PILHA_TAREFA_6, TAM_PILHA_6, 4);
    // CriaTarefa(tarefa_7, "Produtor", PILHA_TAREFA_7, TAM_PILHA_7, 3);
    // CriaTarefa(tarefa_8, "Consumidor", PILHA_TAREFA_8, TAM_PILHA_8, 4);
    // CriaTarefa(tarefa_9, "Tarefa 9", PILHA_TAREFA_9, TAM_PILHA_9, 3);
    // CriaTarefa(tarefa_10, "Tarefa 10", PILHA_TAREFA_10, TAM_PILHA_10, 3);
    
    /* --- ADICIONADO PARA O EXEMPLO PERIÓDICO --- */
    /*
     * Para este teste, vamos criar duas tarefas:
     * 1. tarefa_periodica: Alta prioridade (4), pisca um LED a cada 100ms.
     * 2. tarefa_cpu_intensiva: Baixa prioridade (1), apenas executa um loop infinito para "gastar" CPU.
     */
    CriaTarefa(tarefa_periodica, "Tarefa Periodica", PILHA_TAREFA_PERIODICA, TAM_PILHA_PERIODICA, 4);
    CriaTarefa(tarefa_cpu_intensiva, "Tarefa Intensiva", PILHA_TAREFA_INTENSIVA, TAM_PILHA_INTENSIVA, 1);
    
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
        port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);    /* Turn LED off. */
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
        TarefaEspera(1000);     /* tarefa 1 se coloca em espera por 1000 marcas de tempo (ticks) */
        
        /* Desliga LED. */
        port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
        TarefaEspera(1000);     /* tarefa 1 se coloca em espera por 1000 marcas de tempo (ticks) */
    }
}

void tarefa_4(void)
{
    volatile uint16_t b = 0;
    for(;;)
    {
        b++;
        TarefaEspera(5);    /* tarefa se coloca em espera por 5 marcas de tempo (ticks) */
    }
}

/* Tarefas de exemplo que usam funcoes de semaforo */
void tarefa_5(void)
{
    uint32_t a = 0;         /* inicializacoes para a tarefa */
    
    for(;;)
    {
        
        a++;                /* codigo exemplo da tarefa */

        TarefaEspera(3);    /* tarefa se coloca em espera por 3 marcas de tempo (ticks) */
        
        SemaforoLibera(&SemaforoTeste); /* tarefa libera semaforo para tarefa que esta esperando-o */
        
    }
}

/* Exemplo de tarefa que usa semaforo */
void tarefa_6(void)
{
    
    uint32_t b = 0;     /* inicializacoes para a tarefa */
    
    for(;;)
    {
        
        b++;            /* codigo exemplo da tarefa */
        
        SemaforoAguarda(&SemaforoTeste); /* tarefa se coloca em espera por semaforo */

    }
}

/* solucao com buffer compartihado (Produtor-Consumidor) */
void tarefa_7(void) /* Produtor */
{

    uint8_t a = 1;          /* inicializacoes para a tarefa */
    uint8_t i = 0;
    
    for(;;)
    {
        SemaforoAguarda(&SemaforoVazio);
        
        buffer[i] = a++;
        i = (i+1)%TAM_BUFFER;
        
        SemaforoLibera(&SemaforoCheio); /* tarefa libera semaforo para tarefa que esta esperando-o */
        
        TarefaEspera(10);   /* tarefa se coloca em espera por 10 marcas de tempo (ticks), equivale a 10ms */         
    }
}

void tarefa_8(void) /* Consumidor */
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

/* tarefa 9 e 10 para demonstrar o conceito de 
disputa por um recurso. */
void tarefa_9(void)
{
    for(;;)
    {
        SemaforoAguarda(&MutexRecurso); 

        recurso_compartilhado++;
        
        SemaforoLibera(&MutexRecurso); 

        TarefaEspera(20);
    }
}

void tarefa_10(void)
{
    for(;;)
    {
        
        SemaforoAguarda(&MutexRecurso); 

        recurso_compartilhado++;

        SemaforoLibera(&MutexRecurso); 
        
        TarefaEspera(35); 
    }
}


/* --- ADICIONADO: TAREFAS PARA COMPARATIVO COOPERATIVO VS. PREEMPTIVO --- */

/**
 * \brief Tarefa periódica que executa a cada 100ms.
 *
 * Esta tarefa tem prioridade alta. Ela alterna o estado de um LED e depois
 * se suspende por 100 marcas de tempo (ticks), o que corresponde a 100ms
 * se o tick do sistema for de 1ms.
 */
void tarefa_periodica(void)
{
    for(;;)
    {
        /* Alterna o estado do LED */
        port_pin_toggle_output_level(LED_0_PIN);
        
        /*
         * Suspende a tarefa por 100ms.
         * Esta chamada é crucial: ela libera o processador para outras tarefas.
         */
        TarefaEspera(100);
    }
}

/**
 * \brief Tarefa de baixa prioridade que consome CPU intensivamente.
 *
 * Esta tarefa simplesmente entra em um loop infinito incrementando uma variável.
 * ELA NÃO POSSUI CHAMADAS DE BLOQUEIO (como TarefaEspera), o que a torna
 * uma tarefa que "não coopera" em um sistema cooperativo.
 */
void tarefa_cpu_intensiva(void)
{
    volatile uint32_t contador_intensivo = 0;
    for(;;)
    {
        contador_intensivo++;
        
        /*
         * Propositalmente, não há nenhuma chamada para TarefaEspera() ou 
         * outra função de bloqueio do RTOS aqui.
         */
    }
}