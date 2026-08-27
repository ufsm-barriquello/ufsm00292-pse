#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define STX_BYTE 0x02
#define ETX_BYTE 0x03


typedef enum {
    RX_WAIT_STX,
    RX_WAIT_LEN,
    RX_READ_DATA,
    RX_WAIT_CHK,
    RX_WAIT_ETX
} rx_state_t;

rx_state_t rx_state = RX_WAIT_STX;
uint8_t rx_len = 0;
uint8_t rx_index = 0;
uint8_t rx_calc_chk = 0;
uint8_t rx_buffer[256];
bool rx_packet_valid = false;

void rx_process_byte(uint8_t byte) {
    switch (rx_state) {
        case RX_WAIT_STX:
            if (byte == STX_BYTE) {
                rx_calc_chk = 0;
                rx_index = 0;
                rx_packet_valid = false;
                rx_state = RX_WAIT_LEN;
            }
            break;
            
        case RX_WAIT_LEN:
            rx_len = byte;
            rx_calc_chk ^= byte; 
            rx_state = (rx_len == 0) ? RX_WAIT_CHK : RX_READ_DATA;
            break;
            
        case RX_READ_DATA:
            rx_buffer[rx_index++] = byte;
            rx_calc_chk ^= byte; 
            if (rx_index >= rx_len) {
                rx_state = RX_WAIT_CHK;
            }
            break;
            
        case RX_WAIT_CHK:
            if (byte == rx_calc_chk) {
                rx_state = RX_WAIT_ETX;
            } else {
                rx_state = RX_WAIT_STX; 
            }
            break;
            
        case RX_WAIT_ETX:
            if (byte == ETX_BYTE) {
                rx_packet_valid = true; 
            }
            rx_state = RX_WAIT_STX; 
            break;
            
        default:
            rx_state = RX_WAIT_STX;
            break;
    }
}

typedef enum {
    TX_IDLE,
    TX_SEND_STX,
    TX_SEND_LEN,
    TX_SEND_DATA,
    TX_SEND_CHK,
    TX_SEND_ETX
} tx_state_t;

tx_state_t tx_state = TX_IDLE;
uint8_t* tx_data_ptr = NULL;
uint8_t tx_len = 0;
uint8_t tx_index = 0;
uint8_t tx_calc_chk = 0;

void tx_start(uint8_t* data, uint8_t len) {
    tx_data_ptr = data;
    tx_len = len;
    tx_index = 0;
    tx_state = TX_SEND_STX;
}

// retorna true se houver byte para enviar, salva no ponteiro out_byte
bool tx_process_byte(uint8_t* out_byte) {
    switch (tx_state) {
        case TX_IDLE:
            return false;
            
        case TX_SEND_STX:
            *out_byte = STX_BYTE;
            tx_calc_chk = 0;
            tx_state = TX_SEND_LEN;
            return true;
            
        case TX_SEND_LEN:
            *out_byte = tx_len;
            tx_calc_chk ^= tx_len;
            tx_state = (tx_len > 0) ? TX_SEND_DATA : TX_SEND_CHK;
            return true;
            
        case TX_SEND_DATA:
            *out_byte = tx_data_ptr[tx_index];
            tx_calc_chk ^= *out_byte;
            tx_index++;
            if (tx_index >= tx_len) {
                tx_state = TX_SEND_CHK;
            }
            return true;
            
        case TX_SEND_CHK:
            *out_byte = tx_calc_chk;
            tx_state = TX_SEND_ETX;
            return true;
            
        case TX_SEND_ETX:
            *out_byte = ETX_BYTE;
            tx_state = TX_IDLE;
            return true;
            
        default:
            tx_state = TX_IDLE;
            return false;
    }
}

void run_tests_rx_valid_packet() {
    rx_state = RX_WAIT_STX;
    
    rx_process_byte(0x02); // Envia STX
    assert(rx_state == RX_WAIT_LEN);
    
    rx_process_byte(0x02); // Envia LEN = 2
    assert(rx_state == RX_READ_DATA);
    
    rx_process_byte(0xAA); // Envia DADOS[0]
    assert(rx_state == RX_READ_DATA);
    
    rx_process_byte(0xBB); // Envia DADOS[1]
    assert(rx_state == RX_WAIT_CHK);
    
    // CHK esperado = 0x02 (LEN) ^ 0xAA ^ 0xBB = 0x13
    rx_process_byte(0x13); 
    assert(rx_state == RX_WAIT_ETX);
    
    rx_process_byte(0x03); // Envia ETX
    assert(rx_state == RX_WAIT_STX); 
    assert(rx_packet_valid == true); // Pacote validado com sucesso!
    
    printf("[OK] TDD RX - Recepcao de pacote valido processada corretamente.\n");
}

void run_tests_rx_invalid_checksum() {
    rx_state = RX_WAIT_STX;
    
    rx_process_byte(0x02); // STX
    rx_process_byte(0x01); // LEN = 1
    rx_process_byte(0xFF); // DADOS[0]
    
    
    rx_process_byte(0x00); 
    assert(rx_state == RX_WAIT_STX); 
    
    printf("[OK] TDD RX - Falha de checksum identificada e maquina resetada.\n");
}

void run_tests_tx_generation() {
    uint8_t payload[] = {0x11, 0x22};
    uint8_t byte_to_send = 0;
    
    tx_start(payload, 2);
    
    assert(tx_process_byte(&byte_to_send) == true);
    assert(byte_to_send == 0x02); // STX
    
    assert(tx_process_byte(&byte_to_send) == true);
    assert(byte_to_send == 0x02); // LEN = 2
    
    assert(tx_process_byte(&byte_to_send) == true);
    assert(byte_to_send == 0x11); // DATA 0
    
    assert(tx_process_byte(&byte_to_send) == true);
    assert(byte_to_send == 0x22); // DATA 1
    
    assert(tx_process_byte(&byte_to_send) == true);
    assert(byte_to_send == (0x02 ^ 0x11 ^ 0x22)); // CHK
    
    assert(tx_process_byte(&byte_to_send) == true);
    assert(byte_to_send == 0x03); // ETX
    
    assert(tx_process_byte(&byte_to_send) == false); // IDLE, acabou a transmissao
    
    printf("[OK] TDD TX - Geracao de pacote na transmissao testada byte a byte.\n");
}

int main() {
    printf("\nIniciando testes TDD\n\n");
    
    run_tests_rx_valid_packet();
    run_tests_rx_invalid_checksum();
    run_tests_tx_generation();
    
    printf("         TODOS OS TESTES FORAM REALIZADOS COM SUCESSO!      \n");
    
    return 0;
}