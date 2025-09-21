#include <stdio.h>
#include <stdlib.h>
#include "../tester_framework/tester_framework.h"
#include "transmitter_tester.h"
#include "receiver_tester.h"

static char* runAllTests() {
    executa_teste(runAllTransmitterTests);
    executa_teste(runAllReceiverTests);
    return 0;
}

int main() {
    printf("SISTEMA DE TESTES\n");
    printf("Protocolo: STX + QUANTITY + DATA + CHECKSUM + ETX\n");
    printf("Arquivo: /tmp/uart_comm.dat\n\n");
    
    framework_inicia_contagem();
    
    char* resultado = runAllTests();
    
    if (resultado != 0) {
        printf("TESTE FALHOU: %s\n", resultado);
        return 1;
    }
    
    printf("TODOS OS %d TESTES PASSARAM.\n", framework_get_contagem());
    
    return 0;
}
