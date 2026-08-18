#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define STX_BYTE 0x02
#define ETX_BYTE 0x03
#define MAX_DADOS 256

typedef enum {
    RX_AGUARDA_STX,
    RX_AGUARDA_QTD_DADOS,
    RX_AGUARDA_DADOS,
    RX_AGUARDA_CHK,
    RX_AGUARDA_ETX,
    RX_PACOTE_VALIDO,
    RX_ERRO
} EstadoRX;

typedef struct {
    EstadoRX estado;
    uint8_t buffer_dados[MAX_DADOS];
    uint8_t qtd_dados;
    uint8_t indice_dados;
    uint8_t chk_calculado;
    uint8_t chk_recebido;
} ProtocoloRX;

void rx_init(ProtocoloRX *rx);
bool rx_processa_byte(ProtocoloRX *rx, uint8_t byte_lido);

size_t tx_monta_pacote(const uint8_t *dados_in, uint8_t qtd, uint8_t *pacote_out);

#endif