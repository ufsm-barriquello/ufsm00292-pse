#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "pt.h"

// ================= PROTOCOLO =================
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x15
#define MAX_DATA 256
#define MAX_RETRIES 3

// ================= ESTRUTURAS =================
typedef struct {
    unsigned char data[MAX_DATA];
    unsigned char size;
    unsigned char chk;
} Packet;

// ================= CANAL SIMULADO =================
static struct pt_sem data_sem;
static struct pt_sem ack_sem;
static unsigned char channel_data;
static unsigned char channel_ack;

// ================= VARIÁVEIS GLOBAIS =================
Packet tx_packet;
Packet rx_packet;

// ================= FUNÇÕES AUXILIARES =================
unsigned char calculate_checksum(Packet *pkt) {
    unsigned char chk = STX;
    chk ^= pkt->size;
    for(int i = 0; i < pkt->size; i++)
        chk ^= pkt->data[i];
    return chk;
}

int is_valid_packet_size(unsigned char size) {
    return size <= MAX_DATA;
}

void send_byte(struct pt *pt, unsigned char byte) {
    channel_data = byte;
    PT_SEM_SIGNAL(pt, &data_sem);
}

unsigned char receive_byte(struct pt *pt) {
    PT_SEM_WAIT(pt, &data_sem);
    return channel_data;
}

void send_ack(struct pt *pt, unsigned char ack) {
    channel_ack = ack;
    PT_SEM_SIGNAL(pt, &ack_sem);
}

unsigned char receive_ack(struct pt *pt) {
    PT_SEM_WAIT(pt, &ack_sem);
    return channel_ack;
}

// ================= PROTOTHREAD RECEPTORA =================
static PT_THREAD(protothread_rx(struct pt *pt)) {
    static unsigned char byte;
    static unsigned char idx;

    PT_BEGIN(pt);

    while(1) {
        // Recebe STX
        byte = receive_byte(pt);
        if(byte != STX) continue;

        // Recebe tamanho
        rx_packet.size = receive_byte(pt);
        if(!is_valid_packet_size(rx_packet.size)) {
            send_ack(pt, NAK);
            continue;
        }

        // Recebe dados
        idx = 0;
        while(idx < rx_packet.size) {
            rx_packet.data[idx++] = receive_byte(pt);
        }

        // Recebe checksum
        rx_packet.chk = receive_byte(pt);

        // Verifica checksum
        if(calculate_checksum(&rx_packet) != rx_packet.chk) {
            send_ack(pt, NAK);
            continue;
        }

        // Espera ETX
        byte = receive_byte(pt);
        if(byte != ETX) {
            send_ack(pt, NAK);
            continue;
        }

        // Pacote correto
        send_ack(pt, ACK);

        PT_YIELD(pt);
    }

    PT_END(pt);
}

// ================= PROTOTHREAD TRANSMISSORA =================
static PT_THREAD(protothread_tx(struct pt *pt)) {
    static unsigned char idx;
    static unsigned char retry_count;
    static unsigned char ack;

    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, tx_packet.size > 0);
        retry_count = 0;

        retransmit:
        send_byte(pt, STX);
        send_byte(pt, tx_packet.size);

        idx = 0;
        while(idx < tx_packet.size) {
            send_byte(pt, tx_packet.data[idx++]);
        }

        tx_packet.chk = calculate_checksum(&tx_packet);
        send_byte(pt, tx_packet.chk);
        send_byte(pt, ETX);

        ack = receive_ack(pt);

        if(ack == ACK) {
            tx_packet.size = 0;
        } else if(ack == NAK) {
            retry_count++;
            if(retry_count < MAX_RETRIES) goto retransmit;
            else tx_packet.size = 0;
        }

        PT_YIELD(pt);
    }

    PT_END(pt);
}

// ================= TESTES TDD =================
void test_checksum() {
    Packet pkt = {{0x41, 0x42, 0x43}, 3, 0};
    unsigned char chk = calculate_checksum(&pkt);
    unsigned char expected = STX ^ 0x03 ^ 0x41 ^ 0x42 ^ 0x43;
    assert(chk == expected);
    printf("Checksum calculado corretamente: 0x%02X\n", chk);
}

void test_packet_validation() {
    assert(is_valid_packet_size(0));
    assert(is_valid_packet_size(MAX_DATA));
    assert(!is_valid_packet_size(MAX_DATA + 1));
    printf("Validação de tamanho OK\n");
}

void test_ack_system() {
    struct pt pt;
    PT_INIT(&pt);
    PT_SEM_INIT(&data_sem, 0);
    PT_SEM_INIT(&ack_sem, 0);

    send_ack(&pt, ACK);
    assert(channel_ack == ACK);

    send_ack(&pt, NAK);
    assert(channel_ack == NAK);

    printf("Sistema ACK/NAK OK\n");
}

void test_complete_protocol() {
    struct pt pt_rx, pt_tx;
    PT_INIT(&pt_rx);
    PT_INIT(&pt_tx);
    PT_SEM_INIT(&data_sem, 0);
    PT_SEM_INIT(&ack_sem, 0);

    tx_packet.size = 3;
    tx_packet.data[0] = 0x41;
    tx_packet.data[1] = 0x42;
    tx_packet.data[2] = 0x43;

    for(int i=0; i<20; i++) {
        protothread_tx(&pt_tx);
        protothread_rx(&pt_rx);
        if(tx_packet.size==0) break;
    }

    assert(rx_packet.size == 3);
    assert(rx_packet.data[0]==0x41);
    assert(rx_packet.data[1]==0x42);
    assert(rx_packet.data[2]==0x43);

    printf("Protocolo completo funcionando\n");
}

void run_all_tests() {
    printf("INICIANDO TESTES TDD...\n");
    test_checksum();
    test_packet_validation();
    test_ack_system();
    test_complete_protocol();
    printf("TODOS OS TESTES PASSARAM!\n");
}

// ================= DEMONSTRAÇÃO =================
void demonstration() {
    struct pt pt_rx, pt_tx;
    PT_INIT(&pt_rx);
    PT_INIT(&pt_tx);
    PT_SEM_INIT(&data_sem, 0);
    PT_SEM_INIT(&ack_sem, 0);

    tx_packet.size = 5;
    tx_packet.data[0]='H';
    tx_packet.data[1]='E';
    tx_packet.data[2]='L';
    tx_packet.data[3]='L';
    tx_packet.data[4]='O';

    printf("Transmitindo: HELLO\n");

    for(int i=0;i<30;i++) {
        protothread_tx(&pt_tx);
        protothread_rx(&pt_rx);
        if(tx_packet.size==0) {
            printf("Transmissão completada com sucesso!\n");
            break;
        }
    }
}

// ================= MAIN =================
int main() {
    run_all_tests();
    demonstration();
    return 0;
}
