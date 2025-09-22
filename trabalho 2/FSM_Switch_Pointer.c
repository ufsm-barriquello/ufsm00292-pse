#include <stdio.h>
#include <stdint.h>
#include <string.h>

// --- Definições do Protocolo ---
#define FRAME_START_BYTE 0x02 
#define FRAME_END_BYTE   0x03 
#define MAX_PAYLOAD_SIZE 255

// --- Estados da Máquina de Estados (FSM) ---
typedef enum {
    STATE_WAIT_STX,
    STATE_WAIT_LENGTH,
    STATE_WAIT_PAYLOAD,
    STATE_WAIT_CHECKSUM,
    STATE_WAIT_ETX,
    STATE_DONE,
    STATE_ERROR,
    NUM_STATES
} FsmState_t;

// --- Estrutura de Transição da FSM ---
typedef struct {
    void (*actionHandler)(uint8_t byte);
    FsmState_t nextState;
} FsmTransition_t;

// --- Contexto da FSM (variáveis de trabalho) ---
typedef struct {
    FsmState_t state;
    uint8_t payloadLength;
    uint8_t payload[MAX_PAYLOAD_SIZE];
    uint8_t payloadIndex;
    uint8_t checksum;
} FsmContext_t;

static FsmContext_t g_fsm;

// --- Funções de Ação (Callbacks para cada estado) ---
void action_reset_and_start(uint8_t byte) {
    if (byte == FRAME_START_BYTE) {
        g_fsm.checksum = 0;
        g_fsm.payloadIndex = 0;
        printf("[FSM] STX recebido. Iniciando quadro.\n");
    } else {
        g_fsm.state = STATE_ERROR; 
    }
}

void action_set_length(uint8_t byte) {
    g_fsm.payloadLength = byte;
    g_fsm.checksum = byte; 
    g_fsm.payloadIndex = 0;
    printf("[FSM] Tamanho do Payload: %d\n", g_fsm.payloadLength);
    if (g_fsm.payloadLength == 0) {
        g_fsm.state = STATE_WAIT_CHECKSUM;
    }
}

void action_store_payload(uint8_t byte) {
    g_fsm.payload[g_fsm.payloadIndex++] = byte;
    g_fsm.checksum += byte; 
    printf("[FSM] Payload[%d] = 0x%02X\n", g_fsm.payloadIndex - 1, byte);
}

void action_validate_checksum(uint8_t byte) {
    if (byte == g_fsm.checksum) {
        printf("[FSM] Checksum OK (Recebido: 0x%02X, Calculado: 0x%02X)\n", byte, g_fsm.checksum);
    } else {
        printf("[FSM] ERRO de Checksum! (Recebido: 0x%02X, Esperado: 0x%02X)\n", byte, g_fsm.checksum);
        g_fsm.state = STATE_ERROR;
    }
}

void action_validate_end(uint8_t byte) {
    if (byte == FRAME_END_BYTE) {
        printf("[FSM] ETX recebido. Quadro válido!\n");
    } else {
        printf("[FSM] ERRO: ETX esperado, mas recebeu 0x%02X\n", byte);
        g_fsm.state = STATE_ERROR;
    }
}

// --- Tabela de Transições da FSM ---
FsmTransition_t g_fsmTransitions[NUM_STATES] = {
    [STATE_WAIT_STX]      = { action_reset_and_start,   STATE_WAIT_LENGTH   },
    [STATE_WAIT_LENGTH]   = { action_set_length,        STATE_WAIT_PAYLOAD  },
    [STATE_WAIT_PAYLOAD]  = { action_store_payload,     STATE_WAIT_PAYLOAD  }, 
    [STATE_WAIT_CHECKSUM] = { action_validate_checksum, STATE_WAIT_ETX      },
    [STATE_WAIT_ETX]      = { action_validate_end,      STATE_DONE          },
    [STATE_DONE]          = { NULL,                     STATE_DONE          },
    [STATE_ERROR]         = { NULL,                     STATE_ERROR         }
};

// --- Funções de Controle da FSM ---
void fsm_init(void) {
    g_fsm.state = STATE_WAIT_STX;
    g_fsm.payloadIndex = 0;
    g_fsm.checksum = 0;
    g_fsm.payloadLength = 0;
    memset(g_fsm.payload, 0, MAX_PAYLOAD_SIZE);
}

void fsm_handle_byte(uint8_t byte) {
    FsmState_t currentState = g_fsm.state;

    if (g_fsmTransitions[currentState].actionHandler != NULL) {
        g_fsmTransitions[currentState].actionHandler(byte);
    }

    // A lógica de transição de estado precisa de tratamento especial para alguns casos.
    // Se o estado não foi alterado por uma condição de erro ou especial (como payload de tamanho 0)...
    if (g_fsm.state == currentState) {
        if (currentState == STATE_WAIT_PAYLOAD) {
            if (g_fsm.payloadIndex >= g_fsm.payloadLength) {
                g_fsm.state = STATE_WAIT_CHECKSUM; 
            }
        } else {
            g_fsm.state = g_fsmTransitions[currentState].nextState;
        }
    }
}

// --- Função Auxiliar para Executar Testes ---
void run_test(const char* test_name, uint8_t* frame, size_t frame_size) {
    printf("\n--- Teste: %s ---\n", test_name);
    fsm_init(); // Reseta a FSM antes de cada teste

    for (size_t i = 0; i < frame_size; i++) {
        fsm_handle_byte(frame[i]);
        if (g_fsm.state == STATE_ERROR) {
            printf(">>> [RESULTADO] Falha no processamento!\n");
            return; // Interrompe o teste em caso de erro
        }
    }

    if (g_fsm.state == STATE_DONE) {
        printf(">>> [RESULTADO] Sucesso! Payload: '%.*s'\n", g_fsm.payloadLength, g_fsm.payload);
    } else {
        printf(">>> [RESULTADO] Teste finalizado, mas quadro incompleto. Estado final: %d\n", g_fsm.state);
    }
}


// --- Função Principal com Múltiplos Testes (TDD) ---
int main(void) {
    uint8_t valid_frame[] = {
        FRAME_START_BYTE, 3, 'A', 'B', 'C', (uint8_t)(3+'A'+'B'+'C'), FRAME_END_BYTE
    };
    run_test("Quadro Perfeito", valid_frame, sizeof(valid_frame));



    return 0;
}