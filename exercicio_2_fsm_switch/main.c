#include <stdio.h>
#include <assert.h>
#include "protocolo.h"

void run_tests() {
    printf("Iniciando bateria de testes TDD...\n");

    // Teste 1: Receptor recebe pacote válido
    ReceiverFSM_t rx;
    RX_Init(&rx);
    RX_ProcessByte(&rx, 0x02); // STX
    RX_ProcessByte(&rx, 0x02); // QTD
    RX_ProcessByte(&rx, 0x10); // DADO 1
    RX_ProcessByte(&rx, 0x20); // DADO 2
    RX_ProcessByte(&rx, 0x32); // CHK (2 ^ 0x10 ^ 0x20)
    RX_ProcessByte(&rx, 0x03); // ETX
    assert(rx.packet_ready == true);
    
    // Teste 2: Transmissor gera pacote válido
    TransmitterFSM_t tx;
    TX_Init(&tx);
    uint8_t payload[] = {0xAA, 0xBB};
    TX_StartPacket(&tx, payload, 2);
    
    uint8_t out;
    TX_ProcessByte(&tx, &out); assert(out == 0x02); // STX
    TX_ProcessByte(&tx, &out); assert(out == 0x02); // QTD
    TX_ProcessByte(&tx, &out); assert(out == 0xAA); // DADO 1
    TX_ProcessByte(&tx, &out); assert(out == 0xBB); // DADO 2
    TX_ProcessByte(&tx, &out); assert(out == 0x13); // CHK (2 ^ 0xAA ^ 0xBB)
    TX_ProcessByte(&tx, &out); assert(out == 0x03); // ETX
    
    printf("\n======================================================\n");
    printf("   TODOS OS TESTES FORAM REALIZADOS COM SUCESSO!\n");
    printf("======================================================\n");
}

int main() {
    run_tests();
    return 0;
}