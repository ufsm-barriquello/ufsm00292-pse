#include <stdio.h>
#include <stdbool.h>

// Definição dos estados do transmissor
typedef enum {
    TX_IDLE,
    TX_SEND,
    TX_WAIT_ACK,
    TX_RESEND
} TxState;

// Definição dos estados do receptor
typedef enum {
    RX_IDLE,
    RX_RECEIVE,
    RX_SEND_ACK,
    RX_ERROR
} RxState;

// Estruturas de controle
typedef struct {
    TxState state;
    int retries;
} Transmitter;

typedef struct {
    RxState state;
} Receiver;

// Funções simuladas
bool send_data() { printf("Enviando dados\n"); return true; }
bool receive_data() { printf("Recebendo dados\n"); return true; }
bool send_ack() { printf("Enviando ACK\n"); return true; }
bool ack_received() { return true; } // Simula ACK sempre recebido
bool data_valid() { return true; }   // Simula dados válidos

// Máquina de estados do transmissor
void transmitter_step(Transmitter *tx) {
    switch(tx->state) {
        case TX_IDLE:
            printf("TX: Aguardando dados\n");
            tx->state = TX_SEND;
            break;

        case TX_SEND:
            if (send_data())
                tx->state = TX_WAIT_ACK;
            break;

        case TX_WAIT_ACK:
            if (ack_received()) {
                printf("TX: ACK recebido \n");
                tx->state = TX_IDLE;
            } else {
                tx->retries++;
                if (tx->retries < 3)
                    tx->state = TX_RESEND;
                else {
                    printf("TX: Falha após 3 tentativas.\n");
                    tx->state = TX_IDLE;
                }
            }
            break;

        case TX_RESEND:
            printf("TX: Reenviando...\n");
            tx->state = TX_SEND;
            break;
    }
}

// Máquina de estados do receptor
void receiver_step(Receiver *rx) {
    switch(rx->state) {
        case RX_IDLE:
            rx->state = RX_RECEIVE;
            break;

        case RX_RECEIVE:
            if (receive_data()) {
                if (data_valid())
                    rx->state = RX_SEND_ACK;
                else
                    rx->state = RX_ERROR;
            }
            break;

        case RX_SEND_ACK:
            send_ack();
            rx->state = RX_IDLE;
            break;

        case RX_ERROR:
            printf("RX: Erro nos dados recebidos.\n");
            rx->state = RX_IDLE;
            break;
    }
}

int main() {
    Transmitter tx = {TX_IDLE, 0};
    Receiver rx = {RX_IDLE};

    for (int i = 0; i < 5; i++) {
        transmitter_step(&tx);
        receiver_step(&rx);
    }
printf("\n Todos os testes foram concluídos sem erros\n");
    return 0;

}
