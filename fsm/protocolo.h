/*
 * Decodificador do protocolo:
 *   | STX (1 B) | QTD (1 B) | DADOS (N B) | CHK (1 B) | ETX (1 B) |
 *
 * QTD = quantidade de bytes de DADOS
 * CHK = soma dos bytes de DADOS (modulo 256)
 *
 * FSM implementada com ponteiros de funcao e tabela de estados/eventos.
 */
#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <stdint.h>

#define STX       0x02
#define ETX       0x03
#define MAX_DADOS 255

typedef enum {
    AGUARDA_STX,
    RECEBE_QTD,
    RECEBE_DADOS,
    RECEBE_CHK,
    RECEBE_ETX,
    NUM_ESTADOS
} estado_t;

typedef enum {
    EV_STX,
    EV_ETX,
    EV_DADO,
    NUM_EVENTOS
} evento_t;

typedef struct {
    estado_t estado;
    uint8_t  qtd;                 /* QTD recebido                        */
    uint8_t  indice;              /* dados ja guardados                  */
    uint8_t  soma;                /* checksum acumulado                  */
    uint8_t  pronto;              /* 1 = mensagem completa e valida      */
    uint8_t  dados[MAX_DADOS];
} fsm_t;

void fsm_init(fsm_t *fsm);
void fsm_processa(fsm_t *fsm, uint8_t byte);

#endif
