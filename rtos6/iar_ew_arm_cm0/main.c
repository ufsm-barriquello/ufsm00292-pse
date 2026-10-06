#include <stdint.h>
#include <stdio.h>
#include "rtos.h"

#define TAM_PILHA (TAM_MINIMO_PILHA + 24)

uint32_t P0[TAM_PILHA], P1[TAM_PILHA], P2[TAM_PILHA], P3[TAM_PILHA], P4[TAM_PILHA], P5[TAM_PILHA], P_IDLE[TAM_PILHA];
volatile uint32_t c0 = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0;

void t0(void) { for(;;) { c0++; TarefaSuspende(1); } } 
void t1(void) { for(;;) { c1++; TarefaContinua(1); TarefaSuspende(2); } } 
void t2(void) { for(;;) { c2++; TarefaContinua(2); TarefaSuspende(3); } } 
void t3(void) { for(;;) { c3++; TarefaContinua(3); TarefaSuspende(4); } } 
void t4(void) { for(;;) { c4++; TarefaContinua(4); } } 

void t5(void) {
    for(;;) {
        for(volatile uint32_t i = 0; i < 30000000; i++); 
        printf("Soma: %lu\n", c0 + c1 + c2 + c3 + c4);
    }
}

int main(void) {
    CriaTarefa(t0, "T0", P0, TAM_PILHA, 5); /* ID 1 - Maior prioridade */
    CriaTarefa(t1, "T1", P1, TAM_PILHA, 4); /* ID 2 */
    CriaTarefa(t2, "T2", P2, TAM_PILHA, 3); /* ID 3 */
    CriaTarefa(t3, "T3", P3, TAM_PILHA, 2); /* ID 4 */
    CriaTarefa(t4, "T4", P4, TAM_PILHA, 1); /* ID 5 - Menor prioridade */
    CriaTarefa(t5, "T5", P5, TAM_PILHA, 6); /* ID 6 - Impressão */
    CriaTarefa(tarefa_ociosa, "Idle", P_IDLE, TAM_PILHA, 0);
    
    ConfiguraMarcaTempo();   
    IniciaMultitarefas();
    while(1);
}