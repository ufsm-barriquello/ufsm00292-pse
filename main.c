#include <stdio.h>
#include <stdint.h>

#define START_BYTE 0xAA
#define END_BYTE   0xBB
#define ACK  0x55
#define NACK 0x33

// buffers simulados
uint8_t tx_buffer[64];
uint8_t rx_buffer[64];
uint8_t ack_channel = 0;

// --------------------------------------------------
// Utilidades
// --------------------------------------------------
uint8_t checksum(uint8_t *data, uint8_t len) {
    uint16_t s = 0;
    for(int i=0;i<len;i++) s += data[i];
    return (uint8_t)(s & 0xFF);
}

void send_packet(uint8_t n){
    for(int i=0;i<n;i++)
        rx_buffer[i] = tx_buffer[i];
}

// --------------------------------------------------
// Máquina de estados TRANSMISSOR
// --------------------------------------------------
typedef enum {
    TX_IDLE,
    TX_MONTAR,
    TX_ENVIAR,
    TX_ESPERAR_ACK,
    TX_SUCESSO,
    TX_REENVIO
} tx_state_t;

tx_state_t tx_state = TX_IDLE;

// --------------------------------------------------
// Máquina de estados RECEPTOR
// --------------------------------------------------
typedef enum {
    RX_IDLE,
    RX_START,
    RX_LEN,
    RX_PAYLOAD,
    RX_CHECK,
    RX_END,
    RX_ACK_SEND,
    RX_ERROR
} rx_state_t;

rx_state_t rx_state = RX_IDLE;

// --------------------------------------------------
// DADOS
// --------------------------------------------------
uint8_t data[] = {10,20,30};
uint8_t len = 3;

// --------------------------------------------------
// FSM Transmissor
// --------------------------------------------------
void transmitter_tick(){
    static uint8_t timeout = 0;

    switch(tx_state){

        case TX_IDLE:
            printf("[TX] Preparando dados...\n");
            tx_state = TX_MONTAR;
            break;

        case TX_MONTAR:
            tx_buffer[0] = START_BYTE;
            tx_buffer[1] = len;
            for(int i=0;i<len;i++)
                tx_buffer[2+i] = data[i];
            tx_buffer[2+len] = checksum(data,len);
            tx_buffer[3+len] = END_BYTE;
            tx_state = TX_ENVIAR;
            break;

        case TX_ENVIAR:
            printf("[TX] Enviando pacote\n");
            send_packet(4+len);
            timeout = 0;
            ack_channel = 0;
            tx_state = TX_ESPERAR_ACK;
            break;

        case TX_ESPERAR_ACK:
            if(ack_channel == ACK){
                tx_state = TX_SUCESSO;
            }
            else if(timeout > 3){
                tx_state = TX_REENVIO;
            }
            else{
                timeout++;
            }
            break;

        case TX_SUCESSO:
            printf("[TX] ACK recebido ✔️\n");
            tx_state = TX_IDLE;
            break;

        case TX_REENVIO:
            printf("[TX] Timeout → Reenviando...\n");
            tx_state = TX_MONTAR;
            break;
    }
}

// --------------------------------------------------
// FSM Receptor
// --------------------------------------------------
void receiver_tick(){
    static uint8_t idx = 0;
    static uint8_t len_rx = 0;

    switch(rx_state){
        case RX_IDLE:
            if(rx_buffer[0] == START_BYTE){
                rx_state = RX_LEN;
            }
            break;

        case RX_LEN:
            len_rx = rx_buffer[1];
            rx_state = RX_PAYLOAD;
            break;

        case RX_PAYLOAD:
            idx = 2 + len_rx;
            rx_state = RX_CHECK;
            break;

        case RX_CHECK:{
            uint8_t ok = checksum(&rx_buffer[2], len_rx) == rx_buffer[idx];
            rx_state = ok ? RX_END : RX_ERROR;
        }
        break;

        case RX_END:
            if(rx_buffer[idx+1] == END_BYTE){
                printf("[RX] Pacote OK → ACK\n");
                ack_channel = ACK;
                rx_state = RX_IDLE;
            }
            else{
                rx_state = RX_ERROR;
            }
            break;

        case RX_ERROR:
            printf("[RX] Pacote inválido → NACK\n");
            ack_channel = NACK;
            rx_state = RX_IDLE;
            break;
    }
}

// --------------------------------------------------
// MAIN (simulação)
// --------------------------------------------------
int main(){
    for(int i=0;i<10;i++){
        transmitter_tick();
        receiver_tick();
    }
}
