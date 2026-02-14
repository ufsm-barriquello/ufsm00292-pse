#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "fsm_decoder.h"

// Variáveis globais para armazenar o resultado do pacote processado
static unsigned char processed_packet[MAX_BUFFER];
static int processed_len = 0;
static int packet_ok = 0;
static int packet_error = 0;

// Callback para quando um pacote é recebido com sucesso
void on_packet_received(unsigned char *data, int len) {
    memcpy(processed_packet, data, len);
    processed_len = len;
    packet_ok = 1;
}

// Callback para quando ocorre um erro de protocolo
void on_protocol_error() {
    packet_error = 1;
}

// Função para resetar o estado dos testes
void reset_test_state() {
    processed_len = 0;
    packet_ok = 0;
    packet_error = 0;
    memset(processed_packet, 0, MAX_BUFFER);
    fsm_init(on_packet_received, on_protocol_error);
}

// Teste 1: Pacote válido e completo
void test_valid_packet() {
    reset_test_state();
    printf("Executando teste: Pacote Válido\n");
    unsigned char data[] = {STX, 0x04, 0x0A, 0x0B, 0x0C, 0x0D, 0x3A, ETX}; // CHK = 0x0A ^ 0x0B ^ 0x0C ^ 0x0D = 0x3A
    fsm_process_data(data, sizeof(data));
    assert(packet_ok == 1);
    assert(packet_error == 0);
    assert(processed_len == 4);
    unsigned char expected_data[] = {0x0A, 0x0B, 0x0C, 0x0D};
    assert(memcmp(processed_packet, expected_data, 4) == 0);
    printf("Teste Pacote Válido: SUCESSO\n\n");
}

// Teste 2: Pacote com erro de checksum
void test_invalid_checksum() {
    reset_test_state();
    printf("Executando teste: Checksum Inválido\n");
    unsigned char data[] = {STX, 0x04, 0x0A, 0x0B, 0x0C, 0x0D, 0x3B, ETX}; // CHK correto é 0x3A
    fsm_process_data(data, sizeof(data));
    assert(packet_ok == 0);
    assert(packet_error == 1);
    printf("Teste Checksum Inválido: SUCESSO\n\n");
}

// Teste 3: Pacote com dados antes do STX (ruído)
void test_noise_before_stx() {
    reset_test_state();
    printf("Executando teste: Ruído antes do STX\n");
    unsigned char data[] = {0xFF, 0xFF, STX, 0x02, 0x01, 0x02, 0x03, ETX}; // CHK = 0x01 ^ 0x02 = 0x03
    fsm_process_data(data, sizeof(data));
    assert(packet_ok == 1);
    assert(packet_error == 0);
    assert(processed_len == 2);
    unsigned char expected_data[] = {0x01, 0x02};
    assert(memcmp(processed_packet, expected_data, 2) == 0);
    printf("Teste Ruído antes do STX: SUCESSO\n\n");
}

// Teste 4: Pacote incompleto (falta ETX)
void test_incomplete_packet() {
    reset_test_state();
    printf("Executando teste: Pacote Incompleto (sem ETX)\n");
    unsigned char data[] = {STX, 0x03, 0x10, 0x20, 0x30, 0x00}; // CHK = 0x10 ^ 0x20 ^ 0x30 = 0x00
    fsm_process_data(data, sizeof(data));
    assert(packet_ok == 0);
    assert(packet_error == 0); // Não é erro de protocolo, apenas o pacote não terminou
    printf("Teste Pacote Incompleto: SUCESSO\n\n");
}

// Teste 5: Dois pacotes válidos em sequência
void test_two_valid_packets() {
    reset_test_state();
    printf("Executando teste: Dois Pacotes Válidos\n");
    unsigned char data[] = {STX, 0x02, 0xAA, 0xBB, 0x11, ETX, STX, 0x01, 0xCC, 0xCC, ETX};
    fsm_process_data(data, sizeof(data));
    assert(packet_ok == 1);
    assert(packet_error == 0);
    assert(processed_len == 1);
    unsigned char expected_data[] = {0xCC};
    assert(memcmp(processed_packet, expected_data, 1) == 0);
    printf("Teste Dois Pacotes Válidos: SUCESSO\n\n");
}


int main() {
    test_valid_packet();
    test_invalid_checksum();
    test_noise_before_stx();
    test_incomplete_packet();
    test_two_valid_packets();
    return 0;
}
