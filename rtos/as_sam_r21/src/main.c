/**
 * \file
 * \brief Atividade 7 - Produtor-Consumidor com Semáforos e Buffer Compartilhado
 */

#include <asf.h>
#include "stdint.h"
#include "rtos.h"

/* Tamanho N do buffer compartilhado */
#define N 10

/*
 * Protótipos das Tarefas
 */
void tarefa_produtor(void);
void tarefa_consumidor(void);

/*
 * Definições e Alocação de Pilhas
 */
#define TAM_PILHA_PRODUTOR   (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_CONSUMIDOR (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_OCIOSA     (TAM_MINIMO_PILHA + 24)

uint32_t PILHA_PRODUTOR[TAM_PILHA_PRODUTOR];
uint32_t PILHA_CONSUMIDOR[TAM_PILHA_CONSUMIDOR];
uint32_t PILHA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Buffer Compartilhado e Índices
 */
uint8_t buffer[N];
static uint8_t f = 0; /* Índice do Produtor */
static uint8_t i = 0; /* Índice do Consumidor */

/*
 * Inicialização dos Semáforos (Mapeado do Slide):
 * semaforo cheio = 0;
 * semaforo vazio = N;
 */
semaforo_t cheio = {0, 0};
semaforo_t vazio = {N, 0};

/*
 * Funções Auxiliares de Produção e Consumo
 */
static uint8_t produz(void)
{
    static uint8_t dado = 1;
    return dado++; /* Gera um dado sequencial */
}

static void consome(uint8_t dado)
{
    (void)dado; /* Leitura do dado consumido */
}

/*
 * Função principal de entrada do sistema
 */
int main(int argc, char** argv)
{
#if 0
    system_init();
#endif	

    /* Criação das Tarefas: Produtor e Consumidor */
    CriaTarefa(tarefa_produtor, "Produtor", PILHA_PRODUTOR, TAM_PILHA_PRODUTOR, 1);
    CriaTarefa(tarefa_consumidor, "Consumidor", PILHA_CONSUMIDOR, TAM_PILHA_CONSUMIDOR, 2);
    
    /* Cria tarefa ociosa do sistema */
    CriaTarefa(tarefa_ociosa, "Tarefa ociosa", PILHA_OCIOSA, TAM_PILHA_OCIOSA, 0);
    
#if 0
    ConfiguraMarcaTempo();   
#endif	

    /* Inicia sistema multitarefas */
    IniciaMultitarefas();
    
    return (EXIT_SUCCESS);
}

/*
 * TAREFA PRODUTOR (Conforme pseudocódigo do slide)
 */
void tarefa_produtor(void)
{
    for (;;)
    {
        SemaforoAguarda(&vazio);     /* wait(vazio); */
        
        f = (f + 1) % N;            /* f = (f + 1) % N; */
        buffer[f] = produz();       /* buffer[f] = produz(); */
        
        SemaforoLibera(&cheio);      /* signal(cheio); */
        
        TarefaEspera(10);           /* Atraso/Delay para permitir troca de contexto */
    }
}

/*
 * TAREFA CONSUMIDOR (Conforme pseudocódigo do slide)
 */
void tarefa_consumidor(void)
{
    for (;;)
    {
        SemaforoAguarda(&cheio);     /* wait(cheio); */
        
        i = (i + 1) % N;            /* i = (i + 1) % N; */
        consome(buffer[i]);         /* consome(buffer[i]); */
        
        SemaforoLibera(&vazio);      /* signal(vazio); */
        
        TarefaEspera(20);           /* Atraso/Delay para simular o tempo de consumo */
    }
}