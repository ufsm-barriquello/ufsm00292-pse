#ifndef PROTO_COMM_H
#define PROTO_COMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "pt.h"

#define STX_BYTE 0x02
#define ETX_BYTE 0x03
#define ACK_BYTE 0x06
#define NACK_BYTE 0x15
#define MAX_DADOS 256
#define TIMEOUT_TICKS 5

/* Estrutura do Canal de Comunicação Simulado */
typedef struct {
    uint8_t barramento;
    bool novo_byte_disponivel;
    uint8_t canal_ack;          /* Alterado para uint8_t para permitir comparação com ACK_BYTE */
    bool novo_ack_disponivel;
} CanalComunicacao;

/* Contexto da Protothread Transmissora (TX) */
typedef struct {
    struct pt pt;
    uint8_t buffer_envio[MAX_DADOS + 4];
    size_t tamanho_pacote;
    uint8_t timer_ticks;
    bool ack_recebido;
    bool transmissao_concluida;
    int retransmissoes;
} PT_Transmissor;

/* Contexto da Protothread Receptora (RX) */
typedef struct {
    struct pt pt;
    uint8_t buffer_recebido[MAX_DADOS];
    uint8_t qtd_esperada;
    uint8_t idx_dados;
    uint8_t chk_calculado;
    bool pacote_valido;
} PT_Receptor;

void canal_init(CanalComunicacao *canal);
void pt_tx_init(PT_Transmissor *tx, const uint8_t *dados, uint8_t qtd);
void pt_rx_init(PT_Receptor *rx);

char pt_transmissor_thread(PT_Transmissor *tx, CanalComunicacao *canal);
char pt_receptor_thread(PT_Receptor *rx, CanalComunicacao *canal);

#endif /* PROTO_COMM_H */