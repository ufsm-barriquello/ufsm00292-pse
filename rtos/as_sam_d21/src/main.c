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

/* === NOVAS TAREFAS === */
void tarefa_periodica_100ms(void);
void tarefa_trabalho_pesado(void);

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
#define TAM_PILHA_PERIODICA (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_PESADA    (TAM_MINIMO_PILHA + 24)
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
uint32_t PILHA_TAREFA_PERIODICA[TAM_PILHA_PERIODICA];
uint32_t PILHA_TAREFA_PESADA[TAM_PILHA_PESADA];
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/* Variáveis para teste e medição */
#define PERIODIC_MS 100u
volatile uint32_t periodic_toggle_count = 0;
volatile uint32_t periodic_jitter_max_ms = 0;

/*
 * Funcao principal de entrada do sistema
 */
int main(void)
{   
#if 0
    system_init();
#endif

    /* Criacao das tarefas */
    CriaTarefa(tarefa_1, "Tarefa 1", PILHA_TAREFA_1, TAM_PILHA_1, 2);
    CriaTarefa(tarefa_2, "Tarefa 2", PILHA_TAREFA_2, TAM_PILHA_2, 1);
    CriaTarefa(tarefa_9, "Tarefa 9", PILHA_TAREFA_9, TAM_PILHA_9, 1);
    CriaTarefa(tarefa_10, "Tarefa 10", PILHA_TAREFA_10, TAM_PILHA_10, 1);

    /* NOVAS TAREFAS: preempitiva e carga pesada (cooperativa) */
    CriaTarefa(tarefa_periodica_100ms, "Periodica100ms", 
               PILHA_TAREFA_PERIODICA, TAM_PILHA_PERIODICA, 3);
    CriaTarefa(tarefa_trabalho_pesado, "TrabalhoPesado",
               PILHA_TAREFA_PESADA, TAM_PILHA_PESADA, 2);

    /* Cria tarefa ociosa do sistema */
    CriaTarefa(tarefa_ociosa,"Tarefa ociosa", PILHA_TAREFA_OCIOSA, TAM_PILHA_OCIOSA, 0);

    /* Configura marca de tempo */
    ConfiguraMarcaTempo();

    /* Inicia sistema multitarefas */
    IniciaMultitarefas();

    /* Nunca chega aqui */
    while (1) {}
}

/* === TAREFAS EXISTENTES === */
void tarefa_1(void)
{
    volatile uint16_t a = 0;
    for(;;)
    {
        a++;
        port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
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
        port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
    }
}

void tarefa_3(void)
{
    volatile uint16_t a = 0;
    for(;;)
    {
        a++;   
        port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
        TarefaEspera(1000);
        port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
        TarefaEspera(1000);
    }
}

void tarefa_4(void)
{
    volatile uint16_t b = 0;
    for(;;)
    {
        b++;
        TarefaEspera(5);
    }
}

/* semaforo exemplo */
semaforo_t SemaforoTeste = {0,0};

void tarefa_5(void)
{
    uint32_t a = 0;
    for(;;)
    {
        a++;
        TarefaEspera(3);
        SemaforoLibera(&SemaforoTeste);
    }
}

void tarefa_6(void)
{
    uint32_t b = 0;
    for(;;)
    {
        b++;
        SemaforoAguarda(&SemaforoTeste);
    }
}

#define TAM_BUFFER 10
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
        do {
            REG_ATOMICA_INICIO();
                contador = SemaforoCheio.contador;
            REG_ATOMICA_FIM();
            if (contador == 0)
                TarefaEspera(100);
        } while (!contador);

        SemaforoAguarda(&SemaforoCheio);
        valor = buffer[f];
        f = (f+1)%TAM_BUFFER;
        SemaforoLibera(&SemaforoVazio);
    }
}

/* === NOVAS TAREFAS === */

/* Tarefa preempitiva de 100ms */
void tarefa_periodica_100ms(void)
{
    uint32_t t_antigo = 0;
#ifdef MarcaTempoMillis
    t_antigo = MarcaTempoMillis();
#endif

    for(;;)
    {
        port_pin_toggle_output_level(LED_0_PIN);
        periodic_toggle_count++;

#ifdef MarcaTempoMillis
        {
            uint32_t t_agora = MarcaTempoMillis();
            uint32_t delta = (t_agora >= t_antigo) ? (t_agora - t_antigo) : (t_antigo - t_agora);
            if (delta > periodic_jitter_max_ms)
                periodic_jitter_max_ms = delta;
            t_antigo = t_agora;
        }
#endif
        TarefaEspera(PERIODIC_MS);
    }
}

/* Tarefa de carga que ocupa CPU (cooperativa)*/
void tarefa_trabalho_pesado(void)
{
    for(;;)
    {
        volatile uint32_t i;
        for (i = 0; i < 2000000; i++)
        {
            __asm__ volatile("nop");
        }
        /* opcional: comente para bloquear cooperativo */
        TarefaEspera(10);
    }
}

/* tarefas extra */
void tarefa_9(void)
{
    for(;;)
    {
        port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
        TarefaEspera(200);
        port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
        TarefaEspera(800);
    }
}

void tarefa_10(void)
{
    static uint8_t dado = 100;
    static uint8_t i = 0;
    for(;;)
    {
        SemaforoAguarda(&SemaforoVazio);
        buffer[i] = dado++;
        i = (i+1)%TAM_BUFFER;
        SemaforoLibera(&SemaforoCheio);
        TarefaEspera(50);
    }
}
