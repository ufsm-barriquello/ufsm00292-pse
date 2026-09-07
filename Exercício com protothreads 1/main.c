#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// --- MINI BIBLIOTECA PROTOTHREADS ---
struct pt { unsigned short lc; };

#define PT_WAITING 0
#define PT_YIELDED 1
#define PT_EXITED  2
#define PT_ENDED   3

#define PT_INIT(pt)   (pt)->lc = 0
#define PT_BEGIN(pt)  switch((pt)->lc) { case 0:

#define PT_WAIT_UNTIL(pt, condition)              \
  do {                                            \
    (pt)->lc = __LINE__;                          \
    return PT_WAITING;                            \
    case __LINE__:                                \
    if (!(condition)) return PT_WAITING;          \
  } while (0)

#define PT_YIELD(pt)                              \
  do {                                            \
    (pt)->lc = __LINE__;                          \
    return PT_YIELDED;                            \
    case __LINE__: ;                              \
  } while (0)

#define PT_END(pt)    } (pt)->lc = 0; return PT_ENDED
#define PT_SCHEDULE(f) ((f) < PT_EXITED)
// -------------------------------------

// Definições do Protocolo
#define STX 0x02
#define ETX 0x03
#define MAX_DADOS 255
#define TIMEOUT_MAX 5

// Barramento simulado
uint8_t buffer_comunicacao[300];
int tamanho_buffer = 0;
int ack_recebido = 0;
int timer_simulado = 0;

// Injeção de falha simulada (descarta apenas a primeira tentativa)
int simular_perda_pacote = 1;

// --- FUNÇÕES DO PROTOCOLO ---

uint8_t calcular_checksum(uint8_t qtd, uint8_t *dados) {
    uint8_t chk = qtd;
    for (int i = 0; i < qtd; i++) {
        chk ^= dados[i];
    }
    return chk;
}

int codificar_pacote(uint8_t *dados, uint8_t qtd, uint8_t *buffer_saida) {
    buffer_saida[0] = STX;
    buffer_saida[1] = qtd;
    memcpy(&buffer_saida[2], dados, qtd);
    buffer_saida[2 + qtd] = calcular_checksum(qtd, dados);
    buffer_saida[3 + qtd] = ETX;
    return qtd + 4;
}

int decodificar_pacote(uint8_t *buffer, int tamanho, uint8_t *dados_saida) {
    if (tamanho < 4 || buffer[0] != STX) {
        return 0;
    }

    uint8_t qtd = buffer[1];

    if (tamanho != qtd + 4 || buffer[tamanho - 1] != ETX) {
        return 0;
    }

    uint8_t chk_recebido = buffer[tamanho - 2];
    if (calcular_checksum(qtd, &buffer[2]) != chk_recebido) {
        return 0;
    }

    memcpy(dados_saida, &buffer[2], qtd);
    return 1;
}

// --- FASE DE TESTES (TDD) ---

void rodar_testes_tdd() {
    printf("Iniciando testes unitarios (TDD)...\n");

    uint8_t payload[] = {0x10, 0x20, 0x30};
    uint8_t pacote[16];
    uint8_t saida[16];

    int tam = codificar_pacote(payload, 3, pacote);
    assert(tam == 7);
    assert(pacote[0] == STX);
    assert(pacote[1] == 3);
    assert(pacote[tam - 1] == ETX);

    assert(decodificar_pacote(pacote, tam, saida) == 1);
    assert(saida[0] == 0x10 && saida[1] == 0x20 && saida[2] == 0x30);

    // Teste de checksum inválido
    pacote[tam - 2] ^= 0xFF;
    assert(decodificar_pacote(pacote, tam, saida) == 0);
    pacote[tam - 2] ^= 0xFF;

    // Teste de tamanho divergente
    assert(decodificar_pacote(pacote, tam + 1, saida) == 0);

    // Teste de delimitador inválido
    pacote[0] = 0x00;
    assert(decodificar_pacote(pacote, tam, saida) == 0);

    printf("Todos os testes unitarios passaram com sucesso!\n\n");
}

// --- PROTOTHREADS ---

static struct pt pt_tx, pt_rx;

static int thread_transmissora(struct pt *pt) {
    PT_BEGIN(pt);

    static uint8_t msg[] = "DadosEmbarcados";
    static int tentativas;
    tentativas = 0;

    while (tentativas < 3) {
        printf("[TX] Enviando tentativa %d...\n", tentativas + 1);
        tamanho_buffer = codificar_pacote(msg, strlen((char *)msg), buffer_comunicacao);
        ack_recebido = 0;
        timer_simulado = 0;

        // Aguarda ACK ou Timeout
        PT_WAIT_UNTIL(pt, ack_recebido == 1 || timer_simulado >= TIMEOUT_MAX);

        if (ack_recebido) {
            printf("[TX] Sucesso: ACK recebido apos a tentativa %d.\n", tentativas + 1);
            break;
        } else {
            printf("[TX] Timeout atingido (%d ticks)! Preparando retransmissao...\n\n", TIMEOUT_MAX);
            tentativas++;
        }
    }

    PT_END(pt);
}

static int thread_receptora(struct pt *pt) {
    PT_BEGIN(pt);
    static uint8_t dados_extraidos[MAX_DADOS];

    while (1) {
        PT_WAIT_UNTIL(pt, tamanho_buffer > 0);

        if (simular_perda_pacote) {
            printf("[RX] [SIMULACAO] Ruido na linha: Pacote descartado propositalmente.\n");
            simular_perda_pacote = 0;
            tamanho_buffer = 0;
        } else {
            printf("[RX] Processando pacote recebido...\n");
            if (decodificar_pacote(buffer_comunicacao, tamanho_buffer, dados_extraidos)) {
                printf("[RX] Pacote valido e integro. Enviando ACK.\n");
                ack_recebido = 1;
            } else {
                printf("[RX] Pacote corrompido. ACK descartado.\n");
            }
            tamanho_buffer = 0;
        }
    }

    PT_END(pt);
}

int main() {
    rodar_testes_tdd();

    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);

    printf("=== Executando Maquina de Estados Protothreads ===\n");
    
    // O loop roda enquanto a transmissora estiver ativa
    while (PT_SCHEDULE(thread_transmissora(&pt_tx))) {
        thread_receptora(&pt_rx);
        timer_simulado++;
    }

    printf("\nProcesso finalizado com sucesso.\n");
    return 0;
}