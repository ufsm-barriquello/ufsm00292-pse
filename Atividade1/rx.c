#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03
#define MAX_DATA 255

// Enumeração dos estados do receptor
typedef enum {
    RX_WAIT_STX,
    RX_GET_QTD,
    RX_GET_DATA,
    RX_GET_CHK,
    RX_WAIT_ETX
} rx_state_t;

// Variáveis de estado globais/estáticas para o Receptor
rx_state_t rx_state = RX_WAIT_STX;
uint8_t rx_qtd = 0;
uint8_t rx_data[MAX_DATA];
uint8_t rx_index = 0;
uint8_t rx_calc_chk = 0;
uint8_t rx_received_chk = 0;

// Função chamada a cada byte recebido
void process_rx_byte(uint8_t byte) {
    switch(rx_state) {
        case RX_WAIT_STX:
            if (byte == STX) {
                rx_state = RX_GET_QTD;
                rx_calc_chk = 0; // Reinicia o cálculo do checksum
            }
            break;

        case RX_GET_QTD:
            rx_qtd = byte;
            rx_calc_chk ^= byte; // Inclui quantidade no checksum
            rx_index = 0;
            if (rx_qtd == 0) {
                rx_state = RX_GET_CHK; // Pula os dados se não houver nenhum
            } else {
                rx_state = RX_GET_DATA;
            }
            break;

        case RX_GET_DATA:
            rx_data[rx_index++] = byte;
            rx_calc_chk ^= byte; // Adiciona o dado recebido ao checksum
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
            // Retorna ao estado inicial para aguardar o próximo pacote
            rx_state = RX_WAIT_STX;
            break;

        default:
            rx_state = RX_WAIT_STX;
            break;
    }
}