#include <stdio.h>
#include "fsm_decoder.h"

// Definição dos estados da máquina [cite: 59]
typedef enum {
    STATE_STX = 0,
    STATE_QTD,
    STATE_DATA,
    STATE_CHK,
    STATE_ETX
} fsm_state_t;

// Estrutura para manter o contexto da FSM [cite: 116]
typedef struct {
    fsm_state_t current_state;
    unsigned char buffer[MAX_BUFFER];
    int data_len;
    int data_idx;
    unsigned char calculated_chk;
    packet_handler_t on_packet_ok;
    error_handler_t on_error;
} fsm_context_t;

static fsm_context_t fsm;

// Protótipo para o processador de byte
typedef void (*state_handler_t)(unsigned char byte);

// Funções de tratamento para cada estado
static void handle_stx(unsigned char byte);
static void handle_qtd(unsigned char byte);
static void handle_data(unsigned char byte);
static void handle_chk(unsigned char byte);
static void handle_etx(unsigned char byte);

// Tabela de estados (array de ponteiros de função) [cite: 115]
static state_handler_t state_table[] = {
    [STATE_STX]  = handle_stx,
    [STATE_QTD]  = handle_qtd,
    [STATE_DATA] = handle_data,
    [STATE_CHK]  = handle_chk,
    [STATE_ETX]  = handle_etx
};

// Reinicia a máquina para o estado inicial
static void reset_fsm() {
    fsm.current_state = STATE_STX;
    fsm.data_len = 0;
    fsm.data_idx = 0;
    fsm.calculated_chk = 0;
}

// Implementação dos handlers de estado

static void handle_stx(unsigned char byte) {
    if (byte == STX) {
        reset_fsm(); // Garante que a FSM está limpa
        fsm.current_state = STATE_QTD;
    }
}

static void handle_qtd(unsigned char byte) {
    if (byte > 0 && byte < MAX_BUFFER) {
        fsm.data_len = byte;
        fsm.current_state = STATE_DATA;
    } else {
        if (fsm.on_error) fsm.on_error();
        reset_fsm();
    }
}

static void handle_data(unsigned char byte) {
    fsm.buffer[fsm.data_idx++] = byte;
    fsm.calculated_chk ^= byte; // Cálculo do checksum XOR
    if (fsm.data_idx == fsm.data_len) {
        fsm.current_state = STATE_CHK;
    }
}

static void handle_chk(unsigned char byte) {
    if (byte == fsm.calculated_chk) {
        fsm.current_state = STATE_ETX;
    } else {
        if (fsm.on_error) fsm.on_error();
        reset_fsm();
    }
}

static void handle_etx(unsigned char byte) {
    if (byte == ETX) {
        if (fsm.on_packet_ok) {
            fsm.on_packet_ok(fsm.buffer, fsm.data_len);
        }
    } else {
        if (fsm.on_error) fsm.on_error();
    }
    reset_fsm(); // Retorna ao estado inicial para o próximo pacote
}

// Funções da interface pública

void fsm_init(packet_handler_t on_packet_ok, error_handler_t on_error) {
    fsm.on_packet_ok = on_packet_ok;
    fsm.on_error = on_error;
    reset_fsm();
}

void fsm_process_data(unsigned char *data, int len) {
    for (int i = 0; i < len; ++i) {
        // Executa a função do estado atual passando o byte [cite: 159]
        state_table[fsm.current_state](data[i]);
    }
}
