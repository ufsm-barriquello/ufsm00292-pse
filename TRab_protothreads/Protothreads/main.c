#include "pt.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// =================== Definições ===================
#define STX  0x02
#define ETX  0x03
#define ACK  0x06
#define NACK 0x15
#define MAX_DATA 64

// Buffers
static uint8_t tx_buffer[MAX_DATA + 5];
static uint8_t rx_buffer[MAX_DATA + 5];

// Protothreads
static struct pt pt_tx, pt_rx;

// =================== Funções do protocolo ===================
uint8_t calc_checksum(uint8_t qtd, uint8_t *dados) {
    uint8_t chk = qtd;
    for(int i=0; i<qtd; i++) chk += dados[i];
    return chk;
}

void montar_pacote(uint8_t *buffer, uint8_t *dados, uint8_t qtd) {
    buffer[0] = STX;
    buffer[1] = qtd;
    memcpy(&buffer[2], dados, qtd);
    buffer[2+qtd] = calc_checksum(qtd, dados);
    buffer[3+qtd] = ETX;
}

int decodificar_pacote(uint8_t *buffer, uint8_t *dados_out) {
    if(buffer[0] != STX) return NACK;

    uint8_t qtd = buffer[1];
    if(buffer[qtd+3] != ETX) return NACK;

    memcpy(dados_out, &buffer[2], qtd);
    uint8_t chk = buffer[2+qtd];
    if(chk != calc_checksum(qtd, dados_out))
        return NACK;

    return ACK;
}

// =================== Protothread Transmissora ===================
static int protothread_tx(struct pt *pt) {
    static uint8_t dados[] = {0x10, 0x20, 0x30};
    static uint8_t qtd = 3;
    static int i;
    static int ack_received;

    PT_BEGIN(pt);

    while(1) {
        // Monta pacote
        montar_pacote(tx_buffer, dados, qtd);

        printf("[TX] Enviando pacote...\n");
        for(i=0; i<qtd+4; i++) {
            printf(" TX -> 0x%02X\n", tx_buffer[i]);
            PT_YIELD(pt); // simula tempo de envio
        }

        // Espera ACK
        ack_received = 0;
        for(i=0; i<10; i++) { // timeout
            PT_YIELD(pt);
            if(rx_buffer[0] == ACK) {
                ack_received = 1;
                printf("[TX] ACK recebido!\n");
                rx_buffer[0] = 0; // limpa
                break;
            }
        }

        if(!ack_received) {
            printf("[TX] Timeout! Reenviando...\n");
        }
    }

    PT_END(pt);
}

// =================== Protothread Receptora ===================
static int protothread_rx(struct pt *pt) {
    static int state = 0;
    static uint8_t qtd, dados[MAX_DATA], chk;
    static int idx;

    PT_BEGIN(pt);

    while(1) {
        PT_YIELD(pt);

        uint8_t byte = tx_buffer[0]; // simula recepção do primeiro byte
        memmove(tx_buffer, tx_buffer+1, sizeof(tx_buffer)-1); // shift

        switch(state) {
            case 0: // espera STX
                if(byte == STX) {
                    state = 1;
                    idx = 0;
                }
                break;

            case 1: // pega QTD
                qtd = byte;
                state = 2;
                break;

            case 2: // pega DADOS
                dados[idx++] = byte;
                if(idx >= qtd) state = 3;
                break;

            case 3: // pega CHK
                chk = byte;
                state = 4;
                break;

            case 4: // espera ETX
                if(byte == ETX) {
                    if(chk == calc_checksum(qtd, dados)) {
                        printf("[RX] Pacote OK! Enviando ACK\n");
                        rx_buffer[0] = ACK;
                    } else {
                        printf("[RX] Erro no checksum!\n");
                        rx_buffer[0] = NACK;
                    }
                }
                state = 0;
                break;
        }
    }

    PT_END(pt);
}

// =================== Testes TDD ===================
void test_checksum() {
    uint8_t dados[3] = {0x10, 0x20, 0x30};
    uint8_t chk = calc_checksum(3, dados);
    printf("Teste checksum: esperado=0x%02X obtido=0x%02X -> %s\n",
           (uint8_t)(3+0x10+0x20+0x30), chk,
           chk == (3+0x10+0x20+0x30) ? "OK" : "FALHA");
}

void test_montagem() {
    uint8_t dados[2] = {0xAA, 0xBB};
    uint8_t buffer[10];
    montar_pacote(buffer, dados, 2);

    printf("Teste montagem:\n");
    for(int i=0; i<6; i++) {
        printf("  buffer[%d] = 0x%02X\n", i, buffer[i]);
    }
    if(buffer[0]==STX && buffer[1]==2 && buffer[5]==ETX) {
        printf("  -> OK\n");
    } else {
        printf("  -> FALHA\n");
    }
}

void test_decodificacao_correta() {
    uint8_t dados[2] = {0x01, 0x02};
    uint8_t buffer[10];
    uint8_t saida[10];

    montar_pacote(buffer, dados, 2);
    int resp = decodificar_pacote(buffer, saida);

    printf("Teste decodificação correta: resposta=%s\n", resp==ACK ? "ACK" : "NACK");
}

void test_decodificacao_erro_checksum() {
    uint8_t dados[2] = {0x01, 0x02};
    uint8_t buffer[10];
    uint8_t saida[10];

    montar_pacote(buffer, dados, 2);
    buffer[3] ^= 0xFF; // corrompe dado

    int resp = decodificar_pacote(buffer, saida);
    printf("Teste decodificação com erro: resposta=%s\n", resp==NACK ? "NACK" : "ACK (FALHA)");
}

void test_timeout_reenvio() {
    int ack = 0;
    int tentativas = 0;

    while(!ack && tentativas < 3) {
        printf("Tentativa %d: enviando pacote...\n", tentativas+1);
        ack = 0; // simulação: nada chega
        tentativas++;
    }

    if(!ack) {
        printf("Timeout após %d tentativas -> REENVIO NECESSÁRIO\n", tentativas);
    }
}

// =================== MAIN ===================
int main(void) {

    printf("==== INICIANDO TESTES ====\n");
    test_checksum();
    test_montagem();
    test_decodificacao_correta();
    test_decodificacao_erro_checksum();
    test_timeout_reenvio();
    printf("==== FIM DOS TESTES ====\n");

    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);

    printf("==== SIMULAÇÃO PROTOTHREADS ====\n");
    while(1) {
        protothread_tx(&pt_tx);
        protothread_rx(&pt_rx);
    }
    return 0;
}
