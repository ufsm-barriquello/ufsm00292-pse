#include "fsm_tbl_protocolo.h"
#include <stddef.h>

// Instancia global da estrutura de controle da FSM
FsmProtocoloPtr sm;

// Prototipos estaticos das funcoes tratadoras de cada estado
static void stSTX(uint8_t data);
static void stQtd(uint8_t data);
static void stData(uint8_t data);
static void stChk(uint8_t data);
static void stETX(uint8_t data);

// Estado ST_STX: aguarda o delimitador inicial STX (0x02)
static void stSTX(uint8_t data) {
    if (data == STX) {
        sm.indBuffer = 0;
        sm.qtdBuffer = 0;
        sm.chkBuffer = 0;
        sm.pacote_processado = false;
        sm.state = ST_QTD;
    }
}

// Estado ST_QTD: armazena a quantidade de dados a receber
static void stQtd(uint8_t data) {
    sm.qtdBuffer = data;
    if (sm.qtdBuffer == 0) {
        sm.state = ST_CHK; // Se pacote tem 0 bytes de dados, avanca direto para CHK
    } else {
        sm.state = ST_DATA;
    }
}

// Estado ST_DATA: coleta os bytes de payload e calcula o checksum acumulado (XOR)
static void stData(uint8_t data) {
    if (sm.indBuffer < MAX_BUFFER) {
        sm.buffer[sm.indBuffer++] = data;
    }
    sm.chkBuffer ^= data;
    sm.qtdBuffer--;
    if (sm.qtdBuffer == 0) {
        sm.state = ST_CHK;
    }
}

// Estado ST_CHK: valida o checksum recebido em comparacao com o calculado
static void stChk(uint8_t data) {
    if (data == sm.chkBuffer) {
        sm.state = ST_ETX;
    } else {
        sm.state = ST_STX; // Checksum invalido: aborta e volta para o inicio
    }
}

// Estado ST_ETX: verifica o delimitador final ETX (0x03) e finaliza o pacote
static void stETX(uint8_t data) {
    if (data == ETX) {
        sm.pacote_processado = true;
    }
    sm.state = ST_STX; // Retorna sempre ao estado ST_STX ao finalizar
}

// Inicializa a FSM vinculando cada estado ao seu ponteiro de funcao na tabela
void fsm_tbl_init(void) {
    sm.state = ST_STX;
    sm.indBuffer = 0;
    sm.qtdBuffer = 0;
    sm.chkBuffer = 0;
    sm.pacote_processado = false;

    // Mapeamento direto: Indice do Estado -> Ponteiro de Funcao
    sm.action[ST_STX]  = stSTX;
    sm.action[ST_QTD]  = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK]  = stChk;
    sm.action[ST_ETX]  = stETX;
}

// Executa a acao do estado corrente indexando a tabela de ponteiros de funcao
void fsm_tbl_process_char(uint8_t data) {
    if (sm.state >= 0 && sm.state < NUM_STATES && sm.action[sm.state] != NULL) {
        sm.action[sm.state](data); // Invocacao via ponteiro de funcao
    }
}

// Processa uma sequencia de bytes
void fsm_tbl_process_bytes(const uint8_t *data, int qtd) {
    if (data == NULL) return;
    for (int i = 0; i < qtd; i++) {
        fsm_tbl_process_char(data[i]);
    }
}
