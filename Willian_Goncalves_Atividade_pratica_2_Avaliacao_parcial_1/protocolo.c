#include "protocolo.h"

void rx_init(ProtocoloRX *rx) {
    if (!rx) return;
    rx->estado = RX_AGUARDA_STX;
    rx->qtd_dados = 0;
    rx->indice_dados = 0;
    rx->chk_calculado = 0;
    rx->chk_recebido = 0;
}

/* FSM do Receptor usando switch-case */
bool rx_processa_byte(ProtocoloRX *rx, uint8_t byte_lido) {
    if (!rx) return false;

    switch (rx->estado) {
        case RX_AGUARDA_STX:
            if (byte_lido == STX_BYTE) {
                rx->chk_calculado = 0;
                rx->indice_dados = 0;
                rx->estado = RX_AGUARDA_QTD_DADOS;
            }
            break;

        case RX_AGUARDA_QTD_DADOS:
            rx->qtd_dados = byte_lido;
            if (rx->qtd_dados == 0) {
                rx->estado = RX_AGUARDA_CHK;
            } else {
                rx->estado = RX_AGUARDA_DADOS;
            }
            break;

        case RX_AGUARDA_DADOS:
            rx->buffer_dados[rx->indice_dados] = byte_lido;
            rx->chk_calculado += byte_lido; // Soma simples de 8 bits
            rx->indice_dados++;

            if (rx->indice_dados >= rx->qtd_dados) {
                rx->estado = RX_AGUARDA_CHK;
            }
            break;

        case RX_AGUARDA_CHK:
            rx->chk_recebido = byte_lido;
            if (rx->chk_recebido == rx->chk_calculado) {
                rx->estado = RX_AGUARDA_ETX;
            } else {
                rx_init(rx); // Checksum inválido: descarta pacote
            }
            break;

        case RX_AGUARDA_ETX:
            if (byte_lido == ETX_BYTE) {
                rx->estado = RX_PACOTE_VALIDO;
                return true; // Pacote recebido com sucesso
            } else {
                rx_init(rx); // ETX inválido
            }
            break;

        case RX_PACOTE_VALIDO:
        case RX_ERRO:
        default:
            rx_init(rx);
            break;
    }

    return false;
}

/* Transmissor: Monta o buffer serializado conforme a FSM TX */
size_t tx_monta_pacote(const uint8_t *dados_in, uint8_t qtd, uint8_t *pacote_out) {
    if (!dados_in || !pacote_out) return 0;

    size_t idx = 0;
    uint8_t chk = 0;

    pacote_out[idx++] = STX_BYTE;
    pacote_out[idx++] = qtd;

    for (uint8_t i = 0; i < qtd; i++) {
        pacote_out[idx++] = dados_in[i];
        chk += dados_in[i];
    }

    pacote_out[idx++] = chk;
    pacote_out[idx++] = ETX_BYTE;

    return idx; // Retorna o tamanho total do pacote
}