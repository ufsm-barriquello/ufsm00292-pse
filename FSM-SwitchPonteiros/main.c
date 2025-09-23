#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

#define STX 0x02
#define ETX 0x03
#define MAX_LEN 255
#define CHANNEL_SIZE 1024

/* =============================================================
   Canal de comunicação simulado
   ============================================================= */
uint8_t channel[CHANNEL_SIZE];
int channel_write_index = 0;
int channel_read_index  = 0;

void reset_channel() {
    channel_write_index = 0;
    channel_read_index = 0;
}

void send_byte(uint8_t b) {
    if (channel_write_index < CHANNEL_SIZE) {
        channel[channel_write_index++] = b;
    }
}

int get_byte(uint8_t *b) {
    if (channel_read_index < channel_write_index) {
        *b = channel[channel_read_index++];
        return 1;
    }
    return 0;
}

/* =============================================================
   FSM - TRANSMISSOR
   ============================================================= */
typedef enum {
    TX_SEND_STX,
    TX_SEND_LEN,
    TX_SEND_DATA,
    TX_SEND_CHK,
    TX_SEND_ETX,
    TX_DONE
} TxState;

typedef struct {
    TxState state;
    uint8_t *data;
    uint8_t len;
    uint8_t i;
    uint8_t chk;
} TxContext;

typedef void (*TxHandler)(TxContext*);

void tx_send_stx(TxContext *ctx) {
    send_byte(STX);
    ctx->state = TX_SEND_LEN;
}

void tx_send_len(TxContext *ctx) {
    send_byte(ctx->len);
    ctx->chk ^= ctx->len;
    ctx->state = (ctx->len > 0) ? TX_SEND_DATA : TX_SEND_CHK;
}

void tx_send_data(TxContext *ctx) {
    send_byte(ctx->data[ctx->i]);
    ctx->chk ^= ctx->data[ctx->i];
    ctx->i++;
    if (ctx->i >= ctx->len)
        ctx->state = TX_SEND_CHK;
}

void tx_send_chk(TxContext *ctx) {
    send_byte(ctx->chk);
    ctx->state = TX_SEND_ETX;
}

void tx_send_etx(TxContext *ctx) {
    send_byte(ETX);
    ctx->state = TX_DONE;
}

void tx_done(TxContext *ctx) {
    (void)ctx; // nada
}

TxHandler tx_table[] = {
    tx_send_stx,
    tx_send_len,
    tx_send_data,
    tx_send_chk,
    tx_send_etx,
    tx_done
};

void transmitter_fsm(uint8_t *data, uint8_t len) {
    TxContext ctx = {TX_SEND_STX, data, len, 0, 0};
    while (ctx.state != TX_DONE) {
        tx_table[ctx.state](&ctx);
    }
}

/* =============================================================
   FSM - RECEPTOR
   ============================================================= */
typedef enum {
    RX_WAIT_STX,
    RX_READ_LEN,
    RX_READ_DATA,
    RX_READ_CHK,
    RX_READ_ETX,
    RX_DONE,
    RX_ERROR
} RxState;

typedef struct {
    RxState state;
    uint8_t *out;
    uint8_t *out_len;
    uint8_t len;
    uint8_t chk;
    uint8_t recv_chk;
    uint8_t i;
    uint8_t b;
    int result;
} RxContext;

typedef void (*RxHandler)(RxContext*);

void rx_wait_stx(RxContext *ctx) {
    if (!get_byte(&ctx->b)) return;
    if (ctx->b == STX) {
        ctx->chk = 0;
        ctx->i = 0;
        ctx->state = RX_READ_LEN;
    }
}

void rx_read_len(RxContext *ctx) {
    if (!get_byte(&ctx->b)) return;
    ctx->len = ctx->b;
    ctx->chk ^= ctx->b;
    if (ctx->len > MAX_LEN) {
        ctx->state = RX_ERROR;
        ctx->result = -1;
    } else if (ctx->len == 0) {
        ctx->state = RX_READ_CHK;
    } else {
        ctx->state = RX_READ_DATA;
    }
}

void rx_read_data(RxContext *ctx) {
    if (!get_byte(&ctx->b)) return;
    ctx->out[ctx->i++] = ctx->b;
    ctx->chk ^= ctx->b;
    if (ctx->i >= ctx->len)
        ctx->state = RX_READ_CHK;
}

void rx_read_chk(RxContext *ctx) {
    if (!get_byte(&ctx->b)) return;
    ctx->recv_chk = ctx->b;
    if (ctx->recv_chk != ctx->chk) {
        ctx->state = RX_ERROR;
        ctx->result = -1;
    } else {
        ctx->state = RX_READ_ETX;
    }
}

void rx_read_etx(RxContext *ctx) {
    if (!get_byte(&ctx->b)) return;
    if (ctx->b == ETX) {
        *(ctx->out_len) = ctx->len;
        ctx->state = RX_DONE;
        ctx->result = 1;
    } else {
        ctx->state = RX_ERROR;
        ctx->result = -1;
    }
}

void rx_done(RxContext *ctx) { (void)ctx; }
void rx_error(RxContext *ctx) { (void)ctx; }

RxHandler rx_table[] = {
    rx_wait_stx,
    rx_read_len,
    rx_read_data,
    rx_read_chk,
    rx_read_etx,
    rx_done,
    rx_error
};

int receiver_fsm(uint8_t *out, uint8_t *out_len) {
    RxContext ctx = {RX_WAIT_STX, out, out_len, 0, 0, 0, 0, 0, 0, 0};

    while (ctx.state != RX_DONE && ctx.state != RX_ERROR) {
        if (channel_read_index >= channel_write_index) {
            //erro de protocolo (ETX ausente)
            ctx.state = RX_ERROR;
            ctx.result = -1;
            break;
        }
        rx_table[ctx.state](&ctx);
    }

    return ctx.result;
}


/* =============================================================
   TESTES
   ============================================================= */
static char * teste_transmit_receive_simple(void) {
    reset_channel();
    uint8_t msg[] = {0x10, 0x20, 0x30};
    transmitter_fsm(msg, 3);

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len);

    verifica("erro: recepção simples falhou", result == 1);
    verifica("erro: LEN incorreto", out_len == 3);
    verifica("erro: byte 0 incorreto", out[0] == 0x10);
    verifica("erro: byte 1 incorreto", out[1] == 0x20);
    verifica("erro: byte 2 incorreto", out[2] == 0x30);

    return 0;
}

static char * teste_transmit_receive_empty(void) {
    reset_channel();
    uint8_t msg[] = {};
    transmitter_fsm(msg, 0);

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len);

    verifica("erro: recepção de quadro vazio falhou", result == 1);
    verifica("erro: LEN deveria ser 0", out_len == 0);

    return 0;
}

static char * teste_checksum_error(void) {
    reset_channel();
    uint8_t msg[] = {0xAA, 0xBB};
    transmitter_fsm(msg, 2);

    // Corrupção: altera checksum
    channel[channel_write_index-2] ^= 0xFF;

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len);

    verifica("erro: checksum inválido não detectado", result == -1);

    return 0;
}

static char * teste_etx_missing(void) {
    reset_channel();
    uint8_t msg[] = {0x11};
    transmitter_fsm(msg, 1);

    // Remove o ETX final
    channel_write_index--;

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len);

    verifica("erro: ETX ausente não detectado", result == -1);

    return 0;
}

/* =============================================================
   Runner
   ============================================================= */
static char * executa_testes(void) {
    executa_teste(teste_transmit_receive_simple);
    executa_teste(teste_transmit_receive_empty);
    executa_teste(teste_checksum_error);
    executa_teste(teste_etx_missing);

    return 0;
}

int main(void) {
    char *resultado = executa_testes();
    if (resultado != 0) {
        printf("%s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    return resultado != 0;
}
