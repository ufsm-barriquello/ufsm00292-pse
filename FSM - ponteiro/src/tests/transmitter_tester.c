#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../tester_framework/tester_framework.h"
#include "../transmitter.h"
#include "../protocol.h"

char* testTransmitterInit() {
    printf("Testando transmitterInit()\n");
    
    transmitterInit();
    
    FILE* file = fopen("/tmp/uart_comm.dat", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        verifica("Arquivo deveria estar vazio após init", size == 0);
    }
    
    printf("transmitterInit passou\n");
    return 0;
}

char* testTransmitterSimpleMessage() {
    printf("Testando transmissão de mensagem simples\n");
    
    transmitterInit();
    
    unsigned char msg[] = "TEST";
    size_t msg_len = strlen((char*)msg);
    bool result = transmitterSetMessage(msg, (int)msg_len);
    verifica("transmitterSetMessage deveria retornar true", result == true);
    
    int bytes_enviados = 0;
    bool chegou_success = false;
    
    while(transmitterFiniteStateMachine() && bytes_enviados < 10) {
        bytes_enviados++;
        int novo_estado = transmitterGetState();
        if(novo_estado == 6) {
            chegou_success = true;
        }
    }
    
    verifica("Deveria ter enviado pelo menos 5 bytes", bytes_enviados >= 5);
    verifica("Deveria ter passado pelo estado SUCCESS", chegou_success == true);
    
    printf("Transmissão simples passou\n");
    return 0;
}

char* testTransmitterBinaryData() {
    printf("Testando transmissão de dados binários\n");
    
    transmitterInit();
    
    unsigned char binaryData[] = {0x41, 0x42, 0x00, 0xFF};
    bool result = transmitterSetMessage(binaryData, sizeof(binaryData));
    verifica("transmitterSetMessage com dados binários deveria retornar true", result == true);
    
    bool chegou_success = false;
    while(transmitterFiniteStateMachine()) {
        if(transmitterGetState() == 6) {
            chegou_success = true;
        }
    }
    
    verifica("Deveria ter passado pelo estado SUCCESS", chegou_success == true);
    
    printf("Dados binários passou\n");
    return 0;
}

char* testTransmitterEmptyMessage() {
    printf("Testando transmissão de mensagem vazia\n");
    
    transmitterInit();
    
    bool result = transmitterSetMessage(NULL, 0);
    verifica("transmitterSetMessage com size=0 deveria retornar true", result == true);
    
    bool chegou_success = false;
    while(transmitterFiniteStateMachine()) {
        if(transmitterGetState() == 6) {
            chegou_success = true;
        }
    }
    
    verifica("Deveria transmitir mensagem vazia com sucesso", chegou_success == true);
    
    printf("Mensagem vazia passou\n");
    return 0;
}

char* testTransmitterInvalidData() {
    printf("Testando transmissão com dados inválidos.\n");
    
    transmitterInit();
    
    unsigned char msg[] = "TEST";
    
    bool result = transmitterSetMessage(msg, -1);
    verifica("transmitterSetMessage com tamanho negativo deveria retornar false", result == false);
    
    result = transmitterSetMessage(msg, 300);
    verifica("transmitterSetMessage com tamanho > MAX_DATA_SIZE deveria retornar false", result == false);
    
    result = transmitterSetMessage(NULL, 5);
    verifica("transmitterSetMessage com dados NULL e size > 0 deveria retornar false", result == false);
    
    printf("Dados inválidos passou\n");
    return 0;
}

char* runAllTransmitterTests() {
    printf("\nEXECUTANDO TESTES DO TRANSMITTER\n");
    
    executa_teste(testTransmitterInit);
    executa_teste(testTransmitterSimpleMessage);
    executa_teste(testTransmitterBinaryData);
    executa_teste(testTransmitterEmptyMessage);
    executa_teste(testTransmitterInvalidData);
    
    printf("TODOS OS TESTES DO TRANSMITTER PASSARAM\n\n");
    return 0;
}
