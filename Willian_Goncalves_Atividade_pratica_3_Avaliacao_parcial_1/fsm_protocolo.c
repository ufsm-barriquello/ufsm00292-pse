#include "fsm_protocolo.h"

/* Forward declarations das funções mapeadas por ponteiros */
static State action_stx(FSMDecoder *fsm, uint8_t byte);
static State action_qtd(FSMDecoder *fsm, uint8_t byte);
static State action_dados(FSMDecoder *fsm, uint8_t byte);
static State action_chk(FSMDecoder *fsm, uint8_t byte);
static State action_etx(FSMDecoder *fsm, uint8_t byte);

/* Tabela de Transição de Estados baseada em Ponteiros de Função */
static const StateTransition state_table[] = {
    { STATE_STX,   EVENT_BYTE_RX, action_stx   },
    { STATE_QTD,   EVENT_BYTE_RX, action_qtd   },
    { STATE_DADOS, EVENT_BYTE_RX, action_dados },
    { STATE_CHK,   EVENT_BYTE_RX, action_chk   },
    { STATE_ETX,   EVENT_BYTE_RX, action_etx   }
};

void fsm_init(FSMDecoder *fsm) {
    if (!fsm) return;
    fsm->current_state = STATE_STX;
    fsm->qtd_dados = 0;
    fsm->indice_dados = 0;
    fsm->chk_calculado = 0;
    fsm->chk_recebido = 0;
}

static State action_stx(FSMDecoder *fsm, uint8_t byte) {
    if (byte == STX_BYTE) {
        fsm->chk_calculado = 0;
        fsm->indice_dados = 0;
        return STATE_QTD;
    }
    return STATE_STX;
}

static State action_qtd(FSMDecoder *fsm, uint8_t byte) {
    fsm->qtd_dados = byte;
    if (fsm->qtd_dados == 0) {
        return STATE_CHK;
    }
    return STATE_DADOS;
}

static State action_dados(FSMDecoder *fsm, uint8_t byte) {
    fsm->buffer_dados[fsm->indice_dados] = byte;
    fsm->chk_calculado += byte;
    fsm->indice_dados++;

    if (fsm->indice_dados >= fsm->qtd_dados) {
        return STATE_CHK;
    }
    return STATE_DADOS;
}

static State action_chk(FSMDecoder *fsm, uint8_t byte) {
    fsm->chk_recebido = byte;
    if (fsm->chk_recebido == fsm->chk_calculado) {
        return STATE_ETX;
    }
    fsm_init(fsm); // Falha de Checksum -> Reseta FSM
    return STATE_STX;
}

static State action_etx(FSMDecoder *fsm, uint8_t byte) {
    if (byte == ETX_BYTE) {
        return STATE_VALIDO;
    }
    fsm_init(fsm); // Byte final inválido
    return STATE_STX;
}

/* Processador de Eventos que consulta a Tabela e invoca o Ponteiro de Função */
bool fsm_process_byte(FSMDecoder *fsm, uint8_t byte) {
    if (!fsm) return false;

    size_t table_size = sizeof(state_table) / sizeof(state_table[0]);

    for (size_t i = 0; i < table_size; i++) {
        if (state_table[i].current_state == fsm->current_state && 
            state_table[i].event == EVENT_BYTE_RX) {
            
            // Executa a ação via Ponteiro de Função
            fsm->current_state = state_table[i].action(fsm, byte);
            break;
        }
    }

    if (fsm->current_state == STATE_VALIDO) {
        return true; // Pacote recebido com sucesso
    }

    return false;
}

/* Transmissor (TX) */
size_t tx_monta_pacote(const uint8_t *dados, uint8_t qtd, uint8_t *pacote_out) {
    if (!dados || !pacote_out) return 0;

    size_t idx = 0;
    uint8_t chk = 0;

    pacote_out[idx++] = STX_BYTE;
    pacote_out[idx++] = qtd;

    for (uint8_t i = 0; i < qtd; i++) {
        pacote_out[idx++] = dados[i];
        chk += dados[i];
    }

    pacote_out[idx++] = chk;
    pacote_out[idx++] = ETX_BYTE;

    return idx;
}