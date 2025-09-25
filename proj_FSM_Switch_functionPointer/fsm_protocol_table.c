#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STX 0x02
#define ETX 0x03
#define MAX_DATA 255

// Estados
typedef enum {
    ST_WAIT_STX,
    ST_WAIT_QTD,
    ST_WAIT_DATA,
    ST_WAIT_CHK,
    ST_WAIT_ETX,
    ST_DONE,
    ST_ERROR,
    NR_STATES
} State_t;

// Estrutura da FSM
typedef struct {
    void (*ptrFunc)(uint8_t byte);
    State_t NextState;
} FSM_STATE_TABLE;

// Contexto (variáveis globais da FSM)
typedef struct {
    State_t state;
    uint8_t qtd;
    uint8_t data[MAX_DATA];
    uint8_t index;
    uint8_t chk;
} FSMContext;

static FSMContext ctx;

// --------- Funções de Ação ----------
void act_wait_stx(uint8_t byte) {
    if (byte == STX) {
        ctx.chk = 0;
        ctx.index = 0;
        printf("[FSM] STX recebido\n");
    } else {
        ctx.state = ST_ERROR;
    }
}

void act_wait_qtd(uint8_t byte) {
    ctx.qtd = byte;
    ctx.chk = byte;  // inicia checksum
    ctx.index = 0;
    printf("[FSM] QTD=%d\n", ctx.qtd);
}

void act_wait_data(uint8_t byte) {
    ctx.data[ctx.index++] = byte;
    ctx.chk += byte;
    printf("[FSM] Dado[%d]=%02X\n", ctx.index, byte);
}

void act_wait_chk(uint8_t byte) {
    if (byte == ctx.chk) {
        printf("[FSM] Checksum OK (%02X)\n", byte);
    } else {
        printf("[FSM] Checksum ERR (%02X esperado %02X)\n", byte, ctx.chk);
        ctx.state = ST_ERROR;
    }
}

void act_wait_etx(uint8_t byte) {
    if (byte == ETX) {
        printf("[FSM] ETX recebido, quadro valido!\n");
    } else {
        printf("[FSM] ERRO: ETX esperado, recebeu %02X\n", byte);
        ctx.state = ST_ERROR;
    }
}

// --------- Tabela de Transições ----------
FSM_STATE_TABLE StateTable[NR_STATES] = {
    { act_wait_stx, ST_WAIT_QTD   }, // ST_WAIT_STX
    { act_wait_qtd, ST_WAIT_DATA  }, // ST_WAIT_QTD
    { act_wait_data, ST_WAIT_DATA }, // ST_WAIT_DATA (fica até coletar qtd)
    { act_wait_chk, ST_WAIT_ETX   }, // ST_WAIT_CHK
    { act_wait_etx, ST_DONE       }, // ST_WAIT_ETX
    { NULL, ST_DONE },                // ST_DONE
    { NULL, ST_ERROR }                // ST_ERROR
};

// --------- Execução FSM ----------
void fsm_init(void) {
    ctx.state = ST_WAIT_STX;
    ctx.index = 0;
    ctx.chk = 0;
}

void fsm_process(uint8_t byte) {
    switch (ctx.state) {
        case ST_WAIT_DATA:
            StateTable[ctx.state].ptrFunc(byte);
            if (ctx.index >= ctx.qtd) ctx.state = ST_WAIT_CHK;
            break;
        case ST_WAIT_CHK:
            StateTable[ctx.state].ptrFunc(byte);
            if (ctx.state != ST_ERROR) ctx.state = ST_WAIT_ETX;
            break;
        case ST_WAIT_ETX:
            StateTable[ctx.state].ptrFunc(byte);
            if (ctx.state != ST_ERROR) ctx.state = ST_DONE;
            break;
        default:
            if (StateTable[ctx.state].ptrFunc != NULL)
                StateTable[ctx.state].ptrFunc(byte);
            ctx.state = StateTable[ctx.state].NextState;
            break;
    }
}

// --------- Teste (TDD) ----------
int main(void) {
    fsm_init();

    uint8_t frame[] = {
        STX, 3, 'A', 'B', 'C', (3+'A'+'B'+'C'), ETX
    };

    for (int i=0; i<sizeof(frame); i++) {
        fsm_process(frame[i]);
        if (ctx.state == ST_ERROR) {
            printf("[FSM] Erro no processamento!\n");
            break;
        }
    }

    if (ctx.state == ST_DONE) {
        printf("Quadro recebido com sucesso: %.*s\n", ctx.qtd, ctx.data);
    }
    return 0;
}
