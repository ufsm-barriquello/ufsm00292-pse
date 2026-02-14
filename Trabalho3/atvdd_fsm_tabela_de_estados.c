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

// ====================================================
// === FSM COM PONTEIROS DE FUNÇÃO - VERSÃO SIMPLES ===
// ====================================================

// Estrutura do protocolo
typedef struct ProtocolHandler ProtocolHandler;

// Tipo do ponteiro de função para cada estado
// Recebe o handler e um byte, retorna um código
typedef int (*EstadoFuncao)(ProtocolHandler* handler, uint8_t byte);

typedef struct ProtocolHandler {
    EstadoFuncao estado_atual;     // Ponteiro para função do estado atual
    uint8_t qtd_dados;            // Quantidade de dados esperada
    uint8_t dados[MAX_DATA_SIZE]; // Buffer dos dados
    uint8_t dados_count;          // Quantos dados já recebemos
    uint8_t checksum_recv;        // Checksum que veio na mensagem
    uint8_t checksum_calc;        // Checksum que calculamos
    bool message_ready;           // Mensagem está pronta?
} ProtocolHandler;

// Declaração das funções de cada estado
int espera_stx(ProtocolHandler* handler, uint8_t byte);
int espera_qtd(ProtocolHandler* handler, uint8_t byte);
int espera_dados(ProtocolHandler* handler, uint8_t byte);
int espera_checksum(ProtocolHandler* handler, uint8_t byte);
int espera_etx(ProtocolHandler* handler, uint8_t byte);

// ===========================================
// === IMPLEMENTAÇÃO DAS FUNÇÕES DE ESTADO ===
// ===========================================

// Estado 1: Esperando STX (início da mensagem)
int espera_stx(ProtocolHandler* handler, uint8_t byte) {
    if (byte == STX_BYTE) {
        // STX encontrado! Próximo estado é esperar quantidade
        handler->estado_atual = espera_qtd;
        handler->dados_count = 0;
        handler->checksum_calc = 0;
        handler->message_ready = false;
    }
    // Se não for STX, continua esperando
    return PROTOCOL_WAITING;
}

// Estado 2: Esperando quantidade de dados
int espera_qtd(ProtocolHandler* handler, uint8_t byte) {
    if (byte > 0) {
        handler->qtd_dados = byte;
        handler->estado_atual = espera_dados;  // Vai esperar os dados
    } else {
        handler->estado_atual = espera_stx;    // Quantidade inválida, recomeça
    }
    return PROTOCOL_WAITING;
}

// Estado 3: Esperando dados
int espera_dados(ProtocolHandler* handler, uint8_t byte) {
    // Guarda o byte no buffer
    handler->dados[handler->dados_count] = byte;
    handler->checksum_calc += byte;  // Soma para o checksum
    handler->dados_count++;
    
    // Já recebeu todos os dados?
    if (handler->dados_count >= handler->qtd_dados) {
        handler->estado_atual = espera_checksum;  // Próximo: checksum
    }
    return PROTOCOL_WAITING;
}

// Estado 4: Esperando checksum
int espera_checksum(ProtocolHandler* handler, uint8_t byte) {
    handler->checksum_recv = byte;
    handler->estado_atual = espera_etx;  // Próximo: ETX
    return PROTOCOL_WAITING;
}

// Estado 5: Esperando ETX (fim da mensagem)
int espera_etx(ProtocolHandler* handler, uint8_t byte) {
    if (byte == ETX_BYTE) {
        // ETX correto, vamos verificar o checksum
        if (handler->checksum_calc == handler->checksum_recv) {
            handler->message_ready = true;
            handler->estado_atual = espera_stx;  // Volta ao início
            return PROTOCOL_SUCCESS;  // Mensagem OK!
        }
    }
    // Erro: ETX errado ou checksum errado
    handler->estado_atual = espera_stx;  // Volta ao início
    return PROTOCOL_ERROR;
}

// ========================================
// ========= FUNÇÕES DO PROTOCOLO =========
// ========================================

void protocol_init(ProtocolHandler* handler) {
    if (!handler) return;
    
    handler->estado_atual = espera_stx;  // Começa esperando STX
    handler->qtd_dados = 0;
    handler->dados_count = 0;
    handler->checksum_recv = 0;
    handler->checksum_calc = 0;
    handler->message_ready = false;
    memset(handler->dados, 0, MAX_DATA_SIZE);
}

int protocol_process_byte(ProtocolHandler* handler, uint8_t byte) {
    if (!handler || !handler->estado_atual) {
        return PROTOCOL_INVALID_PARAM;
    }
    
    // Chama a função do estado atual
    return handler->estado_atual(handler, byte);
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
// ============== TESTES ==================
// ========================================

static char * executa_testes(void);

int main() {
    printf("=== PROTOCOLO COM FSM - PONTEIROS DE FUNÇÃO ===\n");
    
    char *resultado = executa_testes();
    if (resultado != 0) {
        printf("FALHOU: %s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM!\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    return resultado != 0;
}

// Teste 1: Inicialização
static char * test_init(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    verifica("estado inicial deve ser espera_stx", handler.estado_atual == espera_stx);
    verifica("message_ready deve ser false", handler.message_ready == false);
    
    return 0;
}

// Teste 2: Mensagem válida simples
static char * test_mensagem_valida(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Mensagem: STX + 1 + 0x42 + 0x42 + ETX
    protocol_process_byte(&handler, STX_BYTE);  // STX
    protocol_process_byte(&handler, 1);         // QTD = 1 byte
    protocol_process_byte(&handler, 0x42);      // DADO = 0x42
    protocol_process_byte(&handler, 0x42);      // CHK = 0x42 (mesmo valor)
    int result = protocol_process_byte(&handler, ETX_BYTE); // ETX
    
    verifica("deve retornar sucesso", result == PROTOCOL_SUCCESS);
    verifica("mensagem deve estar pronta", protocol_message_ready(&handler));
    verifica("deve ter 1 byte de dados", protocol_get_data_count(&handler) == 1);
    verifica("dado deve ser 0x42", handler.dados[0] == 0x42);
    
    return 0;
}

// Teste 3: Checksum errado
static char * test_checksum_errado(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Mensagem com checksum errado
    protocol_process_byte(&handler, STX_BYTE);
    protocol_process_byte(&handler, 1);
    protocol_process_byte(&handler, 0x42);
    protocol_process_byte(&handler, 0x99);      // CHK errado
    int result = protocol_process_byte(&handler, ETX_BYTE);
    
    verifica("deve retornar erro", result == PROTOCOL_ERROR);
    verifica("mensagem não deve estar pronta", !protocol_message_ready(&handler));
    
    return 0;
}

// Teste 4: Ignora bytes até STX
static char * test_ignora_lixo(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Alguns bytes "lixo" antes do STX
    protocol_process_byte(&handler, 0xFF);
    protocol_process_byte(&handler, 0x00);
    verifica("deve continuar esperando STX", handler.estado_atual == espera_stx);
    
    // Agora o STX
    protocol_process_byte(&handler, STX_BYTE);
    verifica("deve mudar para espera_qtd", handler.estado_atual == espera_qtd);
    
    return 0;
}

// Teste 5: Duas mensagens seguidas
static char * test_duas_mensagens(void) {
    ProtocolHandler handler;
    protocol_init(&handler);
    
    // Primeira mensagem
    protocol_process_byte(&handler, STX_BYTE);
    protocol_process_byte(&handler, 1);
    protocol_process_byte(&handler, 0x10);
    protocol_process_byte(&handler, 0x10);
    int result1 = protocol_process_byte(&handler, ETX_BYTE);
    
    verifica("primeira mensagem OK", result1 == PROTOCOL_SUCCESS);
    verifica("primeira mensagem pronta", protocol_message_ready(&handler));
    
    // Segunda mensagem
    protocol_process_byte(&handler, STX_BYTE);
    protocol_process_byte(&handler, 1);
    protocol_process_byte(&handler, 0x20);
    protocol_process_byte(&handler, 0x20);
    int result2 = protocol_process_byte(&handler, ETX_BYTE);
    
    verifica("segunda mensagem OK", result2 == PROTOCOL_SUCCESS);
    verifica("dado da segunda mensagem", handler.dados[0] == 0x20);
    
    return 0;
}

static char * executa_testes(void) {
    executa_teste(test_init);
    executa_teste(test_mensagem_valida);
    executa_teste(test_checksum_errado);
    executa_teste(test_ignora_lixo);
    executa_teste(test_duas_mensagens);
    
    return 0;
}