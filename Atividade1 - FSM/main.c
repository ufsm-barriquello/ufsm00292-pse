#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03
#define MAX_DATA 255

// MÁQUINA DE ESTADOS DO RECEPTOR (RX)
typedef enum {
    RX_WAIT_STX,
    RX_GET_QTD,
    RX_GET_DATA,
    RX_GET_CHK,
    RX_WAIT_ETX
} rx_state_t;

rx_state_t rx_state = RX_WAIT_STX;
uint8_t rx_qtd = 0;
uint8_t rx_data[MAX_DATA];
uint8_t rx_index = 0;
uint8_t rx_calc_chk = 0;
uint8_t rx_received_chk = 0;

void process_rx_byte(uint8_t byte) {
    switch(rx_state) {
        case RX_WAIT_STX:
            if (byte == STX) {
                rx_state = RX_GET_QTD;
                rx_calc_chk = 0; 
            }
            break;

        case RX_GET_QTD:
            rx_qtd = byte;
            rx_calc_chk ^= byte; 
            rx_index = 0;
            if (rx_qtd == 0) {
                rx_state = RX_GET_CHK; 
            } else {
                rx_state = RX_GET_DATA;
            }
            break;

        case RX_GET_DATA:
            rx_data[rx_index++] = byte;
            rx_calc_chk ^= byte; 
            if (rx_index >= rx_qtd) {
                rx_state = RX_GET_CHK;
            }
            break;

        case RX_GET_CHK:
            rx_received_chk = byte;
            rx_state = RX_WAIT_ETX;
            break;

        case RX_WAIT_ETX:
            if (byte == ETX) {
                if (rx_received_chk == rx_calc_chk) {
                }
            }
            rx_state = RX_WAIT_STX;
            break;

        default:
            rx_state = RX_WAIT_STX;
            break;
    }
}

// MÁQUINA DE ESTADOS DO TRANSMISSOR (TX)
typedef enum {
    TX_IDLE,
    TX_SEND_STX,
    TX_SEND_QTD,
    TX_SEND_DATA,
    TX_SEND_CHK,
    TX_SEND_ETX
} tx_state_t;

tx_state_t tx_state = TX_IDLE;
uint8_t tx_data[MAX_DATA];
uint8_t tx_qtd = 0;
uint8_t tx_index = 0;
uint8_t tx_calc_chk = 0;

bool start_transmission(uint8_t *payload, uint8_t length) {
    if (tx_state != TX_IDLE) return false; 

    for(uint8_t i = 0; i < length; i++) {
        tx_data[i] = payload[i];
    }
    tx_qtd = length;
    tx_state = TX_SEND_STX; 

    return true;
}

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
            tx_state = TX_IDLE; 
            break;

        default:
            tx_state = TX_IDLE;
            sending = false;
            break;
    }
    return sending;
}

// FUNÇÃO PRINCIPAL
int main() {
    uint8_t meus_dados[] = {0xA1, 0xB2, 0xC3};
    uint8_t tamanho_dados = sizeof(meus_dados);

    printf("=== Iniciando Teste da FSM ===\n");

    if (start_transmission(meus_dados, tamanho_dados)) {
        uint8_t byte_transmitido;

        while (process_tx_fsm(&byte_transmitido)) {
            printf("TX enviou: 0x%02X \n", byte_transmitido);

            process_rx_byte(byte_transmitido);

            if (rx_state == RX_WAIT_STX && byte_transmitido == ETX) {
                 printf("\n---> RX: Pacote recebido com sucesso e checksum validado!\n");
                 printf("---> RX Dados recebidos: ");
                 for(int i = 0; i < rx_qtd; i++) {
                     printf("0x%02X ", rx_data[i]);
                 }
                 printf("\n");
            }
        }
        printf("\n=== Transmissao Finalizada ===\n");
    } else {
        printf("Erro: Transmissor ocupado.\n");
    }

    return 0;
}