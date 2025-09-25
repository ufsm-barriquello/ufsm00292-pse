#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "pt.h"   // biblioteca protothread de Adam Dunkels

#define STX 0x02
#define ETX 0x03
#define MAX_DATA 255
#define ACK 0x06
#define NACK 0x15

// ---------------- Canal TX -> RX ----------------
uint8_t chan_txrx[1024];
int txrx_pos = 0, rxrx_pos = 0;

void send_txrx(uint8_t b) { chan_txrx[txrx_pos++] = b; }
int recv_txrx(uint8_t *b) {
    if (rxrx_pos < txrx_pos) {
        *b = chan_txrx[rxrx_pos++];
        return 1;
    }
    return 0;
}

// ---------------- Canal RX -> TX ----------------
uint8_t chan_rxtx[1024];
int rxtx_pos = 0, txtx_pos = 0;

void send_rxtx(uint8_t b) { chan_rxtx[rxtx_pos++] = b; }
int recv_rxtx(uint8_t *b) {
    if (txtx_pos < rxtx_pos) {
        *b = chan_rxtx[txtx_pos++];
        return 1;
    }
    return 0;
}

// ---------------- Contextos -----------------
struct tx_ctx {
    struct pt pt;
    uint8_t data[MAX_DATA];
    uint8_t qtd;
    uint8_t chk;
    int waiting_ack;
    int last_result; // 1=ACK, -1=NACK, 0=pendente
};

struct rx_ctx {
    struct pt pt;
    uint8_t buf[MAX_DATA];
    uint8_t qtd, chk, idx;
    int state;
};

static struct tx_ctx tx;
static struct rx_ctx rx;

// ---------------- Protothread TX -----------------
static PT_THREAD(transmitter(struct tx_ctx *t)) {
    PT_BEGIN(&t->pt);

    // envia quadro
    send_txrx(STX);
    send_txrx(t->qtd);
    t->chk = t->qtd;
    for (int i = 0; i < t->qtd; i++) {
        send_txrx(t->data[i]);
        t->chk += t->data[i];
    }
    send_txrx(t->chk);
    send_txrx(ETX);

    t->waiting_ack = 1;
    t->last_result = 0;

    // espera resposta
    while (t->waiting_ack) {
        uint8_t resp;
        if (recv_rxtx(&resp)) {
            if (resp == ACK) {
                t->waiting_ack = 0;
                t->last_result = 1;
            } else {
                t->waiting_ack = 0;
                t->last_result = -1;
            }
        }
        PT_YIELD(&t->pt);
    }

    PT_END(&t->pt);
}

// ---------------- Protothread RX -----------------
enum { ST_WAIT_STX, ST_WAIT_QTD, ST_WAIT_DATA, ST_WAIT_CHK, ST_WAIT_ETX };

static PT_THREAD(receiver(struct rx_ctx *r)) {
    PT_BEGIN(&r->pt);

    r->state = ST_WAIT_STX;
    r->idx = 0;
    r->chk = 0;

    while (1) {
        uint8_t b;
        if (!recv_txrx(&b)) {
            PT_YIELD(&r->pt);
            continue;
        }

        switch (r->state) {
        case ST_WAIT_STX:
            if (b == STX) r->state = ST_WAIT_QTD;
            break;
        case ST_WAIT_QTD:
            r->qtd = b;
            r->chk = b;
            r->idx = 0;
            r->state = (r->qtd ? ST_WAIT_DATA : ST_WAIT_CHK);
            break;
        case ST_WAIT_DATA:
            r->buf[r->idx++] = b;
            r->chk += b;
            if (r->idx >= r->qtd) r->state = ST_WAIT_CHK;
            break;
        case ST_WAIT_CHK:
            if (b == r->chk) {
                r->state = ST_WAIT_ETX;
            } else {
                printf("[RX] Checksum invalido!\n");
                send_rxtx(NACK);
                r->state = ST_WAIT_STX;
            }
            break;
        case ST_WAIT_ETX:
            if (b == ETX) {
                printf("[RX] Quadro valido: %.*s\n", r->qtd, r->buf);
                send_rxtx(ACK);
            } else {
                printf("[RX] ETX invalido (%02X)\n", b);
                send_rxtx(NACK);
            }
            r->state = ST_WAIT_STX;
            break;
        }
    }

    PT_END(&r->pt);
}

// ---------------- Funções Auxiliares ----------------
void reset_system(void) {
    txrx_pos = rxrx_pos = 0;
    rxtx_pos = txtx_pos = 0;
    PT_INIT(&tx.pt);
    PT_INIT(&rx.pt);
    tx.last_result = 0;
}

void run_tx_only(void) {
    transmitter(&tx); // roda só TX, não RX
}

void run_system(int max_steps) {
    for (int i = 0; i < max_steps; i++) {
        transmitter(&tx);
        receiver(&rx);
        if (tx.last_result != 0) break;
    }
}

// ---------------- MAIN (TDD) -----------------
int main(void) {
    // ---- Teste 1: Quadro válido ----
    reset_system();
    strcpy((char*)tx.data, "HELLO");
    tx.qtd = 5;
    run_system(100);
    assert(tx.last_result == 1);
    printf("Teste 1 (quadro valido) OK\n");

    // ---- Teste 2: Checksum errado ----
    reset_system();
    strcpy((char*)tx.data, "WORLD");
    tx.qtd = 5;
    run_tx_only(); // gera quadro
    chan_txrx[tx.qtd + 2] ^= 0xFF; // corrompe checksum
    run_system(100);
    assert(tx.last_result == -1);
    printf("Teste 2 (checksum errado) OK\n");

    // ---- Teste 3: Quadro vazio (qtd=0) ----
    reset_system();
    tx.qtd = 0;
    run_system(100);
    assert(tx.last_result == 1);
    printf("Teste 3 (qtd=0) OK\n");

    // ---- Teste 4: ETX errado ----
    reset_system();
    strcpy((char*)tx.data, "X");
    tx.qtd = 1;
    run_tx_only(); // gera quadro
    chan_txrx[tx.qtd + 3] = 0x99; // corrompe ETX
    run_system(100);
    assert(tx.last_result == -1);
    printf("Teste 4 (ETX errado) OK\n");

    printf("Todos os testes passaram!\n");
    return 0;
}
