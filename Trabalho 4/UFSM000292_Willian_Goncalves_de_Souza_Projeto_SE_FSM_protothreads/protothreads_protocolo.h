// ============================================================
// Arquivo: protothreads_protocolo.h
// ============================================================
#ifndef PROTOTHREADS_PROTOCOLO_H
#define PROTOTHREADS_PROTOCOLO_H

#include <stdint.h>
#include <string.h>

#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x15
#define TIMEOUT_TICKS 50
#define MAX_TENTATIVAS 3

#include "pt.h"

typedef struct {
    struct pt pt;
    uint8_t dados[256];
    uint8_t tamanho_dados;
    uint8_t byte_para_enviar;
    uint8_t tem_byte_saida;
    uint8_t ack_ok;
    uint8_t tentativas;
    unsigned long deadline;
} transmissor_t;

typedef struct {
    struct pt pt;
    uint8_t buffer[256];
    uint8_t qtd_esperada;
    uint8_t indice;
    uint8_t checksum_calculado;
    uint8_t checksum_recebido;
    uint8_t pacote_completo;
    uint8_t resposta;
    uint8_t estado;
} receptor_t;

#define CH_SIZE 128

typedef struct {
    uint8_t buf[CH_SIZE];
    int head;
    int tail;
    int count;
} channel_t;

void protocolo_init(transmissor_t *tx, receptor_t *rx);
uint8_t calcular_checksum(const uint8_t *dados, uint8_t tamanho);
int transmissor_thread(transmissor_t *tx, channel_t *canal_tx, channel_t *canal_rx, unsigned long ticks);
int receptor_thread(receptor_t *rx, channel_t *canal_rx, channel_t *canal_tx);

void ch_init(channel_t *ch);
int ch_put(channel_t *ch, uint8_t b);
int ch_get(channel_t *ch, uint8_t *b);

#endif
