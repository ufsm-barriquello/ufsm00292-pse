#include "fsm_decoder.h"

// Possíveis estados da máquina
typedef enum {
    ST_STX = 0, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} States;

// Ponteiro de função que define a assinatura das ações de cada estado
typedef void (*Action)(unsigned char data); 

// Estrutura da FSM contendo a tabela de eventos (action array)
struct StateMachine {
    States state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chkBuffer;
    int indBuffer;
    int qtdBuffer;
    Action action[5]; // Tabela de estados/eventos
} sm;

// --- Implementação das funções de cada estado ---

static void stSTX(unsigned char data) {
    if (data == STX) {
        sm.indBuffer = sm.qtdBuffer = 0;
        sm.chkBuffer = 0; 
        sm.state = ST_QTD;
    }
}

static void stQtd(unsigned char data) {
    sm.qtdBuffer = data;
    sm.state = ST_DATA;
}

static void stData(unsigned char data) {
    sm.buffer[sm.indBuffer++] = data;
    sm.chkBuffer ^= data; // Checksum baseado em XOR
    
    if (--sm.qtdBuffer == 0) {
        sm.state = ST_CHK;
    }
}

static void stChk(unsigned char data) {
    if (data == sm.chkBuffer) {
        sm.state = ST_ETX;
    } else {
        sm.state = ST_STX; // Erro: descarta e volta pro início
    }
}

static void stETX(unsigned char data) {
    if (data == ETX) {
        handlePackage(sm.buffer, sm.indBuffer); // Chama a função externa
    }
    sm.state = ST_STX; // Finaliza e aguarda o próximo pacote
}

// --- Funções de controle públicas ---

void initSM(void) {
    sm.state = ST_STX;
    sm.buffer[0] = 0;
    sm.chkBuffer = 0;
    sm.indBuffer = 0;
    sm.qtdBuffer = 0;
    
    // Mapeando a tabela de estados (ponteiros de função)
    sm.action[ST_STX] = stSTX;
    sm.action[ST_QTD] = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK] = stChk;
    sm.action[ST_ETX] = stETX;
}

void handleRx(unsigned char *data, int qtd) {
    int i;
    for (i = 0; i < qtd; i++) {
        // A mágica acontece aqui: executa a função baseada no estado atual
        sm.action[sm.state](data[i]);
    }
}