/* protothreads_protocol.c
   Implementação cooperativa (protothread-style).
   Frame: [STX][LEN][DATA...][CHK][ETX]
   CHK = XOR of LEN and DATA bytes
   ACK byte: 0x06
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

/* Protocol bytes */
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NACK 0x15
#define MAX_LEN 255
#define CHANNEL_SIZE 1024

/* Timing / retransmission */
#define TIMEOUT_TICKS 5
#define MAX_RETRIES 3

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

/* read-next byte (consumes it) */
int get_byte(uint8_t *b) {
    if (channel_read_index < channel_write_index) {
        *b = channel[channel_read_index++];
        return 1;
    }
    return 0;
}

/* consume_incoming_byte is the same as get_byte (used by transmitter to read ACKs) */
int consume_incoming_byte(uint8_t *b) {
    return get_byte(b);
}

/* =============================================================
   Global tick counter to simulate time passing
   ============================================================= */
unsigned long ticks = 0;
void tick(void) { ticks++; }

/* =============================================================
   Helper: build frame into a local buffer
   Frame: STX LEN DATA... CHK ETX
   ============================================================= */
static uint16_t build_frame(uint8_t *dst, uint8_t *data, uint8_t len) {
    uint16_t idx = 0;
    dst[idx++] = STX;
    dst[idx++] = len;
    uint8_t chk = 0;
    chk ^= len;
    for (uint8_t i = 0; i < len; ++i) {
        dst[idx++] = data[i];
        chk ^= data[i];
    }
    dst[idx++] = chk;
    dst[idx++] = ETX;
    return idx;
}

/* =============================================================
   Transmitter (protothread-style: maintains its own state integer)
   ============================================================= */
typedef struct {
    int pt_state;               /* protothread state (integer label) */
    uint8_t *data;
    uint8_t len;
    uint8_t frame[3 + MAX_LEN + 1];
    uint16_t frame_len;
    uint16_t send_index;
    int attempts;
    unsigned long wait_start_tick;
    int finished; /* 0 ongoing, 1 success, -1 fail */
} transmitter_t;

/* Called repeatedly; returns 1 when finished (success or fail), 0 while running */
int pt_transmitter(transmitter_t *t) {
    switch (t->pt_state) {
        case 0:
            t->attempts = 0;
            t->finished = 0;
            t->frame_len = build_frame(t->frame, t->data, t->len);
            t->pt_state = 1;
            /* fallthrough */

        case 1:
            if (t->attempts >= MAX_RETRIES) {
                t->finished = -1;
                t->pt_state = 99;
                return 1;
            }
            t->attempts++;
            t->send_index = 0;
            t->pt_state = 2;
            /* fallthrough */

        case 2:
            if (t->send_index < t->frame_len) {
                send_byte(t->frame[t->send_index++]);
                /* yield: return control to scheduler */
                return 0;
            }
            /* finished sending frame */
            t->wait_start_tick = ticks;
            t->pt_state = 3;
            /* fallthrough */

        case 3: {
            uint8_t b;
            if (consume_incoming_byte(&b)) {
                if (b == ACK) {
                    t->finished = 1;
                    t->pt_state = 99;
                    return 1;
                } else if (b == NACK) {
                    /* retransmit immediately */
                    t->pt_state = 1;
                    return 0;
                } else {
                    /* stray byte, ignore */
                }
            }
            if ((ticks - t->wait_start_tick) >= TIMEOUT_TICKS) {
                /* timeout -> retransmit */
                t->pt_state = 1;
                return 0;
            }
            /* no ACK yet, keep waiting (yield) */
            return 0;
        }

        case 99:
            return 1;

        default:
            return 1;
    }
}

/* =============================================================
   Receiver (consumes bytes one-by-one; maintains parsing state)
   ============================================================= */
typedef struct {
    int pt_state; /* not used as coroutine index, but could be extended */
    uint8_t out[MAX_LEN];
    uint8_t out_len;
    uint8_t state; /* parsing sub-state: 0 WAIT_STX,1 READ_LEN,2 READ_DATA,3 READ_CHK,4 READ_ETX */
    uint8_t expected_len;
    uint8_t chk;
    uint8_t i;
    int result; /* 1 message ready, -1 error, 0 ongoing */
} receiver_t;

/* Called repeatedly; tries to consume one byte per call and processes it.
   Returns 1 if produced an ACK (for potential immediate handling), 0 otherwise. */
int pt_receiver(receiver_t *r) {
    uint8_t b;
    if (!get_byte(&b)) return 0; /* nothing to do */

    switch (r->state) {
        case 0: /* WAIT_STX */
            if (b == STX) {
                r->chk = 0;
                r->i = 0;
                r->state = 1; /* READ_LEN */
            } else {
                /* ignore stray bytes */
            }
            break;
        case 1: /* READ_LEN */
            r->expected_len = b;
            r->chk ^= b;
            if (r->expected_len > MAX_LEN) {
                r->result = -1;
                r->state = 0; /* resync */
                return 0;
            } else if (r->expected_len == 0) {
                r->state = 3; /* READ_CHK */
            } else {
                r->state = 2; /* READ_DATA */
            }
            break;
        case 2: /* READ_DATA */
            r->out[r->i++] = b;
            r->chk ^= b;
            if (r->i >= r->expected_len) r->state = 3; /* READ_CHK */
            break;
        case 3: { /* READ_CHK */
            uint8_t recv_chk = b;
            if (recv_chk != r->chk) {
                r->result = -1;
                r->state = 0; /* resync */
                return 0;
            } else {
                r->state = 4; /* READ_ETX */
            }
            break;
        }
        case 4: /* READ_ETX */
            if (b == ETX) {
                r->out_len = r->expected_len;
                r->result = 1;
                r->state = 0; /* ready for next frame */
                /* send ACK */
                send_byte(ACK);
                return 1;
            } else {
                r->result = -1;
                r->state = 0;
                return 0;
            }
            break;
        default:
            r->state = 0;
            break;
    }

    return 0;
}

/* =============================================================
   Scheduler helper: runs receiver then transmitter repeatedly
   ============================================================= */
void run_scheduler(transmitter_t *tx, receiver_t *rx, unsigned long max_iterations) {
    unsigned long iter = 0;
    while (iter++ < max_iterations) {
        /* run receiver first so it can consume bytes sent earlier */
        pt_receiver(rx);
        pt_transmitter(tx);
        tick();
        if (tx->finished != 0) break;
    }
}

/* =============================================================
   Tests (TDD-style: adaptados para implementação sem macros)
   ============================================================= */

static char * teste_transmit_receive_simple(void) {
    reset_channel();
    transmitter_t tx;
    receiver_t rx;
    memset(&tx, 0, sizeof(tx));
    memset(&rx, 0, sizeof(rx));

    uint8_t msg[] = {0x10, 0x20, 0x30};
    tx.data = msg;
    tx.len = 3;
    tx.pt_state = 0;
    tx.finished = 0;
    rx.state = 0;

    run_scheduler(&tx, &rx, 1000);

    verifica("erro: transmissor não terminou", tx.finished != 0);
    verifica("erro: transmissor falhou", tx.finished == 1);

    /* Build expected frame and compare buffer prefix */
    uint8_t expected[512];
    uint16_t expected_len = build_frame(expected, msg, 3);

    verifica("erro: canal não recebeu o frame completo", channel_write_index >= expected_len);
    for (uint16_t i = 0; i < expected_len; ++i) {
        verifica("erro: byte do frame diferente do esperado", channel[i] == expected[i]);
    }

    return 0;
}

static char * teste_retransmit_on_missing_ack(void) {
    reset_channel();
    transmitter_t tx;
    receiver_t rx;
    memset(&tx, 0, sizeof(tx));
    memset(&rx, 0, sizeof(rx));

    uint8_t msg[] = {0x55};
    tx.data = msg;
    tx.len = 1;
    tx.pt_state = 0;
    tx.finished = 0;
    rx.state = 0;

    /* run only transmitter for TIMEOUT_TICKS+2 ticks to force timeout/retransmit */
    for (int i = 0; i < TIMEOUT_TICKS + 2; ++i) {
        pt_transmitter(&tx);
        tick();
    }

    /* Now run both so receiver can acknowledge */
    for (int i = 0; i < 200; ++i) {
        pt_receiver(&rx);
        pt_transmitter(&tx);
        tick();
        if (tx.finished != 0) break;
    }

    verifica("erro: transmissor não terminou", tx.finished != 0);
    verifica("erro: transmissor falhou após retransmissões", tx.finished == 1);

    return 0;
}

static char * teste_checksum_error(void) {
    reset_channel();
    uint8_t msg[] = {0xAA, 0xBB};
    uint8_t temp_frame[512];
    uint16_t fl = build_frame(temp_frame, msg, 2);
    for (uint16_t i = 0; i < fl; ++i) send_byte(temp_frame[i]);
    /* corrupt CHK (fl-2) */
    if (channel_write_index >= 2) channel[channel_write_index - 2] ^= 0xFF;

    receiver_t rx;
    memset(&rx, 0, sizeof(rx));
    rx.state = 0;

    /* run receiver for some steps */
    for (int i = 0; i < 50; ++i) {
        pt_receiver(&rx);
        tick();
    }

    /* there should be no ACK byte in channel tail */
    for (int i = 0; i < channel_write_index; ++i) {
        verifica("erro: checksum inválido erroneamente aceito", channel[i] != ACK);
    }

    return 0;
}

static char * teste_etx_missing(void) {
    reset_channel();
    uint8_t msg[] = {0x11};
    uint8_t temp_frame[512];
    uint16_t fl = build_frame(temp_frame, msg, 1);
    /* write frame but remove ETX */
    for (uint16_t i = 0; i < fl - 1; ++i) send_byte(temp_frame[i]);

    receiver_t rx;
    memset(&rx, 0, sizeof(rx));
    rx.state = 0;

    /* run receiver */
    for (int i = 0; i < 50; ++i) {
        pt_receiver(&rx);
        tick();
    }

    /* there should be no ACK produced */
    for (int i = 0; i < channel_write_index; ++i) {
        verifica("erro: ETX ausente erroneamente aceito", channel[i] != ACK);
    }

    return 0;
}

/* =============================================================
   Runner
   ============================================================= */
static char * executa_testes(void) {
    executa_teste(teste_transmit_receive_simple);
    executa_teste(teste_retransmit_on_missing_ack);
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
