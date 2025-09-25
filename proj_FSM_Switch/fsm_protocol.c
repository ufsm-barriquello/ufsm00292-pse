#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SOF 0xAAu
#define EOF_MARK 0x55u
#define MAX_FRAME 260u  /* SOF + LEN + CMD + DATA(0..255) + CHK + EOF */
#define MAX_PAYLOAD 255u

/* Função auxiliar: calcula checksum XOR */
static uint8_t checksum_xor(const uint8_t *bytes, size_t n) {
    uint8_t x = 0u;
    for (size_t i = 0; i < n; ++i) x ^= bytes[i];
    return x;
}

/* ------------------ TRANSMISSOR (TX) ------------------ */

/* Estados do transmissor */
typedef enum {
    TX_IDLE = 0,     // Inativo, esperando instruções
    TX_SEND_SOF,     // Enviar byte inicial
    TX_SEND_LEN,     // Enviar comprimento
    TX_SEND_CMD,     // Enviar comando
    TX_SEND_DATA,    // Enviar dados
    TX_SEND_CHK,     // Enviar checksum
    TX_SEND_EOF,     // Enviar byte final
} TxState;

/* Contexto do transmissor */
typedef struct {
    TxState state;
    uint8_t buf[MAX_FRAME];
    size_t  out_len;
    uint8_t len;
    uint8_t cmd;
    const uint8_t *data;
    size_t data_idx;
    uint8_t chk;
} TxContext;

/* Inicializa transmissor */
static void tx_init(TxContext *tx) {
    memset(tx, 0, sizeof(*tx));
    tx->state = TX_IDLE;
}

/* Um passo da FSM do transmissor */
static bool tx_build_frame_step(TxContext *tx) {
    switch (tx->state) {
        case TX_IDLE:
            return false;
        case TX_SEND_SOF:
            tx->buf[0] = SOF;
            tx->out_len = 1;
            tx->state = TX_SEND_LEN;
            return false;
        case TX_SEND_LEN:
            tx->buf[tx->out_len++] = tx->len;
            tx->state = TX_SEND_CMD;
            return false;
        case TX_SEND_CMD:
            tx->buf[tx->out_len++] = tx->cmd;
            tx->data_idx = 0;
            tx->state = (tx->len == 0) ? TX_SEND_CHK : TX_SEND_DATA;
            return false;
        case TX_SEND_DATA:
            tx->buf[tx->out_len++] = tx->data[tx->data_idx++];
            if (tx->data_idx >= tx->len) {
                tx->state = TX_SEND_CHK;
            }
            return false;
        case TX_SEND_CHK: {
            uint8_t tmp[1 + 1 + MAX_PAYLOAD];
            size_t n = 0;
            tmp[n++] = tx->len;
            tmp[n++] = tx->cmd;
            for (size_t i = 0; i < tx->len; ++i) tmp[n++] = tx->data[i];
            tx->chk = checksum_xor(tmp, n);
            tx->buf[tx->out_len++] = tx->chk;
            tx->state = TX_SEND_EOF;
            return false;
        }
        case TX_SEND_EOF:
            tx->buf[tx->out_len++] = EOF_MARK;
            tx->state = TX_IDLE;
            return true; 
    }
    return false; 
}

/* Função que monta o quadro de uma vez só */
static size_t tx_build_frame(uint8_t cmd, const uint8_t *data, uint8_t len, uint8_t *out, size_t out_cap) {
    if (len > MAX_PAYLOAD) return 0;
    if (out_cap < (size_t)(len + 5)) return 0; // SOF + LEN + CMD + DATA + CHK + EOF

    TxContext tx;
    tx_init(&tx);
    tx.len = len;
    tx.cmd = cmd;
    tx.data = data;
    tx.state = TX_SEND_SOF;

    while (!tx_build_frame_step(&tx)) { /* executa a FSM */ }

    memcpy(out, tx.buf, tx.out_len);
    return tx.out_len;
}

/* ------------------ RECEPTOR (RX) ------------------ */

/* Estados do receptor */
typedef enum {
    RX_WAIT_SOF = 0,  // Espera byte inicial
    RX_READ_LEN,      // Lê comprimento
    RX_READ_CMD,      // Lê comando
    RX_READ_DATA,     // Lê dados
    RX_READ_CHK,      // Lê checksum
    RX_WAIT_EOF,      // Espera byte final
    RX_ERROR          // Erro → descartar e ressincronizar
} RxState;

/* Contexto do receptor */
typedef struct {
    RxState state;
    uint8_t len;
    uint8_t cmd;
    uint8_t data[MAX_PAYLOAD];
    uint8_t data_idx;
    uint8_t chk_calc;
    uint8_t chk_recv;
} RxContext;

/* Estrutura de saída: quadro decodificado */
typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t data[MAX_PAYLOAD];
} Frame;

/* Inicializa receptor */
static void rx_init(RxContext *rx) {
    memset(rx, 0, sizeof(*rx));
    rx->state = RX_WAIT_SOF;
}

/* Processa um byte recebido */
static bool rx_process_byte(RxContext *rx, uint8_t byte, Frame *out) {
    /* Re-sincronização dura: se encontrar SOF em qualquer estado, reinicia */
    if (rx->state != RX_WAIT_SOF && byte == SOF) {
        rx->state = RX_READ_LEN;
        rx->len = 0; rx->cmd = 0; rx->data_idx = 0; rx->chk_calc = 0;
        return false;
    }

    switch (rx->state) {
        case RX_WAIT_SOF:
            if (byte == SOF) {
                rx->state = RX_READ_LEN;
                rx->len = 0;
                rx->cmd = 0;
                rx->data_idx = 0;
                rx->chk_calc = 0;
            }
            break;

        case RX_READ_LEN:
            rx->len = byte;
            rx->chk_calc ^= byte;
            rx->state = RX_READ_CMD;
            break;

        case RX_READ_CMD:
            rx->cmd = byte;
            rx->chk_calc ^= byte;
            if (rx->len == 0) {
                rx->state = RX_READ_CHK;
            } else {
                rx->state = RX_READ_DATA;
            }
            break;

        case RX_READ_DATA:
            if (rx->data_idx < rx->len) {
                rx->data[rx->data_idx++] = byte;
                rx->chk_calc ^= byte;
                if (rx->data_idx >= rx->len) {
                    rx->state = RX_READ_CHK;
                }
            } else {
                rx->state = RX_ERROR; // overflow
            }
            break;

        case RX_READ_CHK:
            rx->chk_recv = byte;
            rx->state = RX_WAIT_EOF;
            break;

        case RX_WAIT_EOF:
            if (byte != EOF_MARK) {
                rx->state = RX_ERROR;
                break;
            }
            if (rx->chk_calc != rx->chk_recv) {
                rx->state = RX_ERROR;
                break;
            }
            // Quadro válido
            out->cmd = rx->cmd;
            out->len = rx->len;
            if (rx->len) memcpy(out->data, rx->data, rx->len);
            rx->state = RX_WAIT_SOF; 
            return true;

        case RX_ERROR:
            rx->state = (byte == SOF) ? RX_READ_LEN : RX_WAIT_SOF;
            rx->len = 0;
            rx->cmd = 0;
            rx->data_idx = 0;
            rx->chk_calc = 0;
            break;
    }
    return false;
}

/* ------------------ TESTES ------------------ */

static int test_roundtrip(void) {
    uint8_t payload[] = {0x10, 0x20, 0x30, 0x40};
    uint8_t frame[64];
    size_t n = tx_build_frame(0x01, payload, sizeof(payload), frame, sizeof(frame));
    if (n == 0) {
        printf("[ERRO] Transmissor nao gerou quadro\n");
        return 1;
    }

    RxContext rx;
    rx_init(&rx);
    Frame out;
    bool got = false;
    for (size_t i = 0; i < n; ++i) {
        if (rx_process_byte(&rx, frame[i], &out)) {
            got = true;
        }
    }
    if (!got) {
        printf("[ERRO] Receptor nao produziu quadro\n");
        return 1;
    }
    if (out.cmd != 0x01 || out.len != sizeof(payload) ||
        memcmp(out.data, payload, sizeof(payload)) != 0) {
        printf("[ERRO] Dados recebidos diferentes do transmitido\n");
        return 1;
    }
    printf("[OK] Comunicação TX - RX correta\n");
    return 0;
}

static int test_checksum_error(void) {
    uint8_t payload[] = {0xAA, 0xBB, 0xCC};
    uint8_t frame[64];
    size_t n = tx_build_frame(0x99, payload, sizeof(payload), frame, sizeof(frame));
    if (n == 0) { printf("[ERRO] Não foi possível gerar quadro\n"); return 1; }
    frame[4] ^= 0x01; // corrompe um byte

    RxContext rx; rx_init(&rx);
    Frame out;
    bool got = false;
    for (size_t i = 0; i < n; ++i) {
        if (rx_process_byte(&rx, frame[i], &out)) got = true;
    }
    if (got) { printf("[ERRO] Receptor aceitou quadro corrompido\n"); return 1; }
    printf("[OK] Quadro corrompido foi rejeitado\n");
    return 0;
}

static int test_resync_from_garbage(void) {
    uint8_t payload[] = {0x01};
    uint8_t frame[64];
    size_t n = tx_build_frame(0x07, payload, sizeof(payload), frame, sizeof(frame));
    if (n == 0) { printf("[ERRO] Nao foi possível gerar quadro\n"); return 1; }

    RxContext rx; rx_init(&rx);
    Frame out;
    bool got = false;

    // Envia lixo antes do quadro
    uint8_t garbage[] = {0x00, 0xFF, 0x13, 0xAA, 0xAA}; 
    for (size_t i = 0; i < sizeof(garbage); ++i) rx_process_byte(&rx, garbage[i], &out);
    for (size_t i = 0; i < n; ++i) {
        if (rx_process_byte(&rx, frame[i], &out)) got = true;
    }
    if (!got || out.cmd != 0x07 || out.len != 1 || out.data[0] != 0x01) {
        printf("[ERRO] Receptor nao se recuperou apos lixo na linha\n");
        return 1;
    }
    printf("[OK] Receptor se ressincronizou apos lixo\n");
    return 0;
}

/* ------------------ MAIN ------------------ */

int main(void) {
    int fails = 0;
    fails += test_roundtrip();
    fails += test_checksum_error();
    fails += test_resync_from_garbage();

    if (fails == 0) {
        printf("Todos os testes passaram. Exemplo de quadro gerado:\n");
        uint8_t payload[] = {0x10, 0x20};
        uint8_t frame[64];
        size_t n = tx_build_frame(0x42, payload, sizeof(payload), frame, sizeof(frame));
        printf("Quadro gerado (%zu bytes):", n);
        for (size_t i = 0; i < n; ++i) printf(" %02X", frame[i]);
        printf("\n");
    }
    return fails ? 1 : 0;
}
