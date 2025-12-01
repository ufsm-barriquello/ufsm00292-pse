#include <stdio.h>
#include <stdint.h>
#include <string.h>

// --- Definições do Protocolo ---
#define STX 0x02
#define ETX 0x03
#define MAX_SIZE 255

// --- Estados da FSM ---
typedef enum {
    S_WAIT_STX,
    S_WAIT_LEN,
    S_WAIT_PAYLOAD,
    S_WAIT_CSUM,
    S_WAIT_ETX,
    S_FINISH,
    S_FAIL,
    S_TOTAL
} State_t;

// --- Estrutura de Transição ---
typedef struct {
    void (*onByte)(uint8_t b);
    State_t next;
} Transition_t;

// --- Contexto da FSM ---
typedef struct {
    State_t state;
    uint8_t length;
    uint8_t data[MAX_SIZE];
    uint8_t idx;
    uint8_t checksum;
} Fsm_t;

static Fsm_t fsmCtx;

// --- Ações dos estados ---
void act_start(uint8_t b) {
    if (b == STX) {
        fsmCtx.idx = 0;
        fsmCtx.checksum = 0;
        printf("[FSM] STX detectado, iniciando quadro\n");
    } else {
        fsmCtx.state = S_FAIL;
    }
}

void act_len(uint8_t b) {
    fsmCtx.length = b;
    fsmCtx.checksum = b;   // inicia checksum
    fsmCtx.idx = 0;
    printf("[FSM] Comprimento do payload: %d\n", b);

    if (b == 0) {
        fsmCtx.state = S_WAIT_CSUM; // salta direto
    }
}

void act_payload(uint8_t b) {
    if (fsmCtx.idx < MAX_SIZE) {
        fsmCtx.data[fsmCtx.idx++] = b;
        fsmCtx.checksum ^= b; // diferente: usa XOR no checksum
        printf("[FSM] Byte %d armazenado: 0x%02X\n", fsmCtx.idx - 1, b);
    } else {
        fsmCtx.state = S_FAIL;
    }
}

void act_csum(uint8_t b) {
    if (b == fsmCtx.checksum) {
        printf("[FSM] Checksum válido (0x%02X)\n", b);
    } else {
        printf("[FSM] ERRO de checksum! recebido=0x%02X esperado=0x%02X\n", b, fsmCtx.checksum);
        fsmCtx.state = S_FAIL;
    }
}

void act_etx(uint8_t b) {
    if (b == ETX) {
        printf("[FSM] ETX recebido. Quadro completo!\n");
    } else {
        printf("[FSM] ERRO: esperado ETX, recebeu 0x%02X\n", b);
        fsmCtx.state = S_FAIL;
    }
}

// --- Tabela de transições ---
Transition_t transitions[S_TOTAL] = {
    [S_WAIT_STX]    = { act_start,   S_WAIT_LEN    },
    [S_WAIT_LEN]    = { act_len,     S_WAIT_PAYLOAD},
    [S_WAIT_PAYLOAD]= { act_payload, S_WAIT_PAYLOAD},
    [S_WAIT_CSUM]   = { act_csum,    S_WAIT_ETX    },
    [S_WAIT_ETX]    = { act_etx,     S_FINISH      },
    [S_FINISH]      = { NULL,        S_FINISH      },
    [S_FAIL]        = { NULL,        S_FAIL        }
};

// --- Controle ---
void fsm_init(void) {
    memset(&fsmCtx, 0, sizeof(Fsm_t));
    fsmCtx.state = S_WAIT_STX;
}

void fsm_process(uint8_t b) {
    State_t cur = fsmCtx.state;

    if (transitions[cur].onByte) {
        transitions[cur].onByte(b);
    }

    if (fsmCtx.state == cur) {
        if (cur == S_WAIT_PAYLOAD && fsmCtx.idx >= fsmCtx.length) {
            fsmCtx.state = S_WAIT_CSUM;
        } else {
            fsmCtx.state = transitions[cur].next;
        }
    }
}

// --- Teste ---
void run_case(const char *name, uint8_t *frame, size_t sz) {
    printf("\n== Teste: %s ==\n", name);
    fsm_init();
    for (size_t i = 0; i < sz; i++) {
        fsm_process(frame[i]);
        if (fsmCtx.state == S_FAIL) {
            printf(">>> Resultado: Falha!\n");
            return;
        }
    }
    if (fsmCtx.state == S_FINISH) {
        printf(">>> Resultado: Sucesso! Payload: '%.*s'\n", fsmCtx.length, fsmCtx.data);
    } else {
        printf(">>> Resultado: Incompleto (estado=%d)\n", fsmCtx.state);
    }
}

// --- Main ---
int main(void) {
    uint8_t ok_frame[] = {
        STX, 3, 'A', 'B', 'C', (uint8_t)(3 ^ 'A' ^ 'B' ^ 'C'), ETX
    };
    run_case("Quadro Correto", ok_frame, sizeof(ok_frame));

    return 0;
}
