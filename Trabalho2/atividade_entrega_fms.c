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
// PROTOCOL STATE MACHINE IMPLEMENTATION
// ========================================

typedef enum {
    STATE_WAIT_STX,     // Aguardando STX (0x02)
    STATE_WAIT_QTD,     // Aguardando quantidade de dados
    STATE_WAIT_DATA,    // Aguardando dados
    STATE_WAIT_CHK,     // Aguardando checksum
    STATE_WAIT_ETX,     // Aguardando ETX (0x03)
    STATE_MESSAGE_OK,   // Mensagem válida recebida
    STATE_MESSAGE_ERROR // Erro na mensagem
} ProtocolState;

typedef struct {
    ProtocolState state;        // Estado atual
    uint8_t qtd_dados;         // Quantidade esperada de dados
    uint8_t dados[MAX_DATA_SIZE]; // Buffer para dados recebidos
    uint8_t dados_count;       // Contador de dados recebidos
    uint8_t checksum_recv;     // Checksum recebido
    uint8_t checksum_calc;     // Checksum calculado
    bool message_ready;        // Flag de mensagem pronta
} ProtocolHandler;

// Function declarations
void protocol_init(ProtocolHandler* handler);
int protocol_process_byte(ProtocolHandler* handler, uint8_t byte);
int protocol_create_message(uint8_t* dados, uint8_t qtd, uint8_t* buffer, uint8_t* buffer_size);
uint8_t protocol_calculate_checksum(uint8_t* dados, uint8_t qtd);
bool protocol_message_ready(ProtocolHandler* handler);
void protocol_reset(ProtocolHandler* handler);
uint8_t* protocol_get_data(ProtocolHandler* handler);
uint8_t protocol_get_data_count(ProtocolHandler* handler);

// ========================================
// PROTOCOL IMPLEMENTATIONS
// ========================================

void protocol_init(ProtocolHandler* handler) {
    if (!handler) return;
    
    handler->state = STATE_WAIT_STX;
    handler->qtd_dados = 0;
    handler->dados_count = 0;
    handler->checksum_recv = 0;
    handler->checksum_calc = 0;
    handler->message_ready = false;
    memset(handler->dados, 0, MAX_DATA_SIZE);
}

void protocol_reset(ProtocolHandler* handler) {
    if (!handler) return;
    
    handler->state = STATE_WAIT_STX;
    handler->dados_count = 0;
    handler->checksum_calc = 0;
    handler->message_ready = false;
}

int protocol_process_byte(ProtocolHandler* handler, uint8_t byte) {
    if (!handler) return PROTOCOL_INVALID_PARAM;
    
    switch (handler->state) {
        case STATE_WAIT_STX:
            if (byte == STX_BYTE) {
                handler->state = STATE_WAIT_QTD;
                handler->dados_count = 0;
                handler->checksum_calc = 0;
                handler->message_ready = false;
            }
            // Ignora outros bytes
            break;
            
        case STATE_WAIT_QTD:
            if (byte > 0) {  // uint8_t sempre será <= 255
                handler->qtd_dados = byte;
                handler->state = STATE_WAIT_DATA;
            } else {
                handler->state = STATE_WAIT_STX; // Erro: quantidade inválida
            }
            break;
            
        case STATE_WAIT_DATA:
            handler->dados[handler->dados_count] = byte;
            handler->checksum_calc += byte;  // Acumula checksum
            handler->dados_count++;
            
            if (handler->dados_count >= handler->qtd_dados) {
                handler->state = STATE_WAIT_CHK;
            }
            break;
            
        case STATE_WAIT_CHK:
            handler->checksum_recv = byte;
            handler->state = STATE_WAIT_ETX;
            break;
            
        case STATE_WAIT_ETX:
            if (byte == ETX_BYTE) {
                if (handler->checksum_calc == handler->checksum_recv) {
                    handler->state = STATE_MESSAGE_OK;
                    handler->message_ready = true;
                    return PROTOCOL_SUCCESS;
                }
            }
            handler->state = STATE_MESSAGE_ERROR;
            return PROTOCOL_ERROR;
            
        case STATE_MESSAGE_OK:
        case STATE_MESSAGE_ERROR:
            // Reset automático para próxima mensagem
            protocol_reset(handler);
            break;
    }
    
    return PROTOCOL_WAITING;
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
    
    verifica("erro: estado inicial deve ser WAIT_STX", handler.state == STATE_WAIT_STX);
    verifica("erro: message_ready deve ser false", handler.message_ready == false);
    verifica("erro: dados_count deve ser 0", handler.dados_count == 0);
    
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
    verifica("erro: deve permanecer em WAIT_STX", handler.state == STATE_WAIT_STX);
    
    // STX válido
    protocol_process_byte(&handler, STX_BYTE);
    verifica("erro: deve ir para WAIT_QTD", handler.state == STATE_WAIT_QTD);
    
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
    
    // Teste de transições de estado
    verifica("erro: estado inicial", handler.state == STATE_WAIT_STX);
    
    protocol_process_byte(&handler, STX_BYTE);
    verifica("erro: após STX", handler.state == STATE_WAIT_QTD);
    
    protocol_process_byte(&handler, 1);
    verifica("erro: após QTD", handler.state == STATE_WAIT_DATA);
    
    protocol_process_byte(&handler, 0x42);
    verifica("erro: após dados", handler.state == STATE_WAIT_CHK);
    
    protocol_process_byte(&handler, 0x42);
    verifica("erro: após CHK", handler.state == STATE_WAIT_ETX);
    
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
    protocol_process_byte(&handler, ETX_BYTE);
    
    verifica("erro: deve estar em MESSAGE_OK", handler.state == STATE_MESSAGE_OK);
    
    // Próximo byte deve resetar
    protocol_process_byte(&handler, 0x00);
    verifica("erro: deve resetar para WAIT_STX", handler.state == STATE_WAIT_STX);
    
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
    
    return 0;
}
