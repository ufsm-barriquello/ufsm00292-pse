/* FSM do transmissor do protocolo | STX | QTD_DADOS | DADOS | CHK | ETX | */

#include "protocolo.h"

uint8_t protocolo_checksum(uint8_t qtd, const uint8_t *dados)
{
    uint8_t chk = qtd;
    uint8_t i;

    for (i = 0; i < qtd; i++) {
        chk = (uint8_t)(chk + dados[i]);
    }
    return chk;
}

void tx_init(tx_t *tx)
{
    tx->estado = TX_OCIOSO;
    tx->qtd = 0;
    tx->idx = 0;
    tx->chk = 0;
}

int tx_carrega(tx_t *tx, const uint8_t *dados, uint8_t qtd)
{
    uint8_t i;

    if (qtd > PROTO_MAX_DADOS) {
        return 0;
    }

    for (i = 0; i < qtd; i++) {
        tx->dados[i] = dados[i];
    }
    tx->qtd = qtd;
    tx->idx = 0;
    tx->chk = protocolo_checksum(qtd, tx->dados);
    tx->estado = TX_ENVIA_STX;

    return 1;
}

int tx_processa(tx_t *tx, uint8_t *byte)
{
    int enviou = 1;

    switch (tx->estado) {

    case TX_ENVIA_STX:
        *byte = STX;
        tx->estado = TX_ENVIA_QTD;
        break;

    case TX_ENVIA_QTD:
        *byte = tx->qtd;
        /* quadro sem dados vai direto para o checksum */
        tx->estado = (tx->qtd == 0) ? TX_ENVIA_CHK : TX_ENVIA_DADOS;
        break;

    case TX_ENVIA_DADOS:
        *byte = tx->dados[tx->idx];
        tx->idx++;
        if (tx->idx >= tx->qtd) {
            tx->estado = TX_ENVIA_CHK;
        }
        break;

    case TX_ENVIA_CHK:
        *byte = tx->chk;
        tx->estado = TX_ENVIA_ETX;
        break;

    case TX_ENVIA_ETX:
        *byte = ETX;
        tx->estado = TX_OCIOSO;
        break;

    case TX_OCIOSO:
    default:
        tx->estado = TX_OCIOSO;
        enviou = 0;
        break;
    }

    return enviou;
}

int tx_ocupado(const tx_t *tx)
{
    return (tx->estado != TX_OCIOSO);
}
