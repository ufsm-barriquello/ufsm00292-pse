#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Protocol constants
#define STX_BYTE 0x02
#define ETX_BYTE 0x03
#define MAX_DATA_SIZE 256

// Return codes
#define PROTOCOL_SUCCESS 0
#define PROTOCOL_ERROR -1
#define PROTOCOL_WAITING -2
#define PROTOCOL_INVALID_PARAM -3

/* macros de testes - baseado em minUnit: www.jera.com/techinfo/jtns/jtn002.html */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                if (mensagem) return mensagem; } while (0)
int testes_executados = 0;

// ========================================
// FUNCTION POINTER STATE MACHINE IMPLEMENTATION
// ========================================

// Forward declaration of the handler structure
typedef struct ProtocolHandler ProtocolHandler;

// Function pointer type for state functions
typedef int (*StateFunction)(ProtocolHandler* handler, uint8_t byte);

typedef enum {
    ST_STX = 0,    // Aguardando STX (0x02)
    ST_QTD,        // Aguardando quantidade de dados
    ST_DATA,       // Aguardando dados
    ST_CHK,        // Aguardando checksum
    ST_ETX,        // Aguardando ETX (0x03)
    NUM_STATES     // Total number of states
} ProtocolStates;

struct ProtocolHandler {
    ProtocolStates current_state;      // Estado atual (índice)
    StateFunction state_functions[NUM_STATES]; // Array de ponteiros para funções
    uint8_t qtd_dados;                 // Quantidade esperada de dados
    uint8_t dados[MAX_DATA_SIZE];      // Buffer para dados recebidos
    uint8_t dados_count;               // Contador de dados recebidos
    uint8_t checksum_recv;             // Checksum recebido
    uint8_t checksum_calc;             // Checksum calculado
    bool message_ready;                // Flag de mensagem pronta
    int last_result;                   // Último resultado de processamento
};

// ========================================
// STATE FUNCTION DECLARATIONS
// ========================================

int state_wait_stx(ProtocolHandler* handler, uint8_t byte);
int state_wait_qtd(ProtocolHandler* handler, uint8_t byte);
int state_wait_data(ProtocolHandler* handler, uint8_t byte);
int state_wait_chk(ProtocolHandler* handler, uint8_t byte);
int state_wait_etx(ProtocolHandler* handler, uint8_t byte);

// ========================================
// PROTOCOL FUNCTION DECLARATIONS
// ========================================

void protocol_init(ProtocolHandler* handler);
int protocol_process_byte(ProtocolHandler* handler, uint8_t byte);
int protocol_create_message(uint8_t* dados, uint8_t qtd, uint8_t* buffer, uint8_t* buffer_size);
uint8_t protocol_calculate_checksum(uint8_t* dados, uint8_t qtd);
bool protocol_message_ready(ProtocolHandler* handler);
void protocol_reset(ProtocolHandler* handler);
uint8_t* protocol_get_data(ProtocolHandler* handler);
uint8_t protocol_get_data_count(ProtocolHandler* handler);

// ========================================
// STATE FUNCTION IMPLEMENTATIONS
// ========================================

int state_wait_stx(ProtocolHandler* handler, uint8_t byte) {
    if (byte == STX_BYTE) {
        handler->current_state = ST_QTD;
        handler->dados_count = 0;
        handler->checksum_calc = 0;
        handler->message_ready = false;
    }
    // Ignora outros bytes - permanece no mesmo estado
    return PROTOCOL_WAITING;
}

int state_wait_qtd(ProtocolHandler* handler, uint8_t byte) {
    if (byte > 0) {  // uint8_t sempre será <= 255
        handler->qtd_dados = byte;
        handler->current_state = ST_DATA;
        return PROTOCOL_WAITING;
    } else {
        // Erro: quantidade inválida - volta para início
        handler->current_state = ST_STX;
        return PROTOCOL_WAITING;
    }
}

int state_wait_data(ProtocolHandler* handler, uint8_t byte) {
    handler->dados[handler->dados_count] = byte;
    handler->checksum_calc += byte;  // Acumula checksum
    handler->dados_count++;
    
    if (handler->dados_count >= handler->qtd_dados) {
        handler->current_state = ST_CHK;
    }
    return PROTOCOL_WAITING;
}

int state_wait_chk(ProtocolHandler* handler, uint8_t byte) {
    handler->checksum_recv = byte;
    handler->current_state = ST_ETX;
    return PROTOCOL_WAITING;
}

int state_wait_etx(ProtocolHandler* handler, uint8_t byte) {
    if (byte == ETX_BYTE) {
        if (handler->checksum_calc == handler->checksum_recv) {
            handler->message_ready = true;
            // Reset para próxima mensagem
            handler->current_state = ST_STX;
            return PROTOCOL_SUCCESS;
        }
    }
    // Erro - reset para início
    handler->current_state = ST_STX;
    return PROTOCOL_ERROR;
}

// ========================================
// PROTOCOL IMPLEMENTATIONS
// ========================================

void protocol_init(ProtocolHandler* handler) {
    if (!handler) return;
    
    // Initialize state function pointers
    handler->state_functions[ST_STX] = state_wait_stx;
    handler->state_functions[ST_QTD] = state_wait_qtd;
    handler->state_functions[ST_DATA] = state_wait_data;
    handler->state_functions[ST_CHK] = state_wait_chk;
    handler->state_functions[ST_ETX] = state_wait_etx;
    
    // Initialize state and data
    handler->current_state = ST_STX;
    handler->qtd_dados = 0;
    handler->dados_count = 0;
    handler->checksum_recv = 0;
    handler->checksum_calc = 0;
    handler->message_ready = false;
    handler->last_result = PROTOCOL_WAITING;
    memset(handler->dados, 0, MAX_DATA_SIZE);
}

void protocol_reset(ProtocolHandler* handler) {
    if (!handler) return;
    
    handler->current_state = ST_STX;
    handler->dados_count = 0;
    handler->checksum_calc = 0;
    handler->message_ready = false;
    handler->last_result = PROTOCOL_WAITING;
}

int protocol_process_byte(ProtocolHandler* handler, uint8_t byte) {
    if (!handler) return PROTOCOL_INVALID_PARAM;
    
    // Call the current state function using function pointer
    StateFunction current_function = handler->state_functions[handler->current_state];
    if (current_function) {
        handler->last_result = current_function(handler, byte);
        return handler->last_result;
    }
    
    return PROTOCOL_ERROR;
}

uint8_t protocol_calculate_checksum(uint8_t* dados, uint8_t qtd) {
    if (!dados || qtd == 0) return 0;
    
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < qtd; i++) {
        checksum += dados[i];
    }
    return checksum;
}

int protocol_create_message(uint8_t* dados, uint8_t qtd, uint8_t* buffer, uint8_t* buffer_size) {
    if (!dados || !buffer || !buffer_size || qtd == 0) {
        return PROTOCOL_INVALID_PARAM;
    }
    
    uint8_t checksum = protocol_calculate_checksum(dados, qtd);
    uint8_t msg_size = 5 + qtd; // STX + QTD + dados + CHK + ETX
    
    if (*buffer_size < msg_size) {
        return PROTOCOL_ERROR;
    }
    
    buffer[0] = STX_BYTE;
    buffer[1] = qtd;
    memcpy(&buffer[2], dados, qtd);
    buffer[2 + qtd] = checksum;
    buffer[3 + qtd] = ETX_BYTE;
    
    *buffer_size = msg_size;
    return PROTOCOL_SUCCESS;
}

bool protocol_message_ready(ProtocolHandler* handler) {
    return handler ? handler->message_ready : false;
}

uint8_t* protocol_get_data(ProtocolHandler* handler) {
    return handler ? handler->dados : NULL;
}

uint8_t protocol_get_data_count(ProtocolHandler* handler) {
    return handler ? handler->qtd_dados : 0;
}

// ========================================
// PROTOCOL TESTS - TDD IMPLEMENTATION
// ========================================

static char * executa_testes(void);

int main() {
    char *resultado = executa_testes();
    if (resultado != 0) {
        printf("%s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    return resultado != 0;
}

/* TESTES BÁSICOS DO PROTOCOLO DE COMUNICAÇÃO */
/*********************************************/

static char * test_protocol_init(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    verifica("erro: estado inicial deve ser ST_STX", handler.current_state == ST_STX);
    verifica("erro: message_ready deve ser false", handler.message_ready == false);
    verifica("erro: dados_count deve ser 0", handler.dados_count == 0);
    verifica("erro: função ST_STX deve estar definida", handler.state_functions[ST_STX] != NULL);
    verifica("erro: função ST_QTD deve estar definida", handler.state_functions[ST_QTD] != NULL);
    
    return 0;
}

static char * test_receive_valid_message(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Simular mensagem: STX + QTD(2) + DADOS(0x10,0x20) + CHK(0x30) + ETX
    protocol_process_byte(&handler, STX_BYTE);      // STX
    protocol_process_byte(&handler, 2);             // QTD = 2 bytes
    protocol_process_byte(&handler, 0x10);          // DADO 1
    protocol_process_byte(&handler, 0x20);          // DADO 2
    protocol_process_byte(&handler, 0x30);          // CHK = 0x10 + 0x20 = 0x30
    int result = protocol_process_byte(&handler, ETX_BYTE); // ETX
    
    verifica("erro: mensagem deve ser válida", result == PROTOCOL_SUCCESS);
    verifica("erro: mensagem deve estar pronta", protocol_message_ready(&handler) == true);
    verifica("erro: quantidade incorreta", protocol_get_data_count(&handler) == 2);
    verifica("erro: primeiro dado incorreto", handler.dados[0] == 0x10);
    verifica("erro: segundo dado incorreto", handler.dados[1] == 0x20);
    
    return 0;
}

static char * test_invalid_checksum(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Mensagem com checksum incorreto
    protocol_process_byte(&handler, STX_BYTE);      // STX
    protocol_process_byte(&handler, 2);             // QTD = 2
    protocol_process_byte(&handler, 0x10);          // DADO 1
    protocol_process_byte(&handler, 0x20);          // DADO 2
    protocol_process_byte(&handler, 0xFF);          // CHK incorreto
    int result = protocol_process_byte(&handler, ETX_BYTE); // ETX
    
    verifica("erro: mensagem deve ser inválida", result == PROTOCOL_ERROR);
    verifica("erro: mensagem não deve estar pronta", protocol_message_ready(&handler) == false);
    
    return 0;
}

static char * test_invalid_stx(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Bytes inválidos antes do STX
    protocol_process_byte(&handler, 0xFF);
    protocol_process_byte(&handler, 0x00);
    verifica("erro: deve permanecer em ST_STX", handler.current_state == ST_STX);
    
    // STX válido
    protocol_process_byte(&handler, STX_BYTE);
    verifica("erro: deve ir para ST_QTD", handler.current_state == ST_QTD);
    
    return 0;
}

static char * test_create_message(void) {
    uint8_t dados[] = {0xAA, 0xBB, 0xCC};
    uint8_t buffer[10];
    uint8_t buffer_size = 10;
    
    int result = protocol_create_message(dados, 3, buffer, &buffer_size);
    
    verifica("erro: criação deve ser bem-sucedida", result == PROTOCOL_SUCCESS);
    verifica("erro: tamanho incorreto", buffer_size == 8); // STX+QTD+3dados+CHK+ETX = 8
    verifica("erro: STX incorreto", buffer[0] == STX_BYTE);
    verifica("erro: QTD incorreta", buffer[1] == 3);
    verifica("erro: primeiro dado incorreto", buffer[2] == 0xAA);
    verifica("erro: segundo dado incorreto", buffer[3] == 0xBB);
    verifica("erro: terceiro dado incorreto", buffer[4] == 0xCC);
    verifica("erro: ETX incorreto", buffer[6] == ETX_BYTE);
    
    return 0;
}

static char * test_calculate_checksum(void) {
    uint8_t dados[] = {0x10, 0x20, 0x30};
    uint8_t checksum = protocol_calculate_checksum(dados, 3);
    
    verifica("erro: checksum incorreto", checksum == 0x60); // 0x10+0x20+0x30
    
    return 0;
}

static char * test_state_transitions(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Teste de transições de estado usando function pointers
    verifica("erro: estado inicial", handler.current_state == ST_STX);
    
    protocol_process_byte(&handler, STX_BYTE);
    verifica("erro: após STX", handler.current_state == ST_QTD);
    
    protocol_process_byte(&handler, 1);
    verifica("erro: após QTD", handler.current_state == ST_DATA);
    
    protocol_process_byte(&handler, 0x42);
    verifica("erro: após dados", handler.current_state == ST_CHK);
    
    protocol_process_byte(&handler, 0x42);
    verifica("erro: após CHK", handler.current_state == ST_ETX);
    
    return 0;
}

static char * test_reset_after_message(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Processar mensagem completa
    protocol_process_byte(&handler, STX_BYTE);
    protocol_process_byte(&handler, 1);
    protocol_process_byte(&handler, 0x42);
    protocol_process_byte(&handler, 0x42);
    int result = protocol_process_byte(&handler, ETX_BYTE);
    
    verifica("erro: resultado deve ser SUCCESS", result == PROTOCOL_SUCCESS);
    verifica("erro: deve resetar automaticamente para ST_STX", handler.current_state == ST_STX);
    
    return 0;
}

static char * test_function_pointers(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Verificar se os ponteiros de função estão corretos
    verifica("erro: função ST_STX", handler.state_functions[ST_STX] == state_wait_stx);
    verifica("erro: função ST_QTD", handler.state_functions[ST_QTD] == state_wait_qtd);
    verifica("erro: função ST_DATA", handler.state_functions[ST_DATA] == state_wait_data);
    verifica("erro: função ST_CHK", handler.state_functions[ST_CHK] == state_wait_chk);
    verifica("erro: função ST_ETX", handler.state_functions[ST_ETX] == state_wait_etx);
    
    return 0;
}

/***********************************************/

static char * executa_testes(void) {
    executa_teste(test_protocol_init);
    executa_teste(test_receive_valid_message);
    executa_teste(test_invalid_checksum);
    executa_teste(test_invalid_stx);
    executa_teste(test_create_message);
    executa_teste(test_calculate_checksum);
    executa_teste(test_state_transitions);
    executa_teste(test_reset_after_message);
    executa_teste(test_function_pointers);
    
    return 0;
}
