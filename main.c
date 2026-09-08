#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define STX 0x02
#define ETX 0x03
#define MAX_BUFFER 256

// 1. Definição dos Estados
typedef enum {
    ST_STX = 0, 
    ST_QTD, 
    ST_DATA, 
    ST_CHK, 
    ST_ETX
} States; 

// 2. Definição do Ponteiro de Função
typedef void (*Action)(unsigned char data); // Ponteiro de função

// 3. Estrutura da Máquina de Estados (FSM)
struct StateMachine {
    States state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chkBuffer;
    int indBuffer;
    int qtdBuffer;
    Action action[5]; // Tabela de estados/eventos (Array de ponteiros)
} sm; // Estrutura da FSM

bool pacote_valido_recebido = false;

// ==========================================
// FUNÇÕES DE TRANSIÇÃO DE ESTADO
// ==========================================

void stSTX(unsigned char data) {
    if (data == STX) {
        sm.indBuffer = sm.qtdBuffer = 0;
        sm.chkBuffer = 0;
        sm.state = ST_QTD; // Início dos dados
        pacote_valido_recebido = false;
    }
}

void stQtd(unsigned char data) {
    sm.qtdBuffer = data; // Quantidade de dados
    sm.chkBuffer ^= data; // Inicializa o checksum (XOR)
    sm.state = (sm.qtdBuffer > 0) ? ST_DATA : ST_CHK; 
}

void stData(unsigned char data) {
    sm.buffer[sm.indBuffer++] = data;
    sm.chkBuffer ^= data; // Calcula soma de verificação (checksum)
    
    if (sm.indBuffer == sm.qtdBuffer) {
        sm.state = ST_CHK;
    }
}

void stChk(unsigned char data) {
    if (data == sm.chkBuffer) {
        sm.state = ST_ETX; // Testa checksum
    } else {
        sm.state = ST_STX; // Falha no checksum, aborta
    }
}

void stETX(unsigned char data) {
    if (data == ETX) {
        pacote_valido_recebido = true; // Simula a impressão/tratamento de dados
    }
    sm.state = ST_STX;
}

// INICIALIZAÇÃO E EXECUÇÃO


void initSM() {
    sm.state = ST_STX;
    sm.buffer[0] = 0;
    sm.chkBuffer = 0;
    sm.indBuffer = 0;
    sm.qtdBuffer = 0;
    
    // Mapeamento da tabela de estados para as funções correspondentes
    sm.action[ST_STX] = stSTX;
    sm.action[ST_QTD] = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK] = stChk;
    sm.action[ST_ETX] = stETX; // Inicialização da FSM
}

void handleRx(unsigned char *data, int qtd) {
    for (int i = 0; i < qtd; i++) {
        // Execução da FSM chamando a função correspondente ao estado atual
        sm.action[sm.state](data[i]); // Execução da FSM
    }
}

// TESTES TDD

void test_recepcao_pacote_valido() {
    initSM();
    
    // Montando um pacote: STX | QTD (2) | DADOS (0x10, 0x20) | CHK (0x02^0x10^0x20 = 0x32) | ETX
    unsigned char pacote_teste[] = {STX, 0x02, 0x10, 0x20, 0x32, ETX};
    
    handleRx(pacote_teste, sizeof(pacote_teste));
    
    assert(pacote_valido_recebido == true);
    printf("[OK] TDD - Pacote valido decodificado com sucesso usando ponteiros de funcao.\n");
}

void test_falha_de_checksum() {
    initSM();
    
    // Montando pacote com Checksum incorreto (enviado 0xFF em vez de 0x32)
    unsigned char pacote_erro[] = {STX, 0x02, 0x10, 0x20, 0xFF, ETX};
    
    handleRx(pacote_erro, sizeof(pacote_erro));
    
    assert(pacote_valido_recebido == false);
    assert(sm.state == ST_STX); // Deve ter resetado a maquina
    printf("[OK] TDD - Pacote invalido barrado no Checksum e FSM resetada.\n");
}

// ==========================================
// MAIN
// ==========================================

int main() {
    printf("\n INICIANDO TESTES TDD (FSM COM PONTEIROS DE FUNCAO) n\n");
    
    test_recepcao_pacote_valido();
    test_falha_de_checksum();
    
    printf("         TODOS OS TESTES FORAM REALIZADOS COM SUCESSO!      \n");
    
    return 0;
}