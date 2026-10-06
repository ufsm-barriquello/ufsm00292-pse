#include "protocolo_fsm.h"

struct StateMachine sm;
int pacote_valido = 0;

void stSTX(unsigned char data) {
    if (data == STX) {
        sm.indBuffer = sm.qtdBuffer = 0;
        sm.chkBuffer = 0;
        sm.state = ST_QTD; // Início dos dados[cite: 5]
    }
}

void stQtd(unsigned char data) {
    sm.qtdBuffer = data;
    sm.state = ST_DATA; // Quantidade de dados[cite: 5]
}

void stData(unsigned char data) {
    sm.buffer[sm.indBuffer++] = data;
    sm.chkBuffer ^= data;
    if (--sm.qtdBuffer == 0) {
        sm.state = ST_CHK; // Calcula soma de verificação[cite: 5]
    }
}

void stChk(unsigned char data) {
    if (data == sm.chkBuffer) {
        sm.state = ST_ETX; // Testa checksum[cite: 5]
    } else {
        sm.state = ST_STX;
    }
}

void stETX(unsigned char data) {
    if (data == ETX) {
        pacote_valido = 1; // Imprime dados recebidos (neste caso, valida pacote)[cite: 5]
    }
    sm.state = ST_STX;
}

void initSM(void) {
    sm.state = ST_STX;
    sm.indBuffer = 0;
    sm.qtdBuffer = 0;
    sm.chkBuffer = 0;
    pacote_valido = 0;
    
    // Inicialização da FSM com ponteiros de função[cite: 5]
    sm.action[ST_STX] = stSTX;
    sm.action[ST_QTD] = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK] = stChk;
    sm.action[ST_ETX] = stETX;
}

void handleRx(unsigned char *data, int qtd) {
    for (int i = 0; i < qtd; i++) {
        // Execução da FSM[cite: 5]
        sm.action[sm.state](data[i]);
    }
}