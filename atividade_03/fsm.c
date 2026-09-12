#include "fsm.h"

/* Manipuladores individuais de cada estado */
static void state_stx_handler(StateMachine *sm, uint8_t byte) {
    if (byte == STX_BYTE) {
        sm->indBuffer = 0;
        sm->qtdBuffer = 0;
        sm->chkBuffer = 0;
        sm->packageReceived = false;
        sm->state = ST_QTD;
    }
}

static void state_qtd_handler(StateMachine *sm, uint8_t byte) {
    if (byte > 0 && byte <= MAX_BUFFER) {
        sm->qtdBuffer = byte;
        sm->state = ST_DATA;
    } else {
        sm->state = ST_STX; // Tamanho inválido, descarta
    }
}

static void state_data_handler(StateMachine *sm, uint8_t byte) {
    sm->buffer[sm->indBuffer++] = byte;
    sm->chkBuffer += byte; // Soma simples de 8 bits
    if (--sm->qtdBuffer == 0) {
        sm->state = ST_CHK;
    }
}

static void state_chk_handler(StateMachine *sm, uint8_t byte) {
    if (byte == sm->chkBuffer) {
        sm->state = ST_ETX;
    } else {
        sm->state = ST_STX; // Checksum falhou, descarta
    }
}

static void state_etx_handler(StateMachine *sm, uint8_t byte) {
    if (byte == ETX_BYTE) {
        sm->packageReceived = true;
    }
    sm->state = ST_STX; // Retorna ao início para o próximo frame
}

/* Inicialização da FSM e vinculação da tabela de ponteiros */
void fsm_init(StateMachine *sm) {
    sm->state = ST_STX;
    sm->indBuffer = 0;
    sm->qtdBuffer = 0;
    sm->chkBuffer = 0;
    sm->packageReceived = false;

    /* Configuração da Tabela de Ponteiros de Função */
    sm->actions[ST_STX]  = state_stx_handler;
    sm->actions[ST_QTD]  = state_qtd_handler;
    sm->actions[ST_DATA] = state_data_handler;
    sm->actions[ST_CHK]  = state_chk_handler;
    sm->actions[ST_ETX]  = state_etx_handler;
}

/* Execução da FSM via tabela de ponteiros */
void fsm_process_byte(StateMachine *sm, uint8_t byte) {
    if (sm->state < NUM_STATES && sm->actions[sm->state] != 0) {
        sm->actions[sm->state](sm, byte);
    }
}

void fsm_process_buffer(StateMachine *sm, const uint8_t *data, int len) {
    for (int i = 0; i < len; i++) {
        fsm_process_byte(sm, data[i]);
    }
}