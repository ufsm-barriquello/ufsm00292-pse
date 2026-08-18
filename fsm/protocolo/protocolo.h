#ifndef PROTOCOLO_H
#define PROTOCOLO_H

/*
 * Protocolo: | STX | QTD_DADOS | DADOS | CHK | ETX |
 *
 * STX       (1 byte)  -> inicio da transmissao (0x02)
 * QTD_DADOS (1 byte)  -> quantidade de bytes de dados
 * DADOS     (N bytes) -> dados
 * CHK       (1 byte)  -> checksum da transmissao
 * ETX       (1 byte)  -> fim da transmissao (0x03)
 *
 * Checksum adotado: soma modulo 256 do campo QTD_DADOS com todos os
 * bytes de DADOS.
 */

#include <stdint.h>

#define STX 0x02
#define ETX 0x03

/* tamanho maximo do campo DADOS aceito por esta implementacao */
#define PROTO_MAX_DADOS 64

/* calcula o checksum de um quadro (QTD_DADOS + soma dos DADOS) */
uint8_t protocolo_checksum(uint8_t qtd, const uint8_t *dados);

/* ------------------------------------------------------------------ */
/* Transmissor                                                        */
/* ------------------------------------------------------------------ */

typedef enum {
    TX_OCIOSO,        /* sem quadro para enviar                       */
    TX_ENVIA_STX,     /* proximo byte a sair e o STX                  */
    TX_ENVIA_QTD,     /* proximo byte a sair e o QTD_DADOS            */
    TX_ENVIA_DADOS,   /* enviando o campo DADOS                       */
    TX_ENVIA_CHK,     /* proximo byte a sair e o checksum             */
    TX_ENVIA_ETX      /* proximo byte a sair e o ETX                  */
} tx_estado_t;

typedef struct {
    tx_estado_t estado;
    uint8_t qtd;                     /* quantidade de dados do quadro */
    uint8_t idx;                     /* indice do dado sendo enviado  */
    uint8_t chk;                     /* checksum calculado do quadro  */
    uint8_t dados[PROTO_MAX_DADOS];  /* copia dos dados a transmitir  */
} tx_t;

/* coloca o transmissor no estado ocioso */
void tx_init(tx_t *tx);

/* carrega um quadro para transmissao.
 * retorna 1 em caso de sucesso e 0 se qtd > PROTO_MAX_DADOS. */
int tx_carrega(tx_t *tx, const uint8_t *dados, uint8_t qtd);

/* executa um passo da FSM do transmissor.
 * retorna 1 e escreve em *byte o proximo byte do quadro;
 * retorna 0 quando nao ha nada a transmitir (estado TX_OCIOSO). */
int tx_processa(tx_t *tx, uint8_t *byte);

/* 1 enquanto houver quadro em transmissao */
int tx_ocupado(const tx_t *tx);

/* ------------------------------------------------------------------ */
/* Receptor                                                           */
/* ------------------------------------------------------------------ */

typedef enum {
    RX_ESPERA_STX,    /* sincronizando: descarta tudo ate achar o STX */
    RX_ESPERA_QTD,    /* proximo byte e o QTD_DADOS                   */
    RX_RECEBE_DADOS,  /* recebendo o campo DADOS                      */
    RX_ESPERA_CHK,    /* proximo byte e o checksum                    */
    RX_ESPERA_ETX     /* proximo byte e o ETX                         */
} rx_estado_t;

typedef enum {
    RX_NADA,          /* byte consumido, quadro ainda incompleto      */
    RX_QUADRO_OK,     /* quadro completo e validado                   */
    RX_ERRO_QTD,      /* QTD_DADOS maior que PROTO_MAX_DADOS          */
    RX_ERRO_CHK,      /* checksum recebido nao confere                */
    RX_ERRO_ETX       /* byte final diferente de ETX                  */
} rx_result_t;

typedef struct {
    rx_estado_t estado;
    uint8_t qtd;                     /* QTD_DADOS do quadro corrente  */
    uint8_t idx;                     /* quantos dados ja chegaram     */
    uint8_t chk;                     /* checksum acumulado            */
    uint8_t dados[PROTO_MAX_DADOS];  /* dados recebidos               */
} rx_t;

/* coloca o receptor sincronizando (a espera de um STX) */
void rx_init(rx_t *rx);

/* executa um passo da FSM do receptor consumindo um byte da serial.
 * o resultado indica se o quadro terminou (RX_QUADRO_OK) ou se houve
 * erro; em ambos os casos a FSM volta para RX_ESPERA_STX.
 * apos RX_QUADRO_OK os campos rx->qtd e rx->dados contem o quadro. */
rx_result_t rx_processa(rx_t *rx, uint8_t byte);

#endif /* PROTOCOLO_H */
