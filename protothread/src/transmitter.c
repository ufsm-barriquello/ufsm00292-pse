#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "transmitter.h"

#define COMM_FILE "/tmp/uart_comm.dat"
#define ACK_FILE  "/tmp/uart_ack.dat"

#define ACK 0x06
#define NAK 0x15
#define MAX_RETRIES 3
#define TIMEOUT 100

static bool transmitByte(unsigned char byte) {
    FILE* file = fopen(COMM_FILE, "ab");
    if(!file) return false;
    fwrite(&byte, 1, 1, file);
    fflush(file);
    fclose(file);
    return true;
}

static int waitForAck() {
    FILE* file = fopen(ACK_FILE, "rb");
    if(!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if(size <= 0) { fclose(file); return -1; }

    fseek(file, 0, SEEK_SET);
    unsigned char resp = 0;
    fread(&resp, 1, 1, file);
    fclose(file);

    if(resp == ACK) return 1;
    if(resp == NAK) return 0;
    return -1;
}

void transmitterInit(struct transmitter* t) {
    PT_INIT(&t->pt);
    t->data = NULL;
    t->data_size = 0;
    t->index = 0;
    t->checksum = 0;
    t->complete = false;

    FILE* file = fopen(COMM_FILE, "wb"); if(file) fclose(file);
    file = fopen(ACK_FILE, "wb"); if(file) fclose(file);
}

PT_THREAD(transmitterThread(struct transmitter* t, unsigned char* data, int size)) {
    static unsigned char byte;
    static int retries;
    static int timeout_counter;

    PT_BEGIN(&t->pt);

    t->data = data;
    t->data_size = size;
    t->index = 0;
    t->checksum = 0;
    t->complete = false;
    retries = 0;

send_packet:
    // Limpa ACK_FILE
    FILE* ackfile = fopen(ACK_FILE, "wb"); if(ackfile) fclose(ackfile);

    // Envia STX - cede controle após cada byte
    PT_WAIT_UNTIL(&t->pt, transmitByte(STX));
    PT_YIELD(&t->pt);  // Cede controle após STX

    // Envia tamanho - cede controle
    PT_WAIT_UNTIL(&t->pt, transmitByte((unsigned char)t->data_size));
    PT_YIELD(&t->pt);  // Cede controle após tamanho

    // Envia dados - 1 byte por vez com yield
    t->checksum = 0;
    t->index = 0;
    while(t->index < t->data_size) {
        byte = t->data[t->index];
        PT_WAIT_UNTIL(&t->pt, transmitByte(byte));
        t->checksum ^= byte;
        t->index++;
        PT_YIELD(&t->pt);  // Cede controle após cada byte de dados
    }

    // Envia checksum - cede controle
    PT_WAIT_UNTIL(&t->pt, transmitByte(t->checksum));
    PT_YIELD(&t->pt);  // Cede controle após checksum

    // Envia ETX - cede controle
    PT_WAIT_UNTIL(&t->pt, transmitByte(ETX));
    PT_YIELD(&t->pt);  // Cede controle após ETX

    // Aguarda ACK/NAK ou timeout
    timeout_counter = 0;
    while(1) {
        int resp = waitForAck();
        if(resp == 1) {           // ACK
            t->complete = true;
            break;
        } else if(resp == 0) {    // NAK
            retries++;
            if(retries < MAX_RETRIES) goto send_packet;
            t->complete = false;
            break;
        } else {
            PT_YIELD(&t->pt);     // cede execução
            timeout_counter++;
            if(timeout_counter > TIMEOUT) {
                retries++;
                if(retries < MAX_RETRIES) goto send_packet;
                t->complete = false;
                break;
            }
        }
    }

    PT_END(&t->pt);
}
