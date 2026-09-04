/*
 * protocolo_pt.h - protocolo | STX | QTD_DADOS | DADOS | CHK | ETX |
 *                  tratado por duas protothreads.
 *
 *   STX (1 B) = 0x02, inicio da transmissao
 *   QTD (1 B) = quantidade de bytes de dados (1 a 255; 0 e invalido)
 *   DADOS     = N bytes
 *   CHK (1 B) = soma dos bytes de DADOS (modulo 256)
 *   ETX (1 B) = 0x03, fim da transmissao
 *
 * A receptora interpreta o protocolo e responde ACK quando o quadro chega
 * inteiro e correto. A transmissora envia o quadro e reenvia se o ACK nao
 * chegar dentro do tempo maximo de espera.
 */
#ifndef PROTOCOLO_PT_H
#define PROTOCOLO_PT_H

#include <stdint.h>

#include "pt.h"

#define STX 0x02
#define ETX 0x03
#define ACK 0x06

#define MAX_DADOS      255
#define TIMEOUT_MS     50   /* tempo maximo de espera pelo ACK */
#define MAX_TENTATIVAS 3

/* ---- canal de comunicacao simulado (fila de bytes) ---------------- */

typedef struct {
    uint8_t  buffer[512];
    unsigned entrada;
    unsigned saida;
} canal_t;

extern canal_t canal_ida;     /* transmissora -> receptora */
extern canal_t canal_volta;   /* receptora -> transmissora (ACK) */

extern unsigned tempo_ms;     /* relogio da simulacao */
extern int      verboso;      /* 1 = imprime o que acontece */

void     poe(canal_t *canal, uint8_t byte);
int      tem_byte(const canal_t *canal);
uint8_t  tira(canal_t *canal);
unsigned quantos_bytes(const canal_t *canal);
void     canal_limpa(canal_t *canal);

/* Monta um quadro no canal. Com corrompe != 0 o CHK sai errado de proposito. */
void monta_quadro(canal_t *canal, const uint8_t *dados, uint8_t qtd, int corrompe);

/* ---- receptora ---------------------------------------------------- */

typedef enum {
    RX_NADA,          /* nenhum quadro completo ainda            */
    RX_VALIDO,        /* quadro completo e correto (ACK enviado) */
    RX_QTD_INVALIDA,  /* QTD igual a zero                        */
    RX_CHK_ERRADO,    /* checksum nao confere                    */
    RX_ETX_AUSENTE    /* faltou o ETX no fim                     */
} rx_resultado_t;

extern rx_resultado_t rx_resultado;          /* resultado do ultimo quadro */
extern uint8_t        rx_dados[MAX_DADOS + 1]; /* dados do ultimo quadro (+1 para o terminador) */
extern uint8_t        rx_qtd;

int protothread_receptora(struct pt *pt);

/* ---- transmissora ------------------------------------------------- */

typedef enum {
    TX_ENVIANDO,   /* ainda tentando         */
    TX_ACK,        /* ACK recebido           */
    TX_DESISTIU    /* estourou as tentativas */
} tx_resultado_t;

extern const uint8_t *tx_dados;      /* o que sera enviado          */
extern uint8_t        tx_qtd;
extern int            tx_corrompe;   /* quantos envios saem com CHK errado */
extern int            tx_tentativas; /* tentativas gastas ate agora */
extern tx_resultado_t tx_resultado;

void transmissora_prepara(const uint8_t *dados, uint8_t qtd, int corrompe);
int  protothread_transmissora(struct pt *pt);

#endif
