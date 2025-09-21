#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../tester_framework/tester_framework.h"
#include "../receiver.h"
#include "../transmitter.h"
#include "../protocol.h"

static struct receiver rx;
static struct transmitter tx;

// Scheduler cooperativo
static void cooperative_scheduler(unsigned char* msg, int msg_len, const char* test_name) {
    bool tx_active = true;
    bool rx_active = true;
    int cycle = 0;
    
    printf("  [%s] Scheduler cooperativo iniciado\n", test_name);
    
    while((tx_active || rx_active) && cycle < 500) {
        cycle++;
        
        // Executa TX protothread primeiro
        if(tx_active) {
            int tx_result = transmitterThread(&tx, msg, msg_len);
            
            if(tx_result == PT_YIELDED) {
                printf("  [%s:%d] TX cedeu controle (YIELDED) - outras tarefas podem executar\n", test_name, cycle);
            } else if(tx_result == PT_WAITING) {
                printf("  [%s:%d] TX aguardando condição (WAITING) - processando...\n", test_name, cycle);
            } else if(tx_result == PT_ENDED) {
                printf("  [%s:%d] TX finalizou transmissão (ENDED)\n", test_name, cycle);
                tx_active = false;
            } else if(tx_result == PT_EXITED) {
                printf("  [%s:%d] TX saiu com erro (EXITED)\n", test_name, cycle);
                tx_active = false;
            }
        }
        
        // Executa RX protothread
        if(rx_active) {
            int rx_result = receiverThread(&rx);
            
            if(rx_result == PT_YIELDED) {
                printf("  [%s:%d] RX cedeu controle (YIELDED) - outros processos executam\n", test_name, cycle);
            } else if(rx_result == PT_WAITING) {
                printf("  [%s:%d] RX aguardando dados (WAITING) - verificando buffer...\n", test_name, cycle);
            } else if(rx_result == PT_ENDED) {
                printf("  [%s:%d] RX finalizou recepção (ENDED)\n", test_name, cycle);
                rx_active = false;
            } else if(rx_result == PT_EXITED) {
                printf("  [%s:%d] RX saiu com erro (EXITED)\n", test_name, cycle);
                rx_active = false;
            }
        }
        
    }
    
    printf("  [%s] Scheduler finalizado em %d ciclos\n", test_name, cycle);
}

char* testReceiverSimpleMessage() {
    printf("Testando recepção de mensagem simples.\n");

    transmitterInit(&tx);
    receiverReset(&rx);

    unsigned char msg[] = "HELLO";
    size_t msg_len = strlen((char*)msg);

    cooperative_scheduler(msg, (int)msg_len, "SimpleMsg");

    verifica("Deveria ter recebido mensagem completa", rx.complete == true);

    if(rx.complete) {
        verifica("Tamanho recebido deveria ser 5", rx.expected_size == 5);
        verifica("Dados recebidos não deveriam ser NULL", rx.data != NULL);
        if(rx.data) {
            bool iguais = memcmp(rx.data, msg, 5) == 0;
            verifica("Dados recebidos deveriam ser iguais aos enviados (\"HELLO\")", iguais == true);
        }
    }

    printf("Recepção simples passou\n");
    return 0;
}

char* testReceiverBinaryData() {
    printf("Testando recepção de dados binários.\n");

    transmitterInit(&tx);
    receiverReset(&rx);

    unsigned char binaryData[] = {0x41, 0x42, 0x00, 0xFF, 0x7E};

    cooperative_scheduler(binaryData, sizeof(binaryData), "BinaryData");

    verifica("Recepção de dados binários deveria ter sucesso", rx.complete == true);

    if(rx.complete) {
        verifica("Tamanho recebido deveria ser 5", rx.expected_size == 5);
        if(rx.data) {
            bool iguais = memcmp(rx.data, binaryData, 5) == 0;
            verifica("Dados binários recebidos deveriam ser iguais aos enviados ([0x41,0x42,0x00,0xFF,0x7E])", iguais == true);
        }
    }

    printf("Recepção de dados binários passou\n");
    return 0;
}

char* testReceiverEmptyMessage() {
    printf("Testando integração TX->RX para mensagem vazia.\n");

    transmitterInit(&tx);
    receiverReset(&rx);

    cooperative_scheduler(NULL, 0, "EmptyMsg");

    verifica("Deveria ter recebido mensagem vazia completa", rx.complete == true);
    verifica("Tamanho recebido deveria ser 0", rx.expected_size == 0);
    verifica("Dados deveriam ser NULL para mensagem vazia", rx.data == NULL);

    printf("Integração mensagem vazia passou\n");
    return 0;
}

char* testReceiverChecksumError() {
    printf("Testando recepção com checksum inválido.\n");

    unsigned char msg_invalid[] = {STX, 3, 'A','B','C', 0xFF, ETX};
    FILE* file = fopen("/tmp/uart_comm.dat","wb");
    if(file) {
        fwrite(msg_invalid, 1, sizeof(msg_invalid), file);
        fflush(file);
        fclose(file);
    }

    receiverReset(&rx);

    bool rx_active = true;
    int cycle = 0;
    
    printf("  [ChecksumTest] RX-only scheduler iniciado\n");
    
    while(rx_active && cycle < 500) {
        cycle++;
        
        int rx_result = receiverThread(&rx);
        
        if(rx_result == PT_YIELDED) {
            if(cycle % 100 == 0) printf("  [ChecksumTest:%d] RX yielded\n", cycle);
        } else if(rx_result == PT_WAITING) {
            if(cycle % 100 == 0) printf("  [ChecksumTest:%d] RX waiting\n", cycle);
        } else if(rx_result == PT_ENDED) {
            printf("  [ChecksumTest:%d] RX ended\n", cycle);
            rx_active = false;
        } else if(rx_result == PT_EXITED) {
            printf("  [ChecksumTest:%d] RX exited with error\n", cycle);
            rx_active = false;
        }
        
        if(cycle % 50 == 0) {
            usleep(200);
        }
    }

    printf("  [ChecksumTest] RX scheduler finalizado em %d ciclos\n", cycle);

    printf("  Debug: rx.complete=%d, rx.error=%d\n", rx.complete, rx.error);
    printf("  Debug: rx.calculated_checksum=0x%02X, rx.received_checksum=0x%02X\n", 
           rx.calculated_checksum, rx.received_checksum);
    
    bool checksum_mismatch = (rx.calculated_checksum != rx.received_checksum);
    verifica("Receiver deveria detectar diferença no checksum", checksum_mismatch == true);

    printf("Teste de checksum inválido passou\n");
    return 0;
}

char* testReceiverInvalidSTXETX() {
    printf("Testando recepção com STX/ETX inválido.\n");

    receiverReset(&rx);

    unsigned char msg_invalid[] = {
        0x00,       // STX inválido
        3,          // QUANTITY
        'A','B','C',// DATA
        'A'^'B'^'C',// CHECKSUM correto
        0x04        // ETX inválido
    };

    FILE* file = fopen("/tmp/uart_comm.dat", "wb");
    fwrite(msg_invalid, 1, sizeof(msg_invalid), file);
    fflush(file);
    fclose(file);

    receiverReset(&rx);

    bool rx_active = true;
    int cycle = 0;
    
    printf("  [InvalidSTXETXTest] RX-only scheduler iniciado\n");
    
    while(rx_active && cycle < 1000) {
        cycle++;
        
        int rx_result = receiverThread(&rx);
        
        if(rx_result == PT_YIELDED) {
            if(cycle % 100 == 0) printf("  [InvalidSTXETXTest:%d] RX yielded\n", cycle);
        } else if(rx_result == PT_WAITING) {
            if(cycle % 100 == 0) printf("  [InvalidSTXETXTest:%d] RX waiting\n", cycle);
        } else if(rx_result == PT_ENDED) {
            printf("  [InvalidSTXETXTest:%d] RX ended\n", cycle);
            rx_active = false;
        } else if(rx_result == PT_EXITED) {
            printf("  [InvalidSTXETXTest:%d] RX exited with error\n", cycle);
            rx_active = false;
        }
        
        if(cycle % 50 == 0) {
            usleep(100);
        }
    }

    printf("  [InvalidSTXETXTest] RX scheduler finalizado em %d ciclos\n", cycle);

    verifica("Receiver não deveria completar com sucesso com STX inválido", rx.complete == false);

    printf("Teste de STX/ETX inválido passou\n");
    return 0;
}

char* testReceiverMultipleMessages() {
    printf("Testando recepção de múltiplas mensagens consecutivas.\n");

    unsigned char msg1[] = "ONE";
    unsigned char msg2[] = "TWO";

    for(int i=0; i<2; i++) {
        transmitterInit(&tx);
        receiverReset(&rx);

        unsigned char* currentMsg = (i==0)? msg1 : msg2;
        
        cooperative_scheduler(currentMsg, strlen((char*)currentMsg), "MultipleMessages");

        verifica("Receiver deveria receber mensagem completa", rx.complete == true);
    }

    printf("Teste de múltiplas mensagens passou\n");
    return 0;
}

char* testAckNakMechanism() {
    printf("Testando mecanismo de ACK/NAK.\n");

    transmitterInit(&tx);
    receiverReset(&rx);

    unsigned char msg[] = "ACK_TEST";
    size_t msg_len = strlen((char*)msg);

    cooperative_scheduler(msg, (int)msg_len, "AckTest");

    verifica("Transmitter deveria completar com sucesso (recebeu ACK)", tx.complete == true);
    verifica("Receiver deveria completar com sucesso", rx.complete == true);

    if(rx.complete && rx.data) {
        bool iguais = memcmp(rx.data, msg, msg_len) == 0;
        verifica("Dados ACK test deveriam ser iguais aos enviados", iguais == true);
    }

    FILE* ack_file = fopen("/tmp/uart_ack.dat", "rb");
    if(ack_file) {
        fseek(ack_file, 0, SEEK_END);
        long ack_size = ftell(ack_file);
        verifica("Arquivo ACK deveria ter sido criado", ack_size > 0);
        
        if(ack_size > 0) {
            fseek(ack_file, 0, SEEK_SET);
            unsigned char ack_byte = 0;
            fread(&ack_byte, 1, 1, ack_file);
            verifica("ACK deveria ter valor correto (0x06)", ack_byte == 0x06);
        }
        fclose(ack_file);
    }

    printf("Teste de ACK/NAK passou\n");
    return 0;
}

char* testNakOnChecksumError() {
    printf("Testando NAK em erro de checksum.\n");

    unsigned char msg_invalid[] = {0x02, 3, 'N','A','K', 0xFF, 0x03};
    FILE* file = fopen("/tmp/uart_comm.dat","wb");
    if(file) {
        fwrite(msg_invalid, 1, sizeof(msg_invalid), file);
        fflush(file);
        fclose(file);
    }

    receiverReset(&rx);

    bool rx_active = true;
    int cycle = 0;
    
    printf("  [NAKTest] RX-only scheduler para teste NAK iniciado\n");
    
    while(rx_active && cycle < 100) {
        cycle++;
        
        int rx_result = receiverThread(&rx);
        
        if(rx_result == PT_ENDED) {
            printf("  [NAKTest:%d] RX ended\n", cycle);
            rx_active = false;
        } else if(rx_result == PT_EXITED) {
            printf("  [NAKTest:%d] RX exited with error (NAK enviado)\n", cycle);
            rx_active = false;
        }
    }

    verifica("Receiver deveria detectar erro de checksum", rx.error == true);
    verifica("Receiver não deveria completar com sucesso", rx.complete == false);

    FILE* ack_file = fopen("/tmp/uart_ack.dat", "rb");
    if(ack_file) {
        fseek(ack_file, 0, SEEK_END);
        long ack_size = ftell(ack_file);
        verifica("Arquivo NAK deveria ter sido criado", ack_size > 0);
        
        if(ack_size > 0) {
            fseek(ack_file, 0, SEEK_SET);
            unsigned char nak_byte = 0;
            fread(&nak_byte, 1, 1, ack_file);
            verifica("NAK deveria ter valor correto (0x15)", nak_byte == 0x15);
        }
        fclose(ack_file);
    }

    printf("Teste de NAK em erro de checksum passou\n");
    return 0;
}

static char* runAllTests() {
    printf("\nEXECUTANDO TESTES DO PROTOCOLO (Protothread)\n");

    executa_teste(testReceiverSimpleMessage);
    executa_teste(testReceiverBinaryData);
    executa_teste(testReceiverEmptyMessage);
    executa_teste(testReceiverChecksumError);
    executa_teste(testReceiverInvalidSTXETX);
    executa_teste(testReceiverMultipleMessages);
    executa_teste(testAckNakMechanism);
    executa_teste(testNakOnChecksumError);

    printf("TODOS OS TESTES DO PROTOCOLO PASSARAM\n\n");
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
