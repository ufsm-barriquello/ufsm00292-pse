/*
 * Demonstracao das FSMs do protocolo.
 *
 * O laco principal segue o modelo visto em aula (laco infinito com
 * switch): a cada volta o transmissor produz um byte e o receptor o
 * consome, como se os dois estivessem ligados por um canal serial.
 */

#include <stdio.h>
#include <string.h>
#include "protocolo.h"

static void mostra_resultado(rx_result_t r, const rx_t *rx)
{
    uint8_t i;

    switch (r) {
    case RX_QUADRO_OK:
        printf("\n  -> quadro recebido (%u bytes): \"", (unsigned)rx->qtd);
        for (i = 0; i < rx->qtd; i++) {
            printf("%c", rx->dados[i]);
        }
        printf("\"\n");
        break;
    case RX_ERRO_QTD:
        printf("\n  -> erro: QTD_DADOS acima do limite\n");
        break;
    case RX_ERRO_CHK:
        printf("\n  -> erro: checksum invalido\n");
        break;
    case RX_ERRO_ETX:
        printf("\n  -> erro: ETX ausente\n");
        break;
    case RX_NADA:
    default:
        break;
    }
}

/* envia uma mensagem pelo transmissor e entrega os bytes ao receptor */
static void transmite(tx_t *tx, rx_t *rx, const char *msg)
{
    uint8_t byte;

    printf("\nTransmitindo \"%s\"\n", msg);

    if (!tx_carrega(tx, (const uint8_t *)msg, (uint8_t)strlen(msg))) {
        printf("  -> mensagem grande demais para o protocolo\n");
        return;
    }

    printf("  bytes na linha:");
    for (;;) {                                  /* laco da FSM do TX    */
        if (!tx_processa(tx, &byte)) {
            break;                              /* transmissor ocioso   */
        }
        printf(" %02X", byte);
        mostra_resultado(rx_processa(rx, byte), rx);   /* FSM do RX     */
    }
    printf("\n");
}

int main(void)
{
    tx_t tx;
    rx_t rx;
    uint8_t quadro_ruim[] = { STX, 2, 'O', 'K', 0x00, ETX };  /* CHK errado */
    unsigned i;

    tx_init(&tx);
    rx_init(&rx);

    transmite(&tx, &rx, "Sistemas Embarcados");
    transmite(&tx, &rx, "");                    /* quadro sem dados     */

    printf("\nRecebendo um quadro com checksum corrompido\n  bytes na linha:");
    for (i = 0; i < sizeof(quadro_ruim); i++) {
        printf(" %02X", quadro_ruim[i]);
        mostra_resultado(rx_processa(&rx, quadro_ruim[i]), &rx);
    }
    printf("\n");

    transmite(&tx, &rx, "ressincronizado");     /* FSM se recupera      */

    return 0;
}
