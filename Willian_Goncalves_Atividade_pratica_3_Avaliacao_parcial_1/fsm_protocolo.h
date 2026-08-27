#ifndef FSM_PROTOCOLO_H
#define FSM_PROTOCOLO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define STX_BYTE 0x02
#define ETX_BYTE 0x03
#define MAX_DADOS 256

/* Estados do Decodificador */
typedef enum {
    STATE_STX = 0,
    STATE_QTD,
    STATE_DADOS,
    STATE_CHK,
    STATE_ETX,
    STATE_VALIDO,
    STATE_MAX
} State;

/* Eventos de Entrada */
typedef enum {
    EVENT_BYTE_RX,
    EVENT_TIMEOUT,
    EVENT_MAX
} Event;

struct FSMDecoder;

/* Tipo Ponteiro de Função para Ação de Estado */
typedef State (*StateActionFn)(struct FSMDecoder *fsm, uint8_t byte);

/* Contexto do Decodificador */
typedef struct FSMDecoder {
    State current_state;
    uint8_t buffer_dados[MAX_DADOS];
    uint8_t qtd_dados;
    uint8_t indice_dados;
    uint8_t chk_calculado;
    uint8_t chk_recebido;
} FSMDecoder;

/* Estrutura da Tabela de Estados/Eventos */
typedef struct {
    State current_state;
    Event event;
    StateActionFn action;
} StateTransition;

void fsm_init(FSMDecoder *fsm);
bool fsm_process_byte(FSMDecoder *fsm, uint8_t byte);

/* Função do Transmissor (TX) */
size_t tx_monta_pacote(const uint8_t *dados, uint8_t qtd, uint8_t *pacote_out);

#endif /* FSM_PROTOCOLO_H */