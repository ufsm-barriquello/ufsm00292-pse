#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <stdbool.h>

#define STX_BYTE     0x02
#define ETX_BYTE     0x03
#define MAX_BUFFER   256

/* Estados da FSM */
typedef enum {
    ST_STX = 0,
    ST_QTD,
    ST_DATA,
    ST_CHK,
    ST_ETX,
    NUM_STATES
} State;

/* Declaração antecipada da estrutura */
typedef struct StateMachine StateMachine;

/* Assinatura do ponteiro de função para ação de estado */
typedef void (*StateAction)(StateMachine *sm, uint8_t byte);

/* Estrutura de contexto da FSM */
struct StateMachine {
    State state;
    uint8_t buffer[MAX_BUFFER];
    uint8_t chkBuffer;
    int indBuffer;
    int qtdBuffer;
    bool packageReceived;
    StateAction actions[NUM_STATES]; /* Tabela de ponteiros de função */
};

/* Funções de interface pública */
void fsm_init(StateMachine *sm);
void fsm_process_byte(StateMachine *sm, uint8_t byte);
void fsm_process_buffer(StateMachine *sm, const uint8_t *data, int len);

#endif // FSM_H