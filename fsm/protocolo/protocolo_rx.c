/* FSM do receptor do protocolo | STX | QTD_DADOS | DADOS | CHK | ETX | */

#include "protocolo.h"

void rx_init(rx_t *rx)
{
    rx->estado = RX_ESPERA_STX;
    rx->qtd = 0;
    rx->idx = 0;
    rx->chk = 0;
}

rx_result_t rx_processa(rx_t *rx, uint8_t byte)
{
    rx_result_t resultado = RX_NADA;

    switch (rx->estado) {

    case RX_ESPERA_STX:
        /* fora de quadro qualquer byte diferente de STX e ruido */
        if (byte == STX) {
            rx->idx = 0;
            rx->chk = 0;
            rx->estado = RX_ESPERA_QTD;
        }
        break;

    case RX_ESPERA_QTD:
        if (byte > PROTO_MAX_DADOS) {
            rx->estado = RX_ESPERA_STX;
            resultado = RX_ERRO_QTD;
        } else {
            rx->qtd = byte;
            rx->chk = byte;
            rx->idx = 0;
            /* quadro sem dados: o proximo byte ja e o checksum */
            rx->estado = (byte == 0) ? RX_ESPERA_CHK : RX_RECEBE_DADOS;
        }
        break;

    case RX_RECEBE_DADOS:
        rx->dados[rx->idx] = byte;
        rx->idx++;
        rx->chk = (uint8_t)(rx->chk + byte);
        if (rx->idx >= rx->qtd) {
            rx->estado = RX_ESPERA_CHK;
        }
        break;

    case RX_ESPERA_CHK:
        if (byte == rx->chk) {
            rx->estado = RX_ESPERA_ETX;
        } else {
            rx->estado = RX_ESPERA_STX;
            resultado = RX_ERRO_CHK;
        }
        break;

    case RX_ESPERA_ETX:
        rx->estado = RX_ESPERA_STX;
        resultado = (byte == ETX) ? RX_QUADRO_OK : RX_ERRO_ETX;
        break;

    default:
        rx->estado = RX_ESPERA_STX;
        break;
    }

    return resultado;
}
