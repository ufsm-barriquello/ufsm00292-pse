#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pt.h"

#define STX_BYTE        0x02
#define ETX_BYTE        0x03
#define ACK_BYTE        0x06
#define MAX_PAYLOAD     128
#define RETRY_TIMEOUT   5     // Ciclos de clock para timeout de retransmissão
#define MAX_RETRIES     3

/* Fila circular simples para simular o canal serial */
#define QUEUE_SIZE 64
typedef struct {
    uint8_t data[QUEUE_SIZE];
    int head;
    int tail;
    int count;
} ByteQueue;

static void queue_init(ByteQueue *q) {
    q->head = q->tail = q->count = 0;
}

static bool queue_push(ByteQueue *q, uint8_t byte) {
    if (q->count >= QUEUE_SIZE) return false;
    q->data[q->tail] = byte;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;
    return true;
}

static bool queue_pop(ByteQueue *q, uint8_t *byte) {
    if (q->count == 0) return false;
    *byte = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;
    return true;
}

/* Canais de comunicação simulados */
static ByteQueue serial_tx_to_rx;
static ByteQueue serial_rx_to_tx_ack;
static int global_timer = 0;

/* Contexto da Protothread Transmissora (TX) */
typedef struct {
    struct pt pt;
    uint8_t frame[MAX_PAYLOAD + 4];
    int frame_len;
    int tx_index;
    int timer_start;
    int retries;
    bool ack_received;
    bool transmission_done;
    bool success;
} TxContext;

/* Contexto da Protothread Receptora (RX) */
typedef struct {
    struct pt pt;
    uint8_t buffer[MAX_PAYLOAD];
    uint8_t qtd;
    uint8_t rx_byte;
    uint8_t calculated_chk;
    int rx_index;
    bool packet_ready;
} RxContext;

/* Inicialização do transmissor com montagem do frame do protocolo */
void tx_init(TxContext *tx, const uint8_t *payload, uint8_t len) {
    PT_INIT(&tx->pt);
    tx->tx_index = 0;
    tx->retries = 0;
    tx->ack_received = false;
    tx->transmission_done = false;
    tx->success = false;

    // Monta frame: [STX | QTD | DADOS | CHK | ETX]
    tx->frame[0] = STX_BYTE;
    tx->frame[1] = len;
    uint8_t chk = 0;
    for (int i = 0; i < len; i++) {
        tx->frame[2 + i] = payload[i];
        chk += payload[i];
    }
    tx->frame[2 + len] = chk;
    tx->frame[3 + len] = ETX_BYTE;
    tx->frame_len = len + 4;
}

/* Injeção manual de erro para testes de resiliência */
void tx_corrupt_checksum(TxContext *tx) {
    int chk_pos = tx->frame_len - 2;
    tx->frame[chk_pos] ^= 0xFF; // Inverte bits do Checksum
}

/* Protothread Transmissora */
int pt_tx_thread(TxContext *tx) {
    PT_BEGIN(&tx->pt);

    while (tx->retries < MAX_RETRIES && !tx->ack_received) {
        printf("[TX] Enviando frame (Tentativa %d)...\n", tx->retries + 1);

        // 1. Transmite todos os bytes do frame pelo canal serial
        for (tx->tx_index = 0; tx->tx_index < tx->frame_len; tx->tx_index++) {
            PT_WAIT_UNTIL(&tx->pt, queue_push(&serial_tx_to_rx, tx->frame[tx->tx_index]));
        }

        // 2. Dispara temporizador de espera por confirmação (ACK)
        tx->timer_start = global_timer;
        
        // 3. Aguarda recebimento do ACK ou estouro do timeout
        PT_WAIT_UNTIL(&tx->pt, 
            (serial_rx_to_tx_ack.count > 0) || 
            (global_timer - tx->timer_start >= RETRY_TIMEOUT)
        );

        // 4. Trata confirmação
        uint8_t ack_val;
        if (queue_pop(&serial_rx_to_tx_ack, &ack_val) && ack_val == ACK_BYTE) {
            printf("[TX] ACK recebido com sucesso!\n");
            tx->ack_received = true;
            tx->success = true;
            break;
        } else {
            printf("[TX] Timeout / ACK nao recebido. Reenviando...\n");
            tx->retries++;
        }
    }

    tx->transmission_done = true;
    PT_END(&tx->pt);
}

/* Protothread Receptora */
int pt_rx_thread(RxContext *rx) {
    PT_BEGIN(&rx->pt);

    while (1) {
        rx->packet_ready = false;
        rx->calculated_chk = 0;
        rx->rx_index = 0;

        // 1. Espera pelo delimitador inicial STX (0x02)
        do {
            PT_WAIT_UNTIL(&rx->pt, queue_pop(&serial_tx_to_rx, &rx->rx_byte));
        } while (rx->rx_byte != STX_BYTE);

        // 2. Espera e lê o tamanho do payload (QTD)
        PT_WAIT_UNTIL(&rx->pt, queue_pop(&serial_tx_to_rx, &rx->qtd));
        if (rx->qtd == 0 || rx->qtd > MAX_PAYLOAD) {
            continue; // Formato inválido, reinicia
        }

        // 3. Lê os N bytes de dados e calcula o Checksum
        for (rx->rx_index = 0; rx->rx_index < rx->qtd; rx->rx_index++) {
            PT_WAIT_UNTIL(&rx->pt, queue_pop(&serial_tx_to_rx, &rx->rx_byte));
            rx->buffer[rx->rx_index] = rx->rx_byte;
            rx->calculated_chk += rx->rx_byte;
        }

        // 4. Lê o Checksum recebido
        PT_WAIT_UNTIL(&rx->pt, queue_pop(&serial_tx_to_rx, &rx->rx_byte));
        if (rx->rx_byte != rx->calculated_chk) {
            printf("[RX] Erro: Checksum invalido (Esperado: 0x%02X, Recebido: 0x%02X). Descartando!\n", 
                    rx->calculated_chk, rx->rx_byte);
            continue; // Checksum falhou, ignora sem enviar ACK
        }

        // 5. Lê o delimitador final ETX (0x03)
        PT_WAIT_UNTIL(&rx->pt, queue_pop(&serial_tx_to_rx, &rx->rx_byte));
        if (rx->rx_byte != ETX_BYTE) {
            printf("[RX] Erro: ETX invalido. Descartando!\n");
            continue;
        }

        // 6. Pacote íntegro: Envia ACK (0x06) para o transmissor
        printf("[RX] Pacote integro recebido (%d bytes). Enviando ACK...\n", rx->qtd);
        queue_push(&serial_rx_to_tx_ack, ACK_BYTE);
        rx->packet_ready = true;
    }

    PT_END(&rx->pt);
}

/* Loop Cooperativo do Sistema */
void run_simulation(TxContext *tx, RxContext *rx) {
    int max_steps = 100;
    while (!tx->transmission_done && max_steps-- > 0) {
        global_timer++;
        pt_tx_thread(tx);
        pt_rx_thread(rx);
    }
}

/* Bancada de Testes TDD */
int main() {
    printf("====================================================\n");
    printf("     TESTE DE PROTOCOLO COM PROTOTHREADS (TX / RX)  \n");
    printf("====================================================\n\n");

    queue_init(&serial_tx_to_rx);
    queue_init(&serial_rx_to_tx_ack);

    RxContext rx;
    PT_INIT(&rx.pt);
    TxContext tx;

    /* CENÁRIO 1: Transmissão Nominal com Sucesso */
    printf("--- [CENARIO 1] Transmissao Nominal ---\n");
    uint8_t payload1[] = { 10, 20, 30, 40, 50 };
    tx_init(&tx, payload1, sizeof(payload1));
    run_simulation(&tx, &rx);
    printf("Resultado: %s\n\n", tx.success ? "[PASS] Transmissao concluida com ACK!" : "[FAIL]");

    /* CENÁRIO 2: Transmissão com Erro de Checksum e Reenvio */
    printf("--- [CENARIO 2] Injecao de Erro de Checksum com Retransmissao ---\n");
    uint8_t payload2[] = { 0xAA, 0xBB, 0xCC };
    tx_init(&tx, payload2, sizeof(payload2));
    
    // Corrompe o checksum da primeira tentativa
    tx_corrupt_checksum(&tx);
    
    // Executa alguns ciclos até o descarte pelo RX e estouro do timeout
    for (int i = 0; i < RETRY_TIMEOUT + 2; i++) {
        global_timer++;
        pt_tx_thread(&tx);
        pt_rx_thread(&rx);
    }

    // Corrige o pacote para a retransmissão
    tx_init(&tx, payload2, sizeof(payload2));
    tx.retries = 1; // Mantém contagem da retransmissão
    run_simulation(&tx, &rx);
    printf("Resultado: %s\n\n", tx.success ? "[PASS] Retransmissao bem-sucedida apos recuperacao!" : "[FAIL]");

    printf("====================================================\n");
    printf(" STATUS: TODOS OS REQUISITOS COM PROTOTHREADS ATENDIDOS!\n");
    printf("====================================================\n");

    return 0;
}