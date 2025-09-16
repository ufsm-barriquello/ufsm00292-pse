#include "protocolo.h"
#include <string.h> // Para memcpy

// --- Protótipos das Funções de Estado (privadas ao módulo) ---
static bool tx_estado_idle(void *fsm, uint8_t *byte_gerado);
static bool tx_estado_stx(void *fsm, uint8_t *byte_gerado);
static bool tx_estado_qtd(void *fsm, uint8_t *byte_gerado);
static bool tx_estado_dados(void *fsm, uint8_t *byte_gerado);
static bool tx_estado_chk(void *fsm, uint8_t *byte_gerado);
static bool tx_estado_etx(void *fsm, uint8_t *byte_gerado);

static void rx_estado_idle(void *fsm, uint8_t byte_recebido);
static void rx_estado_stx(void *fsm, uint8_t byte_recebido);
static void rx_estado_qtd_e_dados(void *fsm, uint8_t byte_recebido);
static void rx_estado_chk(void *fsm, uint8_t byte_recebido);
static void rx_estado_etx(void *fsm, uint8_t byte_recebido);
static void rx_estado_pacote_pronto(void *fsm, uint8_t byte_recebido);

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
        tx->estado_atual = tx_estado_idle;
        return;
    }
    tx->qtd_dados = qtd;
    if (qtd > 0)
    {
        memcpy(tx->dados, dados_para_enviar, qtd);
    }
    tx->index_dados = 0;
    tx->checksum = calcular_checksum(tx->dados, tx->qtd_dados);
    tx->estado_atual = tx_estado_stx;
}

bool protocolo_tx_gerar_byte(protocolo_tx_t *tx, uint8_t *byte_gerado)
{
    return tx->estado_atual(tx, byte_gerado);
}

static bool tx_estado_idle(void *fsm, uint8_t *byte_gerado)
{
    (void)fsm;
    (void)byte_gerado;
    return false;
}
static bool tx_estado_stx(void *fsm, uint8_t *byte_gerado)
{
    protocolo_tx_t *tx = (protocolo_tx_t *)fsm;
    *byte_gerado = STX_VALUE;
    tx->estado_atual = tx_estado_qtd;
    return true;
}
static bool tx_estado_qtd(void *fsm, uint8_t *byte_gerado)
{
    protocolo_tx_t *tx = (protocolo_tx_t *)fsm;
    *byte_gerado = tx->qtd_dados;
    tx->estado_atual = (tx->qtd_dados > 0) ? tx_estado_dados : tx_estado_chk;
    return true;
}
static bool tx_estado_dados(void *fsm, uint8_t *byte_gerado)
{
    protocolo_tx_t *tx = (protocolo_tx_t *)fsm;
    *byte_gerado = tx->dados[tx->index_dados];
    tx->index_dados++;
    if (tx->index_dados >= tx->qtd_dados)
    {
        tx->estado_atual = tx_estado_chk;
    }
    return true;
}
static bool tx_estado_chk(void *fsm, uint8_t *byte_gerado)
{
    protocolo_tx_t *tx = (protocolo_tx_t *)fsm;
    *byte_gerado = tx->checksum;
    tx->estado_atual = tx_estado_etx;
    return true;
}
static bool tx_estado_etx(void *fsm, uint8_t *byte_gerado)
{
    protocolo_tx_t *tx = (protocolo_tx_t *)fsm;
    *byte_gerado = ETX_VALUE;
    tx->estado_atual = tx_estado_idle;
    return true;
}

// --- Implementação do Receptor ---

void protocolo_rx_iniciar(protocolo_rx_t *rx)
{
    if (!rx)
        return;
    rx->estado_atual = rx_estado_idle;
    rx->index_dados = 0;
    rx->qtd_dados = 0;
    rx->checksum_calculado = 0;
    rx->pacote_pronto = false;
}

void protocolo_rx_processar_byte(protocolo_rx_t *rx, uint8_t byte_recebido)
{
    rx->estado_atual(rx, byte_recebido);
}

bool protocolo_rx_pacote_completo(protocolo_rx_t *rx)
{
    return rx->pacote_pronto;
}

static void rx_estado_idle(void *fsm, uint8_t byte_recebido)
{
    protocolo_rx_t *rx = (protocolo_rx_t *)fsm;
    if (byte_recebido == STX_VALUE)
    {
        rx->pacote_pronto = false;
        rx->index_dados = 0;
        rx->checksum_calculado = 0;
        rx->estado_atual = rx_estado_stx;
    }
}

static void rx_estado_stx(void *fsm, uint8_t byte_recebido)
{
    protocolo_rx_t *rx = (protocolo_rx_t *)fsm;
    rx->qtd_dados = byte_recebido;
    if (rx->qtd_dados > MAX_DADOS_SIZE)
    {
        protocolo_rx_iniciar(rx); // Erro, quantidade excede o buffer
    }
    else if (rx->qtd_dados == 0)
    {
        rx->estado_atual = rx_estado_chk; // Pacote sem dados
    }
    else
    {
        rx->estado_atual = rx_estado_qtd_e_dados;
    }
}

static void rx_estado_qtd_e_dados(void *fsm, uint8_t byte_recebido)
{
    protocolo_rx_t *rx = (protocolo_rx_t *)fsm;
    // Um STX aqui é um erro de enquadramento (pacote interrompido)
    if (byte_recebido == STX_VALUE)
    {
        protocolo_rx_iniciar(rx);
        rx->estado_atual = rx_estado_stx; // Começa a processar o novo pacote
        return;
    }
    rx->dados[rx->index_dados] = byte_recebido;
    rx->checksum_calculado ^= byte_recebido;
    rx->index_dados++;
    if (rx->index_dados >= rx->qtd_dados)
    {
        rx->estado_atual = rx_estado_chk;
    }
}

static void rx_estado_chk(void *fsm, uint8_t byte_recebido)
{
    protocolo_rx_t *rx = (protocolo_rx_t *)fsm;
    if (byte_recebido == STX_VALUE)
    { // Erro de enquadramento
        protocolo_rx_iniciar(rx);
        rx->estado_atual = rx_estado_stx;
        return;
    }
    rx->checksum_recebido = byte_recebido;
    if (rx->checksum_recebido == rx->checksum_calculado)
    {
        rx->estado_atual = rx_estado_etx;
    }
    else
    {
        protocolo_rx_iniciar(rx); // Erro de checksum
    }
}

static void rx_estado_etx(void *fsm, uint8_t byte_recebido)
{
    protocolo_rx_t *rx = (protocolo_rx_t *)fsm;
    if (byte_recebido == ETX_VALUE)
    {
        rx->pacote_pronto = true;
        rx->estado_atual = rx_estado_pacote_pronto; // Entra no estado final
    }
    else
    {
        protocolo_rx_iniciar(rx); // Erro, esperado ETX mas veio outro byte
    }
}

static void rx_estado_pacote_pronto(void *fsm, uint8_t byte_recebido)
{
    // Se um novo pacote começar, processa-o. Senão, ignora.
    if (byte_recebido == STX_VALUE)
    {
        rx_estado_idle(fsm, byte_recebido);
    }
}