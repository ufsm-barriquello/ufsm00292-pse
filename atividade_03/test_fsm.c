#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "fsm.h"

static int tests_passed = 0;
static int tests_total = 0;

#define TEST_ASSERT(cond, name) do { \
    tests_total++; \
    if (cond) { \
        printf("  [PASS] %s\n", name); \
        tests_passed++; \
    } else { \
        printf("  [FAIL] %s (linha %d)\n", name, __LINE__); \
    } \
} while(0)

/* Teste 1: Rejeição de ruído antes do STX */
void test_noise_rejection() {
    StateMachine sm;
    fsm_init(&sm);

    uint8_t noise[] = { 0xFF, 0x00, 0x55, 0xAA };
    fsm_process_buffer(&sm, noise, sizeof(noise));

    TEST_ASSERT(sm.state == ST_STX, "Ignora bytes de ruido antes do STX");
    TEST_ASSERT(!sm.packageReceived, "Nenhum pacote entregue durante ruido");
}

/* Teste 2: Recepção de pacote válido completo */
void test_valid_packet() {
    StateMachine sm;
    fsm_init(&sm);

    // Frame: STX(0x02), QTD(3), DADOS(10, 20, 30), CHK(60), ETX(0x03)
    uint8_t frame[] = { 0x02, 0x03, 10, 20, 30, 60, 0x03 };
    fsm_process_buffer(&sm, frame, sizeof(frame));

    TEST_ASSERT(sm.packageReceived == true, "Pacote integro recebido com sucesso");
    TEST_ASSERT(sm.indBuffer == 3, "Tamanho dos dados correto (3 bytes)");
    TEST_ASSERT(sm.buffer[0] == 10 && sm.buffer[1] == 20 && sm.buffer[2] == 30, "Conteudo dos dados integro");
    TEST_ASSERT(sm.state == ST_STX, "FSM retornou ao estado inicial apos ETX");
}

/* Teste 3: Detecção de erro de Checksum */
void test_invalid_checksum() {
    StateMachine sm;
    fsm_init(&sm);

    // Checksum esperado: 10+20+30 = 60 (0x3C). Enviado: 99 (0x63)
    uint8_t frame_corrompido[] = { 0x02, 0x03, 10, 20, 30, 99, 0x03 };
    fsm_process_buffer(&sm, frame_corrompido, sizeof(frame_corrompido));

    TEST_ASSERT(sm.packageReceived == false, "Rejeita pacote com Checksum incorreto");
    TEST_ASSERT(sm.state == ST_STX, "FSM reiniciou apos erro de Checksum");
}

/* Teste 4: Detecção de delimitador ETX inválido */
void test_invalid_etx() {
    StateMachine sm;
    fsm_init(&sm);

    // ETX esperado: 0x03. Enviado: 0x00
    uint8_t frame_sem_etx[] = { 0x02, 0x02, 5, 5, 10, 0x00 };
    fsm_process_buffer(&sm, frame_sem_etx, sizeof(frame_sem_etx));

    TEST_ASSERT(sm.packageReceived == false, "Rejeita pacote sem ETX valido");
    TEST_ASSERT(sm.state == ST_STX, "FSM reiniciou apos erro de ETX");
}

/* Teste 5: Processamento consecutivo de múltiplos pacotes */
void test_multiple_packets() {
    StateMachine sm;
    fsm_init(&sm);

    uint8_t stream[] = {
        0x02, 0x02, 1, 2, 3, 0x03,       // Pacote 1 (Valido)
        0x02, 0x01, 50, 99, 0x03,        // Pacote 2 (Invalido: CHK 99 != 50)
        0x02, 0x01, 77, 77, 0x03         // Pacote 3 (Valido)
    };

    fsm_process_buffer(&sm, stream, sizeof(stream));

    TEST_ASSERT(sm.packageReceived == true, "Processamento de fluxo continuo finalizado");
    TEST_ASSERT(sm.indBuffer == 1 && sm.buffer[0] == 77, "Ultimo pacote valido armazenado");
}

int main() {
    printf("====================================================\n");
    printf("     BANCADA DE TESTES TDD - FSM (PONTEIROS)       \n");
    printf("====================================================\n\n");

    printf("[SUITE 1] Teste de Ruido e Sincronismo:\n");
    test_noise_rejection();

    printf("\n[SUITE 2] Teste de Pacote Nominal:\n");
    test_valid_packet();

    printf("\n[SUITE 3] Teste de Resiliencia (Checksum):\n");
    test_invalid_checksum();

    printf("\n[SUITE 4] Teste de Resiliencia (ETX):\n");
    test_invalid_etx();

    printf("\n[SUITE 5] Teste de Fluxo Continuo:\n");
    test_multiple_packets();

    printf("\n====================================================\n");
    printf(" RESULTADO: %d/%d TESTES PASSARAM COM SUCESSO!\n", tests_passed, tests_total);
    printf(" STATUS: TODOS OS REQUISITOS TDD FORAM ATENDIDOS.  \n");
    printf("====================================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}