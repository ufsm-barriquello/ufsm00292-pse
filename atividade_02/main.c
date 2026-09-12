#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STX_BYTE 0x02
#define ETX_BYTE 0x03
#define MAX_PAYLOAD 256

typedef enum { RX_STX = 0, RX_QTD, RX_DATA, RX_CHK, RX_ETX } RxState;

typedef struct {
    RxState state;
    uint8_t buffer[MAX_PAYLOAD];
    uint8_t chkBuffer;
    int indBuffer;
    int qtdBuffer;
} RxContext;

void initRx(RxContext *rx) {
    rx->state = RX_STX;
    rx->indBuffer = 0;
    rx->qtdBuffer = 0;
    rx->chkBuffer = 0;
}

bool processRxByte(RxContext *rx, uint8_t byte) {
    switch (rx->state) {
        case RX_STX:
            if (byte == STX_BYTE) {
                rx->indBuffer = rx->qtdBuffer = rx->chkBuffer = 0;
                rx->state = RX_QTD;
            }
            break;
        case RX_QTD:
            rx->qtdBuffer = byte;
            rx->state = (byte > 0 && byte <= MAX_PAYLOAD) ? RX_DATA : RX_STX;
            break;
        case RX_DATA:
            rx->buffer[rx->indBuffer++] = byte;
            rx->chkBuffer += byte;
            if (rx->indBuffer >= rx->qtdBuffer) rx->state = RX_CHK;
            break;
        case RX_CHK:
            rx->state = (byte == rx->chkBuffer) ? RX_ETX : RX_STX;
            break;
        case RX_ETX:
            rx->state = RX_STX;
            return (byte == ETX_BYTE);
    }
    return false;
}

int main() {
    FILE *log = fopen("log_fsm.csv", "w");
    if (!log) return 1;
    fprintf(log, "PacoteID,Tamanho,Status,ChecksumCalculado\n");

    // Vetores de teste simulando canal
    uint8_t pacotes[][10] = {
        {0x02, 3, 10, 20, 30, 60, 0x03},          // Válido
        {0x02, 4, 1, 2, 3, 4, 10, 0x03},           // Válido
        {0x02, 3, 10, 20, 30, 99, 0x03},          // Erro de Checksum
        {0x02, 2, 5, 5, 10, 0x00},                // Erro de ETX
        {0x02, 5, 10, 10, 10, 10, 10, 50, 0x03}   // Válido
    };
    int tamanhos[] = {7, 8, 7, 6, 9};

    for (int p = 0; p < 5; p++) {
        RxContext rx;
        initRx(&rx);
        bool sucesso = false;

        for (int i = 0; i < tamanhos[p]; i++) {
            if (processRxByte(&rx, pacotes[p][i])) sucesso = true;
        }

        fprintf(log, "%d,%d,%s,%d\n", p + 1, rx.indBuffer, sucesso ? "SUCESSO" : "FALHA", rx.chkBuffer);
    }

    fclose(log);
    printf("Simulacao concluida. Log 'log_fsm.csv' gerado.\n");
    return 0;
}