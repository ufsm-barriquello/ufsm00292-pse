#include "protocol.h"

#include <stdio.h>

int main(void)
{
    static const uint8_t message[] = {0x11U, 0x22U, 0x33U, 0x44U};
    protocol_tx_t tx;
    protocol_rx_t rx;
    protocol_rx_event_t event = PROTOCOL_RX_NONE;
    uint8_t byte;
    size_t i;

    protocol_tx_init(&tx);
    protocol_rx_init(&rx);

    if (!protocol_tx_start(&tx, message, sizeof(message))) {
        fprintf(stderr, "Nao foi possivel iniciar a transmissao.\n");
        return 1;
    }

    printf("Quadro transmitido:");
    while (protocol_tx_next(&tx, &byte)) {
        printf(" %02X", byte);
        event = protocol_rx_process(&rx, byte);
    }
    putchar('\n');

    if (event != PROTOCOL_RX_FRAME_READY) {
        fprintf(stderr, "O receptor rejeitou o quadro.\n");
        return 1;
    }

    printf("Dados recebidos:");
    for (i = 0U; i < protocol_rx_length(&rx); ++i) {
        printf(" %02X", protocol_rx_payload(&rx)[i]);
    }
    putchar('\n');
    return 0;
}
