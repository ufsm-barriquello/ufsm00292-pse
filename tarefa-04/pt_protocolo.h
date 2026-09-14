#ifndef PT_PROTOCOLO_H
#define PT_PROTOCOLO_H

#include <stdint.h>
#include <stdbool.h>
#include "pt.h"

#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NACK 0x15

#define MAX_PAYLOAD 256
#define MAX_FRAME_SIZE (MAX_PAYLOAD + 4) // STX + QTD + DADOS + CHK + ETX
#define MAX_RETENTATIVAS 3

// Canal de Comunicacao Simulado (Fila FIFO de bytes)
typedef struct {
    uint8_t buffer[MAX_FRAME_SIZE * 2];
    int head;
    int tail;
    int count;
} CanalComunicacao;

void canal_init(CanalComunicacao *c);
bool canal_write(CanalComunicacao *c, uint8_t byte);
bool canal_read(CanalComunicacao *c, uint8_t *byte);
bool canal_is_empty(const CanalComunicacao *c);

// Estrutura de contexto do Codificador / Transmissor PT
typedef struct {
    struct pt pt;                   // Instancia da Protothread
    uint8_t payload[MAX_PAYLOAD];   // Dados uteis
    uint8_t qtd;                    // Tamanho dos dados uteis
    uint8_t frame[MAX_FRAME_SIZE];  // Quadro montado pelo codificador
    int frame_len;                  // Comprimento do quadro
    int tx_index;                   // Indice de transmissao byte a byte
    int retentativas;               // Contador de tentativas
    int timer;                      // Contador de tempo para timeout
    int timeout_limit;              // Limite de tempo de espera pelo ACK
    bool ack_recebido;              // Flag de ACK confirmado
    bool erro_timeout;              // Flag de erro (estouro de retentativas)
    bool concluido;                 // Flag de finalizacao bem-sucedida
} TransmissorPT;

// Estrutura de contexto do Decodificador / Receptor PT
typedef struct {
    struct pt pt;                   // Instancia da Protothread
    uint8_t buffer_rx[MAX_PAYLOAD]; // Dados extraidos
    uint8_t qtd_esperada;           // Bytes esperados no payload
    uint8_t chk_calculado;          // Checksum XOR acumulado
    uint8_t chk_recebido;           // Checksum lido
    int ind_rx;                     // Indice de escrita no buffer_rx
    uint8_t byte_temp;              // Byte temporario lido do canal
    bool pacote_valido;             // Flag de pacote integro
    int pacotes_recebidos_count;    // Contador total de pacotes validados
} ReceptorPT;

// API de Inicializacao
void tx_init(TransmissorPT *tx, const uint8_t *dados, uint8_t qtd, int timeout_limit);
void rx_init(ReceptorPT *rx);

// Codificador
int codificar_pacote(const uint8_t *dados, uint8_t qtd, uint8_t *frame_out);

// Protothreads de Transmissao e Recepcao
int pt_transmissora_run(TransmissorPT *tx, CanalComunicacao *tx_to_rx, CanalComunicacao *rx_to_tx);
int pt_receptora_run(ReceptorPT *rx, CanalComunicacao *tx_to_rx, CanalComunicacao *rx_to_tx);

#endif // PT_PROTOCOLO_H
