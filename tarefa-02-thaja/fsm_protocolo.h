#ifndef FSM_PROTOCOLO_H
#define FSM_PROTOCOLO_H

#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03
#define MAX_BUFFER 256

// Definição dos estados lógicos da FSM conforme material da UFSM
typedef enum {
    ST_STX = 0, // Aguardando caractere de início de texto (0x02)
    ST_QTD,     // Aguardando quantidade de bytes de dados
    ST_DATA,    // Recebendo os bytes de payload (dados)
    ST_CHK,     // Aguardando e validando o byte de Checksum
    ST_ETX      // Aguardando caractere de fim de texto (0x03)
} States;

// Estrutura de dados que armazena o contexto da nossa FSM
typedef struct {
    States state;                // Estado atual da máquina de estados
    uint8_t buffer[MAX_BUFFER];  // Armazena os dados úteis recebidos
    uint8_t chkBuffer;           // Valor calculado acumulativo do Checksum (XOR)
    int indBuffer;               // Índice de escrita no buffer de dados
    int qtdBuffer;               // Contador de bytes restantes a receber no estado DATA
    bool pacote_processado;      // Flag indicando se um pacote válido foi recebido
} FsmProtocolo;

// Inicializa a estrutura da FSM com os valores iniciais limpos
void fsm_init(FsmProtocolo *fsm);

// Processa um caractere isolado na máquina de estados FSM
void fsm_process_char(FsmProtocolo *fsm, uint8_t c);

#endif // FSM_PROTOCOLO_H
