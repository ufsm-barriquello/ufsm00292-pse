#include <stdio.h>
#include <assert.h>
#include "fsm_decoder.h"

// Variável para verificar se o teste passou
int pacote_recebido_com_sucesso = 0; 

// Implementação obrigatória: O que fazer quando a FSM terminar de ler um pacote válido?
void handlePackage(unsigned char *buffer, int qtd) {
    pacote_recebido_com_sucesso = 1;
}

// Função de testes (TDD)
void rodar_testes() {
    printf("Iniciando bateria de testes...\n");

    // --- TESTE 1: Pacote válido ---
    initSM();
    pacote_recebido_com_sucesso = 0;
    // Formato esperado: STX | QTD(2 bytes) | DADOS(0x11, 0x22) | CHK(0x33) | ETX
    // 0x11 ^ 0x22 = 0x33 (Checksum válido)
    unsigned char pacote_valido[] = {STX, 2, 0x11, 0x22, 0x33, ETX};
    
    handleRx(pacote_valido, sizeof(pacote_valido));
    assert(pacote_recebido_com_sucesso == 1);
    printf("[OK] Teste 1: Pacote valido decodificado com sucesso.\n");

    // --- TESTE 2: Pacote com Checksum inválido ---
    initSM();
    pacote_recebido_com_sucesso = 0;
    // Enviando o CHK errado (0xFF em vez de 0x33)
    unsigned char pacote_invalido[] = {STX, 2, 0x11, 0x22, 0xFF, ETX}; 
    
    handleRx(pacote_invalido, sizeof(pacote_invalido));
    assert(pacote_recebido_com_sucesso == 0); // FSM deve ignorar o pacote
    printf("[OK] Teste 2: Pacote com checksum invalido rejeitado corretamente.\n");

    printf("\n=== TODOS OS TESTES PASSARAM ===\n");
}

int main() {
    rodar_testes();
    return 0;
}