#ifndef FSM_TBL_PROTOCOLO_H
#define FSM_TBL_PROTOCOLO_H

#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03
#define MAX_BUFFER 256
#define NUM_STATES 5

// Enumeracao dos estados formais da FSM
typedef enum {
    ST_STX = 0,
    ST_QTD,
    ST_DATA,
    ST_CHK,
    ST_ETX
} States;

// Tipo do ponteiro de funcao para cada acao de estado
typedef void (*StateAction)(uint8_t data);

// Estrutura do contexto da FSM com tabela de ponteiros de funcao
typedef struct {
    States state;                   // Estado atual da FSM
    uint8_t buffer[MAX_BUFFER];     // Buffer para armazenar os dados do pacote
    uint8_t chkBuffer;              // Acumulador do Checksum (XOR dos dados)
    int indBuffer;                  // Indice atual de escrita no buffer
    int qtdBuffer;                  // Quantidade de bytes de dados pendentes
    bool pacote_processado;         // Flag que sinaliza sucesso no processamento do pacote
    StateAction action[NUM_STATES]; // Tabela de ponteiros de funcao (vetor de acoes)
} FsmProtocoloPtr;

// Instancia global exportada do contexto da FSM
extern FsmProtocoloPtr sm;

// Funcoes de interface publica
void fsm_tbl_init(void);
void fsm_tbl_process_char(uint8_t data);
void fsm_tbl_process_bytes(const uint8_t *data, int qtd);

#endif // FSM_TBL_PROTOCOLO_H
