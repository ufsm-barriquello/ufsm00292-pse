#include "protocolo.h"
#include <string.h> // Para memcpy
#include <stdio.h>

// --- Funções Auxiliares ---
static uint8_t calcular_checksum(const uint8_t *dados, uint8_t qtd)
{
    uint8_t chk = 0;
    for (uint8_t i = 0; i < qtd; i++)
    {
        chk ^= dados[i];
    }
    return chk;
}

// --- Implementação do Transmissor ---

void protocolo_tx_iniciar(protocolo_tx_t *tx, const uint8_t *dados_para_enviar, uint8_t qtd)
{
    if (!tx || (!dados_para_enviar && qtd > 0) || qtd > MAX_DADOS_SIZE)
    {
        tx->estado = TX_IDLE;
        return;
    }

    tx->qtd_dados = qtd;
    if (qtd > 0)
    {
        memcpy(tx->dados, dados_para_enviar, qtd);
    }
    tx->index_dados = 0;
    tx->checksum = calcular_checksum(tx->dados, tx->qtd_dados);
    tx->estado = TX_STX;
}

bool protocolo_tx_gerar_byte(protocolo_tx_t *tx, uint8_t *byte_gerado)
{
    if (tx->estado == TX_IDLE)
    {
        return false;
    }

    switch (tx->estado)
    {
    case TX_STX:
        *byte_gerado = STX_VALUE;
        tx->estado = TX_QTD;
        break;
    case TX_QTD:
        *byte_gerado = tx->qtd_dados;
        tx->estado = (tx->qtd_dados > 0) ? TX_DADOS : TX_CHK;
        break;
    case TX_DADOS:
        *byte_gerado = tx->dados[tx->index_dados];
        tx->index_dados++;
        if (tx->index_dados >= tx->qtd_dados)
        {
            tx->estado = TX_CHK;
        }
        break;
    case TX_CHK:
        *byte_gerado = tx->checksum;
        tx->estado = TX_ETX;
        break;
    case TX_ETX:
        *byte_gerado = ETX_VALUE;
        tx->estado = TX_IDLE;
        break;
    default:
        return false;
    }
    return true;
}

// --- Implementação do Receptor ---

void protocolo_rx_iniciar(protocolo_rx_t *rx)
{
    if (!rx)
        return;
    rx->estado = RX_IDLE;
    rx->index_dados = 0;
    rx->qtd_dados = 0;
    rx->checksum_calculado = 0;
    rx->pacote_pronto = false;
}

void protocolo_rx_processar_byte(protocolo_rx_t *rx, uint8_t byte_recebido)
{

    printf(">> Estado: %d | Byte: 0x%02X | Idx: %d | Qtd: %d | Chk: 0x%02X\n",
           rx->estado, byte_recebido, rx->index_dados, rx->qtd_dados, rx->checksum_calculado);

    // Se um pacote já está pronto, ignora tudo até a FSM ser reiniciada pela aplicação.
    if (rx->estado == RX_PACOTE_PRONTO)
    {
        return;
    }

    switch (rx->estado)
    {
    case RX_IDLE:
        if (byte_recebido == STX_VALUE)
        {
            rx->pacote_pronto = false;
            rx->index_dados = 0;
            rx->checksum_calculado = 0;
            rx->estado = RX_STX;
        }
        break;

    case RX_STX:
        // O byte recebido neste estado é SEMPRE a quantidade.
        rx->qtd_dados = byte_recebido;
        if (rx->qtd_dados > MAX_DADOS_SIZE)
        {
            protocolo_rx_iniciar(rx); // Erro, quantidade excede o buffer
        }
        else if (rx->qtd_dados == 0)
        {
            rx->estado = RX_CHK;
        }
        else
        {
            rx->estado = RX_QTD;
        }
        break;

    case RX_QTD:
    case RX_DADOS:
        // Um STX aqui indica o início de um novo pacote, abortando o atual.
        if (byte_recebido == STX_VALUE)
        {
            protocolo_rx_iniciar(rx);
            rx->estado = RX_STX; // Começa de novo
            break;
        }
        rx->dados[rx->index_dados] = byte_recebido;
        rx->checksum_calculado ^= byte_recebido;
        rx->index_dados++;
        if (rx->index_dados >= rx->qtd_dados)
        {
            rx->estado = RX_CHK;
        }
        else
        {
            rx->estado = RX_DADOS;
        }
        break;

    case RX_CHK:
        if (byte_recebido == STX_VALUE)
        {
            protocolo_rx_iniciar(rx);
            rx->estado = RX_STX; // Começa de novo
            break;
        }
        rx->checksum_recebido = byte_recebido;
        if (rx->checksum_recebido == rx->checksum_calculado)
        {
            rx->estado = RX_ETX;
        }
        else
        {
            protocolo_rx_iniciar(rx); // Erro de checksum
        }
        break;

    case RX_ETX:
        if (byte_recebido == ETX_VALUE)
        {
            rx->pacote_pronto = true;
            rx->estado = RX_PACOTE_PRONTO;
        }
        else
        {
            protocolo_rx_iniciar(rx);
        }
        break;

    case RX_PACOTE_PRONTO:
        break;
    }
}

bool protocolo_rx_pacote_completo(protocolo_rx_t *rx)
{
    return rx->pacote_pronto;
}