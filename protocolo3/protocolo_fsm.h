#ifndef PROTOCOLO_FSM_H
#define PROTOCOLO_FSM_H

#define STX 0x02
#define ETX 0x03
#define MAX_BUFFER 256

/* Possiveis estados da maquina de estados de comunicacao[cite: 5] */
typedef enum {
    ST_STX = 0, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} States;

/* Ponteiro de função[cite: 5] */
typedef void (*Action)(unsigned char data);

/* Estrutura da FSM[cite: 5] */
struct StateMachine {
    States state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chkBuffer;
    int indBuffer;
    int qtdBuffer;
    Action action[5];
};

extern struct StateMachine sm;
extern int pacote_valido;

void initSM(void);
void handleRx(unsigned char *data, int qtd);

#endif