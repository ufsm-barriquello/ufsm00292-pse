// Enumeração dos estados do transmissor
typedef enum {
    TX_IDLE,
    TX_SEND_STX,
    TX_SEND_QTD,
    TX_SEND_DATA,
    TX_SEND_CHK,
    TX_SEND_ETX
} tx_state_t;

// Variáveis de estado globais/estáticas para o Transmissor
tx_state_t tx_state = TX_IDLE;
uint8_t tx_data[MAX_DATA];
uint8_t tx_qtd = 0;
uint8_t tx_index = 0;
uint8_t tx_calc_chk = 0;

// Função para agendar uma nova transmissão (Interface de usuário)
bool start_transmission(uint8_t *payload, uint8_t length) {
    if (tx_state != TX_IDLE) {
        return false; // Transmissor ocupado
    }

    // Copia os dados para o buffer local
    for(uint8_t i = 0; i < length; i++) {
        tx_data[i] = payload[i];
    }
    tx_qtd = length;
    tx_state = TX_SEND_STX; // Inicia a FSM

    return true;
}

// Máquina de estados executada periodicamente
bool process_tx_fsm(uint8_t *byte_to_send) {
    bool sending = true;

    switch(tx_state) {
        case TX_IDLE:
            sending = false;
            break;

        case TX_SEND_STX:
            *byte_to_send = STX;
            tx_calc_chk = 0;
            tx_state = TX_SEND_QTD;
            break;

        case TX_SEND_QTD:
            *byte_to_send = tx_qtd;
            tx_calc_chk ^= *byte_to_send;
            tx_index = 0;
            if (tx_qtd == 0) {
                tx_state = TX_SEND_CHK;
            } else {
                tx_state = TX_SEND_DATA;
            }
            break;

        case TX_SEND_DATA:
            *byte_to_send = tx_data[tx_index++];
            tx_calc_chk ^= *byte_to_send;
            if (tx_index >= tx_qtd) {
                tx_state = TX_SEND_CHK;
            }
            break;

        case TX_SEND_CHK:
            *byte_to_send = tx_calc_chk;
            tx_state = TX_SEND_ETX;
            break;

        case TX_SEND_ETX:
            *byte_to_send = ETX;
            tx_state = TX_IDLE; // Finaliza transmissão
            break;

        default:
            tx_state = TX_IDLE;
            sending = false;
            break;
    }
    return sending;
}