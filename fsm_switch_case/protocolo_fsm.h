#ifndef PROTOCOLO_FSM_H
#define PROTOCOLO_FSM_H

#define STX 0x02
#define ETX 0x03
#define MAX_BUFFER 256

/* Possiveis estados da maquina de estados de comunicacao */
typedef enum {
    ST_STX = 0, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} States;

/* Expondo as variáveis globais para que os testes consigam ler o estado atual */
extern States state;
extern unsigned char buffer[MAX_BUFFER];
extern int indBuffer;
extern int qtdBuffer;
extern unsigned char chkBuffer;
extern int pacote_valido;

/* Assinaturas das funções */
void reset_fsm(void);
void handleRx(unsigned char *data, int qtd);

#endif