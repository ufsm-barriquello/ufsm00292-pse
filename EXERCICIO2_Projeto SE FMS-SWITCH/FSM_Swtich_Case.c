#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <stdbool.h>
#include <string.h> 

// Framework de Testes (minUnit)
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                if (mensagem) return mensagem; } while (0)
                                
// Constantes do Protocolo
#define STX 0x02
#define ETX 0x03
#define MAX_DADOS_LEN 255

// Status de retorno do processamento do receptor
typedef enum {
    PACKET_IN_PROGRESS,
    PACKET_OK,
    PACKET_ERROR
} PacketStatus;

int testes_executados = 0;

// Função auxiliar para calcular o checksum
uint8_t calcular_checksum(const uint8_t* dados, uint8_t len) {
    uint8_t chk = 0;
    for (int i = 0; i < len; i++) {
        chk ^= dados[i];
    }
    return chk;
}

// Estados do Receptor
typedef enum {
    RECEPTOR_STATE_STX,
    RECEPTOR_STATE_QTD,
    RECEPTOR_STATE_DADOS,
    RECEPTOR_STATE_CHK,
    RECEPTOR_STATE_ETX
} ReceptorState;

// Estrutura de contexto do Receptor
typedef struct {
    ReceptorState state;
    uint8_t data_buffer[MAX_DADOS_LEN];
    uint8_t buffer_index;
    uint8_t expected_len;
    uint8_t calculated_checksum;
} ReceptorFSM;

// Protótipos das funções do Receptor
void receptor_init(ReceptorFSM* fsm);
PacketStatus receptor_process_byte(ReceptorFSM* fsm, uint8_t byte);


/* -------------------------- Testes do Receptor -------------------------- */

static char* teste_receptor_pacote_valido() {
    ReceptorFSM fsm;
    receptor_init(&fsm);

    uint8_t dados[] = {'T', 'D', 'D'};
    uint8_t checksum = calcular_checksum(dados, 3);
    uint8_t pacote[] = {STX, 3, 'T', 'D', 'D', checksum, ETX};
    
    PacketStatus status;
    for (int i = 0; i < sizeof(pacote) - 1; i++) {
        status = receptor_process_byte(&fsm, pacote[i]);
        verifica("ERRO: Pacote deveria estar em progresso", status == PACKET_IN_PROGRESS);
    }
    status = receptor_process_byte(&fsm, pacote[sizeof(pacote)-1]);
    verifica("ERRO: Pacote valido deveria retornar PACKET_OK", status == PACKET_OK);
    verifica("ERRO: Maquina deveria voltar ao estado STX", fsm.state == RECEPTOR_STATE_STX);

    return 0;
}

static char* teste_receptor_checksum_invalido() {
    ReceptorFSM fsm;
    receptor_init(&fsm);
    
    uint8_t pacote[] = {STX, 2, 'A', 'B', 0x99, ETX}; // Checksum correto seria 'A'^'B'
    
    receptor_process_byte(&fsm, pacote[0]); // STX
    receptor_process_byte(&fsm, pacote[1]); // QTD
    receptor_process_byte(&fsm, pacote[2]); // A
    receptor_process_byte(&fsm, pacote[3]); // B
    
    PacketStatus status = receptor_process_byte(&fsm, pacote[4]); // CHK inválido
    verifica("ERRO: Checksum invalido deveria retornar PACKET_ERROR", status == PACKET_ERROR);
    verifica("ERRO: Maquina deveria voltar ao estado STX apos erro", fsm.state == RECEPTOR_STATE_STX);

    return 0;
}

static char* teste_receptor_ignora_lixo_antes_do_stx() {
    ReceptorFSM fsm;
    receptor_init(&fsm);
    
    receptor_process_byte(&fsm, 0xAA);
    receptor_process_byte(&fsm, 0xFF);
    verifica("ERRO: Maquina deveria permanecer em STX com lixo", fsm.state == RECEPTOR_STATE_STX);
    
    PacketStatus status = receptor_process_byte(&fsm, STX);
    verifica("ERRO: Maquina deveria ir para estado QTD apos STX", fsm.state == RECEPTOR_STATE_QTD);
    verifica("ERRO: Status deveria ser IN_PROGRESS apos STX", status == PACKET_IN_PROGRESS);
    
    return 0;
}

/* -------------------------- Implementação do Receptor -------------------------- */

void receptor_init(ReceptorFSM* fsm) {
    fsm->state = RECEPTOR_STATE_STX;
    fsm->buffer_index = 0;
    fsm->expected_len = 0;
    fsm->calculated_checksum = 0;
    memset(fsm->data_buffer, 0, MAX_DADOS_LEN);
}

PacketStatus receptor_process_byte(ReceptorFSM* fsm, uint8_t byte) {
    switch (fsm->state) {
        case RECEPTOR_STATE_STX:
            if (byte == STX) {
                fsm->state = RECEPTOR_STATE_QTD;
            }
            break;
            
        case RECEPTOR_STATE_QTD:
            fsm->expected_len = byte;
            fsm->buffer_index = 0;
            fsm->calculated_checksum = 0;
            if (fsm->expected_len == 0) {
                 fsm->state = RECEPTOR_STATE_CHK;
            } else {
                 fsm->state = RECEPTOR_STATE_DADOS;
            }
            break;
            
        case RECEPTOR_STATE_DADOS:
            fsm->data_buffer[fsm->buffer_index] = byte;
            fsm->calculated_checksum ^= byte;
            fsm->buffer_index++;
            if (fsm->buffer_index == fsm->expected_len) {
                fsm->state = RECEPTOR_STATE_CHK;
            }
            break;
            
        case RECEPTOR_STATE_CHK:
            if (byte == fsm->calculated_checksum) {
                fsm->state = RECEPTOR_STATE_ETX;
            } else {
                receptor_init(fsm);
                return PACKET_ERROR;
            }
            break;

        case RECEPTOR_STATE_ETX:
            receptor_init(fsm); // Prepara para o próximo pacote
            if (byte == ETX) {
                return PACKET_OK;
            } else {
                return PACKET_ERROR;
            }
    }
    return PACKET_IN_PROGRESS;
}

// Estrutura de contexto do Transmissor (não precisa de enum de estados)
typedef struct {
    const uint8_t* data_ptr;
    uint8_t data_len;
    uint8_t sent_index;
    uint8_t checksum;
} Transmissor;

// Protótipos
void transmissor_build_packet(Transmissor* tx, const uint8_t* data, uint8_t len, uint8_t* buffer);


/* -------------------------- Testes do Transmissor -------------------------- */

static char* teste_transmissor_pacote_com_dados() {
    Transmissor tx;
    uint8_t dados[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t len = 4;
    
    // Tamanho do pacote = STX(1) + QTD(1) + DADOS(len) + CHK(1) + ETX(1)
    uint8_t packet_buffer[len + 5];
    
    transmissor_build_packet(&tx, dados, len, packet_buffer);
    
    uint8_t checksum_esperado = calcular_checksum(dados, len);
    
    verifica("ERRO Tx: STX incorreto", packet_buffer[0] == STX);
    verifica("ERRO Tx: QTD incorreto", packet_buffer[1] == len);
    verifica("ERRO Tx: DADO 0 incorreto", packet_buffer[2] == 0xDE);
    verifica("ERRO Tx: DADO 3 incorreto", packet_buffer[5] == 0xEF);
    verifica("ERRO Tx: CHK incorreto", packet_buffer[len + 2] == checksum_esperado);
    verifica("ERRO Tx: ETX incorreto", packet_buffer[len + 3] == ETX);
    
    return 0;
}

static char* teste_transmissor_pacote_sem_dados() {
    Transmissor tx;
    // Tamanho do pacote = STX(1) + QTD(1) + CHK(1) + ETX(1)
    uint8_t packet_buffer[4];
    
    transmissor_build_packet(&tx, NULL, 0, packet_buffer);
    
    verifica("ERRO Tx Vazio: STX incorreto", packet_buffer[0] == STX);
    verifica("ERRO Tx Vazio: QTD incorreto", packet_buffer[1] == 0);
    verifica("ERRO Tx Vazio: CHK incorreto", packet_buffer[2] == 0x00); // Checksum de 0 bytes é 0
    verifica("ERRO Tx Vazio: ETX incorreto", packet_buffer[3] == ETX);
    
    return 0;
}

/* -------------------------- Implementação do Transmissor -------------------------- */

void transmissor_build_packet(Transmissor* tx, const uint8_t* data, uint8_t len, uint8_t* buffer) {
    uint8_t index = 0;
    
    // 1. STX
    buffer[index++] = STX;
    
    // 2. QTD_DADOS
    buffer[index++] = len;
    
    // 3. DADOS
    for (int i = 0; i < len; i++) {
        buffer[index++] = data[i];
    }
    
    // 4. CHK
    buffer[index++] = calcular_checksum(data, len);
    
    // 5. ETX
    buffer[index++] = ETX;
}

static char* executa_todos_testes(void) {
    printf("--- Iniciando testes do Receptor ---\n");
    executa_teste(teste_receptor_pacote_valido);
    executa_teste(teste_receptor_checksum_invalido);
    executa_teste(teste_receptor_ignora_lixo_antes_do_stx);

    printf("--- Iniciando testes do Transmissor ---\n");
    executa_teste(teste_transmissor_pacote_com_dados);
    executa_teste(teste_transmissor_pacote_sem_dados);
    
    return 0;
}

int main() {
    char *resultado = executa_todos_testes();
    
    printf("\n--- Resultado Final ---\n");
    if (resultado != 0) {
        printf("FALHOU: %s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);
    
    return resultado != 0;
}