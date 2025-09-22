#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <stdint.h>
#include <stdbool.h>

// --- Constantes do Protocolo ---
#define STX_VALUE 0x02
#define ETX_VALUE 0x03
#define MAX_DADOS_SIZE 16

// --- Tipos de Ponteiros de Função para as Ações dos Estados ---
typedef bool (*tx_action_t)(void *fsm, uint8_t *byte_gerado);
typedef void (*rx_action_t)(void *fsm, uint8_t byte_recebido);

// --- Máquina de Estados do Transmissor (TX) ---
typedef struct
{
    uint8_t dados[MAX_DADOS_SIZE];
    uint8_t qtd_dados;
    uint8_t index_dados;
    uint8_t checksum;
    tx_action_t estado_atual;
} protocolo_tx_t;

void protocolo_tx_iniciar(protocolo_tx_t *tx, const uint8_t *dados_para_enviar, uint8_t qtd);
bool protocolo_tx_gerar_byte(protocolo_tx_t *tx, uint8_t *byte_gerado);

// --- Máquina de Estados do Receptor (RX) ---
typedef struct
{
    uint8_t dados[MAX_DADOS_SIZE];
    uint8_t qtd_dados;
    uint8_t index_dados;
    uint8_t checksum_calculado;
    uint8_t checksum_recebido;
    bool pacote_pronto;
    rx_action_t estado_atual;
} protocolo_rx_t;

void protocolo_rx_iniciar(protocolo_rx_t *rx);
void protocolo_rx_processar_byte(protocolo_rx_t *rx, uint8_t byte_recebido);
bool protocolo_rx_pacote_completo(protocolo_rx_t *rx);

#endif // PROTOCOLO_H