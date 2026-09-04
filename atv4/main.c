/*
 * Exercicio com protothreads - UFSM00292 Projeto de Sistemas Embarcados
 *
 * Demonstracao das duas protothreads trocando mensagens no protocolo
 * | STX | QTD_DADOS | DADOS | CHK | ETX |.
 *
 * O primeiro envio sai com o checksum corrompido de proposito, para mostrar
 * o tempo maximo de espera e a retransmissao funcionando.
 */
#include <stdio.h>

#include "protocolo_pt.h"

int main(void)
{
    static const uint8_t mensagem[] = { 'O', 'l', 'a' };
    struct pt pt_tx, pt_rx;
    int estado_tx = PT_ESPERANDO;

    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);
    transmissora_prepara(mensagem, sizeof(mensagem), 1);

    printf("Duas protothreads trocando mensagens no protocolo"
           " STX|QTD|DADOS|CHK|ETX\n\n");

    /* cada volta do laco equivale a 1 ms */
    for (tempo_ms = 0; tempo_ms < 1000 && estado_tx != PT_ENCERRADA; tempo_ms++) {
        protothread_receptora(&pt_rx);
        estado_tx = protothread_transmissora(&pt_tx);
    }

    printf("\nFim da simulacao\n");
    return 0;
}
