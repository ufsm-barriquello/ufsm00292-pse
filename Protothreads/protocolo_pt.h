#ifndef PROTOCOLO_PT_H
#define PROTOCOLO_PT_H

#include <stdint.h>
#include <stdbool.h>
#include "pt.h"

// --- Constantes do Protocolo ---
#define STX_VALUE 0x02
#define ETX_VALUE 0x03
#define ACK_VALUE 0x06
#define MAX_DADOS_SIZE 16
#define TIMEOUT_MS 500

// Estrutura para simular um buffer de comunicação
typedef struct {
    uint8_t buffer[256];
    uint16_t head;
    uint16_t tail;
} comm_buffer_t;

// Estrutura para compartilhar dados e estado entre os protothreads e o driver
typedef struct {
    // Buffers de comunicação
    comm_buffer_t tx_to_rx_buffer;
    comm_buffer_t rx_to_tx_buffer;

    // Estado do Transmissor
    uint8_t dados_para_enviar[MAX_DADOS_SIZE];
    uint8_t qtd_a_enviar;
    bool iniciar_transmissao;
    bool ack_recebido;

    // Estado do Receptor
    uint8_t dados_recebidos[MAX_DADOS_SIZE];
    uint8_t qtd_recebida;
    bool pacote_recebido_com_sucesso;
    uint8_t rx_chk_calculado;

} shared_channel_t;

// Protótipos das funções
PT_THREAD(protothread_tx(struct pt* pt, shared_channel_t* channel));
PT_THREAD(protothread_rx(struct pt* pt, shared_channel_t* channel));
bool buffer_write(comm_buffer_t* comm, uint8_t byte);
bool buffer_read(comm_buffer_t* comm, uint8_t* byte);

#endif // PROTOCOLO_PT_H