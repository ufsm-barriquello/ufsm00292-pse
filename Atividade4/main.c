#include "protocol.h"

#include <stdio.h>

#define DEMO_MAX_STEPS 1000U

int main(void)
{
    static const uint8_t message[] = {0x11U, 0x22U, 0x33U, 0x44U};
    uint8_t received[PROTOCOL_MAX_PAYLOAD];
    protocol_system_t system;
    size_t received_length;
    size_t i;
    unsigned int step;

    protocol_system_init(&system, 5U);

    /* Simula a perda do primeiro ACK para demonstrar a retransmissao. */
    protocol_drop_next_ack(&system);

    if (!protocol_send(&system, message, sizeof(message))) {
        fprintf(stderr, "Nao foi possivel iniciar a transmissao.\n");
        return 1;
    }

    for (step = 0U; step < DEMO_MAX_STEPS && !system.transmitter.completed; ++step) {
        protocol_system_step(&system);
    }

    if (!system.transmitter.completed) {
        fprintf(stderr, "A transmissao nao terminou.\n");
        return 1;
    }

    if (!protocol_receive(&system, received, sizeof(received), &received_length)) {
        fprintf(stderr, "Nenhum quadro valido foi recebido.\n");
        return 1;
    }

    printf("ACK 0x%02X recebido pelo transmissor.\n", PROTOCOL_ACK);
    printf("Transmissao confirmada apos %u tentativa(s).\n",
           system.transmitter.attempts);
    printf("Retransmissoes: %u\n", system.transmitter.retransmissions);
    printf("Dados recebidos:");
    for (i = 0U; i < received_length; ++i) {
        printf(" %02X", received[i]);
    }
    putchar('\n');
    return 0;
}
