#ifndef FSM_DECODER_H
#define FSM_DECODER_H

// Definições do protocolo
#define STX 0x02
#define ETX 0x03
#define MAX_BUFFER 256

// Protótipos das funções principais da Máquina de Estados
void initSM(void);
void handleRx(unsigned char *data, int qtd);

// Protótipo da função que processa o pacote final 
// (Deve ser implementada no main.c ou pela aplicação principal)
void handlePackage(unsigned char *buffer, int qtd);

#endif // FSM_DECODER_H