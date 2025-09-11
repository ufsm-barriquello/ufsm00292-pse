#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../tester_framework/tester_framework.h"
#include "../receiver.h"
#include "../transmitter.h"
#include "../protocol.h"

char* testReceiverSimpleMessage() {
    printf("Testando recepção de mensagem simples.\n");
    
    transmitterInit();
    receiverReset();
    
    unsigned char msg[] = "HELLO";
    size_t msg_len = strlen((char*)msg);
    bool txResult = transmitterSetMessage(msg, (int)msg_len);
    verifica("transmitterSetMessage deveria ter sucesso", txResult == true);
    
    bool tx_continua = true;
    bool mensagem_completa = false;
    int passos = 0;
    
    while(tx_continua && !mensagem_completa && passos < 15) {
        passos++;
        
        tx_continua = transmitterFiniteStateMachine();
        
        if(receiveByte()) {
            if(receiverHasCompleteMessage()) {
                mensagem_completa = true;
                break;
            }
            if(receiverHasError()) {
                break;
            }
        }
    }
    
    verifica("Deveria ter recebido mensagem completa", mensagem_completa == true);
    
    if(mensagem_completa) {
        int receivedSize = receiverGetDataSize();
        unsigned char* receivedData = receiverGetData();
        
        verifica("Tamanho recebido deveria ser 5", receivedSize == 5);
        verifica("Dados recebidos não deveriam ser NULL", receivedData != NULL);
        
        if (receivedData) {
            bool dados_iguais = memcmp(receivedData, msg, (size_t)receivedSize) == 0;
            verifica("Dados recebidos deveriam ser iguais aos enviados (\"HELLO\")", dados_iguais == true);
        }
    }
    
    printf("Recepção simples passou\n");
    return 0;
}

char* testReceiverBinaryData() {
    printf("Testando recepção de dados binários.\n");
    
    transmitterInit();
    receiverReset();
    
    unsigned char binaryData[] = {0x41, 0x42, 0x00, 0xFF, 0x7E};
    bool txResult = transmitterSetMessage(binaryData, sizeof(binaryData));
    verifica("transmitterSetMessage de dados binários deveria ter sucesso", txResult == true);
    
    bool tx_continua = true;
    bool mensagem_completa = false;
    int passos = 0;
    
    while(tx_continua && !mensagem_completa && passos < 15) {
        passos++;
        
        tx_continua = transmitterFiniteStateMachine();
        
        if(receiveByte()) {
            if(receiverHasCompleteMessage()) {
                mensagem_completa = true;
                break;
            }
            if(receiverHasError()) {
                break;
            }
        }
    }
    
    verifica("Recepção de dados binários deveria ter sucesso", mensagem_completa == true);
    
    if(mensagem_completa) {
        int receivedSize = receiverGetDataSize();
        unsigned char* receivedData = receiverGetData();
        
        verifica("Tamanho recebido deveria ser 5", receivedSize == 5);
        
        if (receivedData) {
            bool dados_iguais = memcmp(receivedData, binaryData, (size_t)receivedSize) == 0;
            verifica("Dados binários recebidos deveriam ser iguais aos enviados ([0x41,0x42,0x00,0xFF,0x7E])", dados_iguais == true);
        }
    }
    
    printf("Recepção de dados binários passou\n");
    return 0;
}

char* testReceiverProtocolValidation() {
    printf("Testando validação de protocolo.\n");
    
    FILE* file = fopen("/tmp/uart_comm.dat", "wb");
    verifica("Deveria conseguir criar arquivo de teste", file != NULL);
    
    if (file) {
        unsigned char invalidData[] = {0xFF};
        fwrite(invalidData, 1, sizeof(invalidData), file);
        fclose(file);
    }
    
    receiverReset();
    
    bool rxResult = receiveByte();
    verifica("Recepção de protocolo inválido deveria processar", rxResult == false);
    verifica("Deveria haver erro", receiverHasError() == true);
    
    printf("Validação de protocolo passou\n");
    return 0;
}

char* testReceiverChecksumValidation() {
    printf("Testando validação de checksum\n");
    
    FILE* file = fopen("/tmp/uart_comm.dat", "wb");
    verifica("Deveria conseguir criar arquivo de teste", file != NULL);
    
    if (file) {
        unsigned char invalidChecksum[] = {STX, 0x04, 'T', 'E', 'S', 'T', 0xFF, ETX};
        fwrite(invalidChecksum, 1, sizeof(invalidChecksum), file);
        fclose(file);
    }
    
    unsigned char* receivedData = NULL;
    int receivedSize = 0;
    
    bool rxResult = receiveMessage(&receivedData, &receivedSize);
    verifica("Recepção com checksum inválido deveria falhar", rxResult == false);
    
    if (receivedData != NULL) {
        free(receivedData);
    }
    
    printf("Validação de checksum passou\n");
    return 0;
}

char* testReceiverEmptyMessage() {
    printf("Testando recepção de mensagem vazia válida.\n");
    
    transmitterInit();
    bool txResult = transmitterSetMessage(NULL, 0);
    verifica("transmitterSetMessage com size=0 deveria ter sucesso", txResult == true);
    
    txResult = transmitterSetMessage(NULL, 5);
    verifica("transmitterSetMessage com dados NULL e size > 0 deveria falhar", txResult == false);
    
    receiverReset();
    
    FILE* file = fopen("/tmp/uart_comm.dat", "wb");
    if (file) {
        unsigned char stx = STX;
        fwrite(&stx, 1, 1, file);
        fclose(file);
    }
    bool rxResult = receiveByte();
    verifica("Deveria processar STX corretamente", rxResult == true);
    
    file = fopen("/tmp/uart_comm.dat", "wb");
    if (file) {
        unsigned char quantity = 0x00;
        fwrite(&quantity, 1, 1, file);
        fclose(file);
    }
    rxResult = receiveByte();
    verifica("Deveria processar QUANTITY=0 corretamente", rxResult == true);
    
    file = fopen("/tmp/uart_comm.dat", "wb");
    if (file) {
        unsigned char checksum = 0x00;
        fwrite(&checksum, 1, 1, file);
        fclose(file);
    }
    rxResult = receiveByte();
    verifica("Deveria processar CHECKSUM=0 corretamente", rxResult == true);
    
    file = fopen("/tmp/uart_comm.dat", "wb");
    if (file) {
        unsigned char etx = ETX;
        fwrite(&etx, 1, 1, file);
        fclose(file);
    }
    rxResult = receiveByte();
    verifica("Deveria processar ETX corretamente", rxResult == true);
    
    verifica("Deveria ter mensagem completa", receiverHasCompleteMessage() == true);
    verifica("Tamanho deveria ser 0", receiverGetDataSize() == 0);
    verifica("Dados deveriam ser NULL para mensagem vazia", receiverGetData() == NULL);
    
    printf("Mensagem vazia passou\n");
    return 0;
}

char* testReceiverNullMessage() {
    printf("Testando comportamento do receiver com dados corrompidos.\n");
    
    receiverReset();
    
    FILE* file = fopen("/tmp/uart_comm.dat", "wb");
    if (file) {
        unsigned char invalidByte = 0x00;
        fwrite(&invalidByte, 1, 1, file);
        fclose(file);
    }
    
    bool rxResult = receiveByte();
    verifica("Recepção de byte inválido deveria falhar", rxResult == false);
    verifica("Deveria indicar erro", receiverHasError() == true);
    verifica("Não deveria ter mensagem completa", receiverHasCompleteMessage() == false);
    
    transmitterInit();
    bool txResult = transmitterSetMessage(NULL, 5);
    verifica("transmitterSetMessage com dados NULL deveria falhar", txResult == false);
    
    printf("Dados NULL/corrompidos passou\n");
    return 0;
}

char* testReceiverIntegrationWithTransmitter() {
    printf("Testando integração com várias transmissões\n");
    
    unsigned char* testMessages[] = {
        (unsigned char*)"A",
        (unsigned char*)"HELLO",
        (unsigned char*)"123456789"
    };
    int testSizes[] = {1, 5, 9};
    
    for (int i = 0; i < 3; i++) {
        printf("  Teste %d: '%s' (%d bytes)\n", i+1, testMessages[i], testSizes[i]);
        
        transmitterInit(); 
        receiverReset();
        
        bool txResult = transmitterSetMessage(testMessages[i], testSizes[i]);
        verifica("transmitterSetMessage deveria ter sucesso", txResult == true);
        
        bool tx_continua = true;
        bool mensagem_completa = false;
        int passos = 0;
        
        while(tx_continua && !mensagem_completa && passos < 20) {
            passos++;
            
            tx_continua = transmitterFiniteStateMachine();
            
            if(receiveByte()) {
                if(receiverHasCompleteMessage()) {
                    mensagem_completa = true;
                    break;
                }
                if(receiverHasError()) {
                    break;
                }
            }
        }
        
        verifica("Receiver deveria ter sucesso", mensagem_completa == true);
        
        if(mensagem_completa) {
            int receivedSize = receiverGetDataSize();
            unsigned char* receivedData = receiverGetData();
            
            verifica("Tamanho deveria corresponder", receivedSize == testSizes[i]);
            
            if (receivedData) {
                bool dados_iguais = memcmp(receivedData, testMessages[i], (size_t)receivedSize) == 0;
                if (i == 0) {
                    verifica("Dados deveriam ser iguais para mensagem \"A\"", dados_iguais == true);
                } else if (i == 1) {
                    verifica("Dados deveriam ser iguais para mensagem \"HELLO\"", dados_iguais == true);
                } else if (i == 2) {
                    verifica("Dados deveriam ser iguais para mensagem \"123456789\"", dados_iguais == true);
                }
            }
        }
    }
    
    printf("Integração completa passou\n");
    return 0;
}

char* testReceiverEmptyMessageIntegration() {
    printf("Testando integração TX->RX para mensagem vazia.\n");
    
    transmitterInit();
    receiverReset();
    
    unsigned char* emptyData = NULL;
    bool txResult = transmitterSetMessage(emptyData, 0);
    verifica("transmitterSetMessage com size=0 deveria ter sucesso", txResult == true);
    
    bool tx_continua = true;
    bool mensagem_completa = false;
    int passos = 0;
    
    while(tx_continua && !mensagem_completa && passos < 10) {
        passos++;
        
        tx_continua = transmitterFiniteStateMachine();
        
        if(receiveByte()) {
            if(receiverHasCompleteMessage()) {
                mensagem_completa = true;
                break;
            }
            if(receiverHasError()) {
                break;
            }
        }
    }
    
    verifica("Deveria ter recebido mensagem vazia completa", mensagem_completa == true);
    
    if(mensagem_completa) {
        int receivedSize = receiverGetDataSize();
        unsigned char* receivedData = receiverGetData();
        
        verifica("Tamanho recebido deveria ser 0", receivedSize == 0);
        verifica("Dados deveriam ser NULL para mensagem vazia", receivedData == NULL);
    }
    
    printf("Integração mensagem vazia passou\n");
    return 0;
}

char* runAllReceiverTests() {
    printf("\nEXECUTANDO TESTES DO RECEIVER\n");
    
    executa_teste(testReceiverSimpleMessage);
    executa_teste(testReceiverBinaryData);
    executa_teste(testReceiverEmptyMessage);
    executa_teste(testReceiverEmptyMessageIntegration);
    executa_teste(testReceiverNullMessage);
    executa_teste(testReceiverProtocolValidation);
    executa_teste(testReceiverIntegrationWithTransmitter);
    
    printf("TODOS OS TESTES DO RECEIVER PASSARAM\n\n");
    return 0;
}
