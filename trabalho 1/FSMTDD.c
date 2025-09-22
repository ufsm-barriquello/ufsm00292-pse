#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { \
    printf("Executando: %-35s... ", #teste); \
    char *mensagem = teste(); \
    testes_executados++; \
    if (mensagem) { \
        printf("FALHOU\n"); \
        return mensagem; \
    } else { \
        printf("OK\n"); \
    } \
} while (0)

int testes_executados = 0;

/*
    Definições do protocolo
*/
#define STX 0x02
#define ETX 0x03
#define MAX_LEN 255 
#define CHANNEL_SIZE 1024

/*
    Canal de comunicação simulado
*/
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

/*
    FSM - Transmissor
*/
typedef enum {
    TX_SEND_STX,
    TX_SEND_LEN,
    TX_SEND_DATA,
    TX_SEND_CHK,
    TX_SEND_ETX,
    TX_DONE
} TxState;

void transmitter_fsm(uint8_t *data, uint8_t len) {
    TxState state = TX_SEND_STX;
    uint8_t i = 0;
    uint8_t chk = 0;

    while (state != TX_DONE) {
        switch (state) {
            case TX_SEND_STX:
                send_byte(STX);
                state = TX_SEND_LEN;
                break;

            case TX_SEND_LEN:
                send_byte(len);
                chk ^= len;
                state = (len > 0) ? TX_SEND_DATA : TX_SEND_CHK;
                break;

            case TX_SEND_DATA:
                send_byte(data[i]);
                chk ^= data[i];
                i++;
                if (i >= len)
                    state = TX_SEND_CHK;
                break;

            case TX_SEND_CHK:
                send_byte(chk);
                state = TX_SEND_ETX;
                break;

            case TX_SEND_ETX:
                send_byte(ETX);
                state = TX_DONE;
                break;

            case TX_DONE:
                state = TX_DONE;
                break;
        }
    }
}

/*
    FSM - Receptor
*/
typedef enum {
    RX_WAIT_STX,
    RX_READ_LEN,
    RX_READ_DATA,
    RX_READ_CHK,
    RX_READ_ETX,
    RX_DONE,
    RX_ERROR
} RxState;

/* Retorno:
   1  -> mensagem valida
  -1  -> erro
   0  -> incompleto */
int receiver_fsm(uint8_t *out, uint8_t *out_len, uint8_t max_len_allowed) {
    RxState state = RX_WAIT_STX;
    uint8_t len = 0;
    uint8_t chk = 0, recv_chk = 0;
    uint8_t i = 0, b;

    while (get_byte(&b)) {
        switch (state) {
            case RX_WAIT_STX:
                if (b == STX) {
                    state = RX_READ_LEN;
                    chk = 0;
                    i = 0;
                }
                break;

            case RX_READ_LEN:
                len = b;
                chk ^= b;
                
                if (len > max_len_allowed) {
                    state = RX_ERROR;
                } else if (len == 0) {
                    state = RX_READ_CHK;
                } else {
                    state = RX_READ_DATA;
                }
                break;

            case RX_READ_DATA:
                out[i++] = b;
                chk ^= b;
                if (i >= len)
                    state = RX_READ_CHK;
                break;

            case RX_READ_CHK:
                recv_chk = b;
                if (recv_chk != chk) {
                    state = RX_ERROR;
                } else {
                    state = RX_READ_ETX;
                }
                break;

            case RX_READ_ETX:
                if (b == ETX) {
                    *out_len = len;
                    state = RX_DONE;
                } else {
                    state = RX_ERROR;
                }
                break;

            case RX_DONE:
            case RX_ERROR:
                goto end_loop;
        }
    }
end_loop:;

    if (state == RX_DONE) return 1;
    if (state == RX_ERROR) return -1;
    return 0;
}


/*
    TESTES
*/
static char * teste_transmit_receive_simple(void) {
    reset_channel();
    uint8_t msg[] = {0x10, 0x20, 0x30};
    transmitter_fsm(msg, 3);

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len, MAX_LEN);

    verifica("erro: recepção simples falhou", result == 1);
    verifica("erro: LEN incorreto", out_len == 3);
    verifica("erro: byte 0 incorreto", out[0] == 0x10);
    verifica("erro: byte 1 incorreto", out[1] == 0x20);
    verifica("erro: byte 2 incorreto", out[2] == 0x30);

    return 0;
}

static char * teste_transmit_receive_empty(void) {
    reset_channel();
    transmitter_fsm(NULL, 0);

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len, MAX_LEN);

    verifica("erro: recepção de quadro vazio falhou", result == 1);
    verifica("erro: LEN deveria ser 0", out_len == 0);

    return 0;
}

static char * teste_checksum_error(void) {
    reset_channel();
    uint8_t msg[] = {0xAA, 0xBB};
    transmitter_fsm(msg, 2);

    channel[channel_write_index-2] ^= 0xFF; 

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len, MAX_LEN);

    verifica("erro: checksum inválido não detectado", result == -1);

    return 0;
}

static char * teste_etx_missing(void) {
    reset_channel();
    uint8_t msg[] = {0x11};
    transmitter_fsm(msg, 1);

    channel_write_index--; 

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len, MAX_LEN);

    verifica("erro: ETX ausente deveria retornar incompleto", result == 0);

    return 0;
}

static char * teste_garbage_before_stx(void) {
    reset_channel();
    send_byte(0x55);
    send_byte(0x55);
    send_byte(0x55);

    uint8_t msg[] = {0xBE, 0xEF};
    transmitter_fsm(msg, 2);

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len, MAX_LEN);

    verifica("erro: FSM não ignorou lixo antes do STX", result == 1);
    verifica("erro: LEN incorreto após lixo", out_len == 2);
    verifica("erro: byte 0 incorreto após lixo", out[0] == 0xBE);
    verifica("erro: byte 1 incorreto após lixo", out[1] == 0xEF);

    return 0;
}


static char * teste_data_corruption(void) {
    reset_channel();
    uint8_t msg[] = {0x12, 0x34};
    transmitter_fsm(msg, 2);

    channel[2] ^= 0xFF;

    uint8_t out[10];
    uint8_t out_len = 0;
    int result = receiver_fsm(out, &out_len, MAX_LEN);

    verifica("erro: corrupção de dado não detectada pelo checksum", result == -1);

    return 0;
}

static char * teste_len_too_long(void) {
    reset_channel();
    uint8_t test_max_len = 10;
    uint8_t msg[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; 

    
    transmitter_fsm(msg, 11);

    uint8_t out[20];
    uint8_t out_len = 0;
    
    int result = receiver_fsm(out, &out_len, test_max_len);

    verifica("erro: tamanho maior que o permitido não foi detectado", result == -1);

    return 0;
}

/*
    Runner
*/
static char * executa_testes(void) {
    executa_teste(teste_transmit_receive_simple);
    executa_teste(teste_transmit_receive_empty);
    executa_teste(teste_checksum_error);
    executa_teste(teste_etx_missing);
    executa_teste(teste_garbage_before_stx);
    executa_teste(teste_data_corruption);
    executa_teste(teste_len_too_long);

    return 0;
}

int main(void) {
    char *resultado = executa_testes();
    printf("----------------------------------------\n");
    if (resultado != 0) {
        printf("Resultado: Teste falhou!\n");
        printf("Motivo: %s\n", resultado);
    } else {
        printf("Resultado: TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    return resultado != 0;
}