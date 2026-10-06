#include "protocolo_fsm.h"

// Inicializando as variáveis da máquina
States state = ST_STX;
unsigned char buffer[MAX_BUFFER];
int indBuffer = 0, qtdBuffer = 0;
unsigned char chkBuffer = 0;
int pacote_valido = 0; 

void reset_fsm(void) {
    state = ST_STX;
    indBuffer = 0;
    qtdBuffer = 0;
    chkBuffer = 0;
    pacote_valido = 0;
}

/* Implementação da máquina de estados usando switch */
void handleRx(unsigned char *data, int qtd) {
    int i;
    for (i = 0; i < qtd; i++) {
        switch (state) {
            case ST_STX:
                if (data[i] == STX) {
                    indBuffer = qtdBuffer = chkBuffer = 0;
                    state = ST_QTD;
                }
                break;
            case ST_QTD:
                qtdBuffer = data[i];
                state = ST_DATA;
                break;
            case ST_DATA:
                buffer[indBuffer++] = data[i];
                chkBuffer ^= data[i]; // Calcula soma de verificação via XOR
                if (--qtdBuffer == 0) {
                    state = ST_CHK;
                }
                break;
            case ST_CHK:
                if (data[i] == chkBuffer) {
                    state = ST_ETX;
                } else {
                    state = ST_STX; // Falha no checksum, reseta
                }
                break;
            case ST_ETX:
                if (data[i] == ETX) {
                    pacote_valido = 1; // Sucesso!
                }
                state = ST_STX;
                break;
        }
    }
}