#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "receiver.h"

#define COMM_FILE "/tmp/uart_comm.dat"
#define ACK_FILE "/tmp/uart_ack.dat"
#define ACK 0x06
#define NAK 0x15

static void sendAck(bool success) {
    FILE* file = fopen(ACK_FILE, "wb");
    if(!file) return;
    unsigned char resp = success ? ACK : NAK;
    fwrite(&resp, 1, 1, file);
    fflush(file);
    fclose(file);
}

static bool readByte(struct receiver* r, unsigned char* byte) {
    if (!r->file) return false;
    
    // Verifica se há dados disponíveis na posição atual
    fseek(r->file, 0, SEEK_END);
    long file_size = ftell(r->file);
    
    // Se não há dados suficientes, retorna false para simular espera por dados
    if (r->read_offset >= file_size) {
        return false;
    }
    
    fseek(r->file, r->read_offset, SEEK_SET);
    size_t read_bytes = fread(byte, 1, 1, r->file);
    if(read_bytes == 1) {
        r->read_offset++;
        return true;
    }
    return false;
}

void receiverReset(struct receiver* r) {
    PT_INIT(&r->pt);

    if(r->data) { free(r->data); r->data = NULL; }
    if(r->file) { fclose(r->file); r->file = NULL; }

    r->file = fopen(COMM_FILE, "rb");
    if(!r->file) {
        perror("Erro ao abrir /tmp/uart_comm.dat");
        r->error = true;
    }

    r->expected_size = 0;
    r->received_index = 0;
    r->received_checksum = 0;
    r->calculated_checksum = 0;
    r->complete = false;
    r->error = false;
    r->read_offset = 0;
}

PT_THREAD(receiverThread(struct receiver* r)) {
    static unsigned char byte;

    PT_BEGIN(&r->pt);

    r->complete = false;
    r->error = false;

    // Espera STX - cede controle enquanto aguarda
    PT_WAIT_UNTIL(&r->pt, readByte(r, &byte));
    if (byte != STX) { r->error = true; sendAck(false); PT_EXIT(&r->pt); }
    PT_YIELD(&r->pt);  // Cede controle após receber STX

    // Espera tamanho - cede controle
    PT_WAIT_UNTIL(&r->pt, readByte(r, &byte));
    r->expected_size = byte;
    r->received_index = 0;
    r->calculated_checksum = 0;
    PT_YIELD(&r->pt);  // Cede controle após receber tamanho

    if (r->expected_size > 0) {
        if(r->data) free(r->data);
        r->data = (unsigned char*)malloc((size_t)r->expected_size);
        if(!r->data) { r->error = true; sendAck(false); PT_EXIT(&r->pt); }

        // Recebe dados 1 byte por vez com yield
        while(r->received_index < r->expected_size) {
            PT_WAIT_UNTIL(&r->pt, readByte(r, &byte));
            r->data[r->received_index++] = byte;
            r->calculated_checksum ^= byte;
            PT_YIELD(&r->pt);  // Cede controle após cada byte de dados
        }
    }

    // Espera CHECKSUM - cede controle
    PT_WAIT_UNTIL(&r->pt, readByte(r, &byte));
    r->received_checksum = byte;
    if(r->received_checksum != r->calculated_checksum) {
        r->error = true;
        sendAck(false);
        PT_EXIT(&r->pt);
    }
    PT_YIELD(&r->pt);  // Cede controle após receber checksum

    // Espera ETX - cede controle
    PT_WAIT_UNTIL(&r->pt, readByte(r, &byte));
    if(byte != ETX) { r->error = true; sendAck(false); PT_EXIT(&r->pt); }
    PT_YIELD(&r->pt);  // Cede controle após receber ETX

    r->complete = true;
    sendAck(true);

    // Limpa arquivo de comunicação após pacote
    fclose(r->file);
    r->file = fopen(COMM_FILE, "wb");
    if(r->file) fclose(r->file);
    r->file = fopen(COMM_FILE, "rb");

    PT_END(&r->pt);
}
