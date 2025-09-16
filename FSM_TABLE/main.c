#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03

// Estados da FSM
typedef enum {
    WAIT_STX = 0,
    WAIT_QTD,
    WAIT_DATA,
    WAIT_CHK,
    WAIT_ETX,
    DONE,
    ERROR,
    NUM_STATES
} State;

typedef struct {
    State state;
    uint8_t qtd;
    uint8_t dados[256];
    uint8_t index;
    uint8_t checksum;
} FSM_Context;


void fsm_init(FSM_Context *ctx);


void fsm_process(FSM_Context *ctx, uint8_t byte);

typedef enum {
    CLASS_OTHER,
    CLASS_STX,
    CLASS_ETX,
    CLASS_QTD,
    CLASS_DATA,
    CLASS_CHK
} ByteClass;

static ByteClass classify(FSM_Context *ctx, uint8_t byte) {
    if (ctx->state == WAIT_STX && byte == STX) return CLASS_STX;
    if (ctx->state == WAIT_ETX && byte == ETX) return CLASS_ETX;
    if (ctx->state == WAIT_QTD) return CLASS_QTD;
    if (ctx->state == WAIT_DATA) return CLASS_DATA;
    if (ctx->state == WAIT_CHK) return CLASS_CHK;
    return CLASS_OTHER;
}


static const State transition[NUM_STATES][6] = {
    // STX       QTD       DATA      CHK       ETX       OTHER
    { WAIT_QTD,  ERROR,    ERROR,    ERROR,    ERROR,    ERROR },   // WAIT_STX
    { ERROR,     WAIT_DATA,ERROR,    ERROR,    ERROR,    ERROR },   // WAIT_QTD
    { ERROR,     ERROR,    WAIT_DATA,WAIT_CHK, ERROR,    ERROR },   // WAIT_DATA
    { ERROR,     ERROR,    ERROR,    WAIT_ETX, ERROR,    ERROR },   // WAIT_CHK
    { ERROR,     ERROR,    ERROR,    ERROR,    DONE,     ERROR },   // WAIT_ETX
    { DONE,      DONE,     DONE,     DONE,     DONE,     DONE  },   // DONE
    { ERROR,     ERROR,    ERROR,    ERROR,    ERROR,    ERROR }    // ERROR
};


void fsm_init(FSM_Context *ctx) {
    ctx->state = WAIT_STX;
    ctx->qtd = 0;
    ctx->index = 0;
    ctx->checksum = 0;
}


void fsm_process(FSM_Context *ctx, uint8_t byte) {
    ByteClass c = classify(ctx, byte);
    State next = transition[ctx->state][c];


    switch (ctx->state) {
        case WAIT_QTD:
            if (c == CLASS_QTD) {
                ctx->qtd = byte;
                ctx->index = 0;
                ctx->checksum = 0;
            }
            break;

        case WAIT_DATA:
            if (c == CLASS_DATA) {
                ctx->dados[ctx->index++] = byte;
                ctx->checksum += byte;
                if (ctx->index == ctx->qtd) {
                    next = WAIT_CHK;
                }
            }
            break;

        case WAIT_CHK:
            if (c == CLASS_CHK) {
                if (ctx->checksum != byte) {
                    next = ERROR;
                }
            }
            break;

        default:
            break;
    }

    ctx->state = next;
}

int main(void) {
    FSM_Context ctx;
    int erros = 0;

    // Teste 1: mensagem válida
    fsm_init(&ctx);
    uint8_t msg1[] = {0x02, 0x03, 'A', 'B', 'C', ('A'+'B'+'C'), 0x03};
    for (int i = 0; i < sizeof(msg1); i++) fsm_process(&ctx, msg1[i]);
    if (ctx.state != DONE) {
        printf("Teste 1 falhou! Esperado DONE, obtido %d\n", ctx.state);
        erros++;
    } else {
        printf("Teste 1 passou!\n");
    }

    // Teste 2: checksum errado
    fsm_init(&ctx);
    uint8_t msg2[] = {0x02, 0x02, 'X', 'Y', 0x00, 0x03};
    for (int i = 0; i < sizeof(msg2); i++) fsm_process(&ctx, msg2[i]);
    if (ctx.state != ERROR) {
        printf("Teste 2 falhou! Esperado ERROR, obtido %d\n", ctx.state);
        erros++;
    } else {
        printf("Teste 2 passou!\n");
    }

    // Teste 3: sem ETX
    fsm_init(&ctx);
    uint8_t msg3[] = {0x02, 0x01, 'Z', 'Z'};
    for (int i = 0; i < sizeof(msg3); i++) fsm_process(&ctx, msg3[i]);
    if (ctx.state != ERROR) {
        printf("Teste 3 falhou! Esperado ERROR, obtido %d\n", ctx.state);
        erros++;
    } else {
        printf("Teste 3 passou!\n");
    }

    if (erros == 0) {
        printf("\nTodos os testes passaram!\n");
        return 0;
    } else {
        printf("\nFalharam %d testes.\n", erros);
        return 1;
    }
}
