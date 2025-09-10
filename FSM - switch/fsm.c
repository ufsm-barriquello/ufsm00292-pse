#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/**********************
 * DEFINIÇÕES DO PROTOCOLO
 **********************/
#define STX 0x02
#define ETX 0x03
#define MAX_DATA_SIZE 255

/**********************
 * MÁQUINA DE ESTADOS
 **********************/
typedef enum {
    // Estados do Receptor
    RX_WAIT_STX,
    RX_WAIT_QTD,
    RX_READ_DATA,
    RX_CHECK_CHK,
    RX_WAIT_ETX,
    RX_DONE,
    RX_ERROR,
    
    // Estados do Transmissor
    TX_SEND_STX,
    TX_SEND_QTD,
    TX_SEND_DATA,
    TX_SEND_CHK,
    TX_SEND_ETX,
    TX_DONE,
    TX_ERROR
} ProtocolState;

typedef struct {
    // Receptor
    ProtocolState rx_state;
    uint8_t rx_data[MAX_DATA_SIZE];
    uint8_t rx_expected_bytes;
    uint8_t rx_received_bytes;
    uint8_t rx_calculated_chk;
    
    // Transmissor
    ProtocolState tx_state;
    const uint8_t* tx_data;
    uint8_t tx_data_len;
    uint8_t tx_sent_bytes;
    uint8_t tx_calculated_chk;
} Protocol;

/**********************
 * FUNÇÕES AUXILIARES
 **********************/
uint8_t calculate_checksum(const uint8_t* data, uint8_t length) {
    uint8_t chk = 0;
    for (uint8_t i = 0; i < length; i++) {
        chk ^= data[i];
    }
    return chk;
}

void protocol_init(Protocol* proto) {
    // Inicializa receptor
    proto->rx_state = RX_WAIT_STX;
    proto->rx_received_bytes = 0;
    proto->rx_expected_bytes = 0;
    proto->rx_calculated_chk = 0;
    
    // Inicializa transmissor
    proto->tx_state = TX_SEND_STX;
    proto->tx_sent_bytes = 0;
    proto->tx_calculated_chk = 0;
    proto->tx_data = NULL;
    proto->tx_data_len = 0;
}

/**********************
 * RECEPTOR (Máquina de Estados)
 **********************/
bool protocol_rx_byte(Protocol* proto, uint8_t byte) {
    switch (proto->rx_state) {
        case RX_WAIT_STX:
            if (byte == STX) {
                proto->rx_state = RX_WAIT_QTD;
                proto->rx_calculated_chk = byte;
            }
            break;
            
        case RX_WAIT_QTD:
            proto->rx_expected_bytes = byte;
            proto->rx_received_bytes = 0;
            proto->rx_state = RX_READ_DATA;
            proto->rx_calculated_chk ^= byte;
            break;
            
        case RX_READ_DATA:
            if (proto->rx_received_bytes < MAX_DATA_SIZE) {
                proto->rx_data[proto->rx_received_bytes++] = byte;
                proto->rx_calculated_chk ^= byte;
                if (proto->rx_received_bytes >= proto->rx_expected_bytes) {
                    proto->rx_state = RX_CHECK_CHK;
                }
            } else {
                proto->rx_state = RX_ERROR;
            }
            break;
            
        case RX_CHECK_CHK:
            if (proto->rx_calculated_chk == byte) {
                proto->rx_state = RX_WAIT_ETX;
            } else {
                proto->rx_state = RX_ERROR;
            }
            break;
            
        case RX_WAIT_ETX:
            if (byte == ETX) {
                proto->rx_state = RX_DONE;
                return true;
            } else {
                proto->rx_state = RX_ERROR;
            }
            break;
            
        case RX_ERROR:
            // Mantém no estado de erro até ser reinicializado
            break;
            
        case RX_DONE:
            // Mantém no estado de conclusão até ser reinicializado
            return true;
    }
    
    return false;
}

/**********************
 * TRANSMISSOR (Máquina de Estados)
 **********************/
void protocol_tx_begin(Protocol* proto, const uint8_t* data, uint8_t length) {
    proto->tx_state = TX_SEND_STX;
    proto->tx_data = data;
    proto->tx_data_len = length;
    proto->tx_sent_bytes = 0;
    proto->tx_calculated_chk = 0;
}

bool protocol_tx_byte(Protocol* proto, uint8_t* byte) {
    switch (proto->tx_state) {
        case TX_SEND_STX:
            *byte = STX;
            proto->tx_calculated_chk = *byte;
            proto->tx_state = TX_SEND_QTD;
            return false;
            
        case TX_SEND_QTD:
            *byte = proto->tx_data_len;
            proto->tx_calculated_chk ^= *byte;
            proto->tx_state = TX_SEND_DATA;
            proto->tx_sent_bytes = 0;
            return false;
            
        case TX_SEND_DATA:
            if (proto->tx_sent_bytes < proto->tx_data_len) {
                *byte = proto->tx_data[proto->tx_sent_bytes++];
                proto->tx_calculated_chk ^= *byte;
                if (proto->tx_sent_bytes >= proto->tx_data_len) {
                    proto->tx_state = TX_SEND_CHK;
                }
                return false;
            }
            break;
            
        case TX_SEND_CHK:
            *byte = proto->tx_calculated_chk;
            proto->tx_state = TX_SEND_ETX;
            return false;
            
        case TX_SEND_ETX:
            *byte = ETX;
            proto->tx_state = TX_DONE;
            return true;
            
        case TX_ERROR:
            return false;
            
        case TX_DONE:
            return true;
    }
    
    return false;
}

/**********************
 * TESTES (TDD)
 **********************/
void test_calculate_checksum() {
    printf("=== Teste calculate_checksum ===\n");
    
    // Teste 1: Checksum de array vazio
    uint8_t empty_data[] = {};
    uint8_t result = calculate_checksum(empty_data, 0);
    assert(result == 0);
    printf("Checksum de array vazio: 0x%02X ✓\n", result);
    
    // Teste 2: Checksum de dados simples
    uint8_t simple_data[] = {0x01, 0x02};
    result = calculate_checksum(simple_data, 2);
    assert(result == 0x03);
    printf("Checksum de [0x01, 0x02]: 0x%02X ✓\n", result);
    
    // Teste 3: Checksum com mais dados - Vamos calcular corretamente
    uint8_t complex_data[] = {0x41, 0x42, 0x43, 0x44};
    result = calculate_checksum(complex_data, 4);
    
    // Cálculo manual para verificação:
    // 0x41 ^ 0x42 = 0x03
    // 0x03 ^ 0x43 = 0x40
    // 0x40 ^ 0x44 = 0x04
    uint8_t expected = 0x41 ^ 0x42 ^ 0x43 ^ 0x44;
    assert(result == expected);
    printf("Checksum de [0x41, 0x42, 0x43, 0x44]: 0x%02X (esperado: 0x%02X) ✓\n", result, expected);
}

void test_protocol_init() {
    printf("\n=== Teste protocol_init ===\n");
    
    Protocol proto;
    protocol_init(&proto);
    
    assert(proto.rx_state == RX_WAIT_STX);
    assert(proto.rx_received_bytes == 0);
    assert(proto.rx_expected_bytes == 0);
    assert(proto.rx_calculated_chk == 0);
    
    assert(proto.tx_state == TX_SEND_STX);
    assert(proto.tx_sent_bytes == 0);
    assert(proto.tx_calculated_chk == 0);
    assert(proto.tx_data == NULL);
    assert(proto.tx_data_len == 0);
    
    printf("Protocol inicializado corretamente ✓\n");
}

void test_rx_valid_packet() {
    printf("\n=== Teste RX: Pacote válido ===\n");
    
    Protocol proto;
    protocol_init(&proto);
    
    // Pacote válido: STX, tamanho=2, dados=[0x01, 0x02], checksum, ETX
    uint8_t packet_data[] = {STX, 0x02, 0x01, 0x02};
    uint8_t checksum = calculate_checksum(packet_data, 4);
    uint8_t valid_packet[] = {STX, 0x02, 0x01, 0x02, checksum, ETX};
    
    bool complete = false;
    for (int i = 0; i < sizeof(valid_packet); i++) {
        complete = protocol_rx_byte(&proto, valid_packet[i]);
        printf("Byte %d: 0x%02X - Estado: %d - Completo: %d\n", 
              i, valid_packet[i], proto.rx_state, complete);
    }
    
    assert(complete == true);
    assert(proto.rx_state == RX_DONE);
    assert(proto.rx_received_bytes == 2);
    assert(proto.rx_data[0] == 0x01);
    assert(proto.rx_data[1] == 0x02);
    
    printf("Pacote válido recebido com sucesso ✓\n");
}

void test_rx_invalid_checksum() {
    printf("\n=== Teste RX: Checksum inválido ===\n");
    
    Protocol proto;
    protocol_init(&proto);
    
    // Pacote com checksum inválido
    uint8_t invalid_packet[] = {STX, 0x02, 0x01, 0x02, 0x00, ETX}; // CHK errado
    
    bool complete = false;
    for (int i = 0; i < sizeof(invalid_packet); i++) {
        complete = protocol_rx_byte(&proto, invalid_packet[i]);
        printf("Byte %d: 0x%02X - Estado: %d - Completo: %d\n", 
              i, invalid_packet[i], proto.rx_state, complete);
    }
    
    assert(complete == false);
    assert(proto.rx_state == RX_ERROR);
    
    printf("Pacote com checksum inválido rejeitado corretamente ✓\n");
}

void test_rx_missing_etx() {
    printf("\n=== Teste RX: ETX ausente ===\n");
    
    Protocol proto;
    protocol_init(&proto);
    
    // Pacote sem ETX
    uint8_t packet_data[] = {STX, 0x02, 0x01, 0x02};
    uint8_t checksum = calculate_checksum(packet_data, 4);
    uint8_t packet[] = {STX, 0x02, 0x01, 0x02, checksum, 0x00}; // Não é ETX
    
    bool complete = false;
    for (int i = 0; i < sizeof(packet); i++) {
        complete = protocol_rx_byte(&proto, packet[i]);
        printf("Byte %d: 0x%02X - Estado: %d - Completo: %d\n", 
              i, packet[i], proto.rx_state, complete);
    }
    
    assert(complete == false);
    assert(proto.rx_state == RX_ERROR);
    
    printf("Pacote sem ETX rejeitado corretamente ✓\n");
}

void test_tx_transmission() {
    printf("\n=== Teste TX: Transmissão completa ===\n");
    
    Protocol proto;
    protocol_init(&proto);
    
    uint8_t tx_data[] = {0x01, 0x02};
    protocol_tx_begin(&proto, tx_data, sizeof(tx_data));
    
    uint8_t tx_byte;
    bool tx_complete;
    int step = 0;
    
    // STX
    tx_complete = protocol_tx_byte(&proto, &tx_byte);
    assert(tx_byte == STX);
    assert(tx_complete == false);
    printf("Step %d: 0x%02X (STX) ✓\n", step++, tx_byte);
    
    // Tamanho
    tx_complete = protocol_tx_byte(&proto, &tx_byte);
    assert(tx_byte == 0x02);
    assert(tx_complete == false);
    printf("Step %d: 0x%02X (QTD) ✓\n", step++, tx_byte);
    
    // Dado 1
    tx_complete = protocol_tx_byte(&proto, &tx_byte);
    assert(tx_byte == 0x01);
    assert(tx_complete == false);
    printf("Step %d: 0x%02X (DATA1) ✓\n", step++, tx_byte);
    
    // Dado 2
    tx_complete = protocol_tx_byte(&proto, &tx_byte);
    assert(tx_byte == 0x02);
    assert(tx_complete == false);
    printf("Step %d: 0x%02X (DATA2) ✓\n", step++, tx_byte);
    
    // Checksum
    tx_complete = protocol_tx_byte(&proto, &tx_byte);
    uint8_t expected_chk = calculate_checksum((uint8_t[]){STX, 0x02, 0x01, 0x02}, 4);
    assert(tx_byte == expected_chk);
    assert(tx_complete == false);
    printf("Step %d: 0x%02X (CHK) ✓\n", step++, tx_byte);
    
    // ETX
    tx_complete = protocol_tx_byte(&proto, &tx_byte);
    assert(tx_byte == ETX);
    assert(tx_complete == true);
    printf("Step %d: 0x%02X (ETX) ✓\n", step++, tx_byte);
    
    printf("Transmissão completada com sucesso ✓\n");
}

void test_full_cycle() {
    printf("\n=== Teste: Ciclo completo TX/RX ===\n");
    
    Protocol proto;
    
    // Dados para transmitir
    uint8_t data_to_send[] = {0x41, 0x42, 0x43}; // "ABC"
    
    // Transmite
    printf("Transmitindo...\n");
    protocol_init(&proto);
    protocol_tx_begin(&proto, data_to_send, sizeof(data_to_send));
    
    uint8_t tx_buffer[256];
    int tx_index = 0;
    
    uint8_t tx_byte;
    bool tx_done;
    do {
        tx_done = protocol_tx_byte(&proto, &tx_byte);
        tx_buffer[tx_index++] = tx_byte;
        printf("TX: 0x%02X\n", tx_byte);
    } while (!tx_done);
    
    // Recebe
    printf("Recebendo...\n");
    protocol_init(&proto);
    bool rx_done = false;
    for (int i = 0; i < tx_index; i++) {
        rx_done = protocol_rx_byte(&proto, tx_buffer[i]);
        printf("RX: 0x%02X - Estado: %d\n", tx_buffer[i], proto.rx_state);
    }
    
    assert(rx_done == true);
    assert(proto.rx_received_bytes == 3);
    assert(proto.rx_data[0] == 0x41);
    assert(proto.rx_data[1] == 0x42);
    assert(proto.rx_data[2] == 0x43);
    
    printf("Ciclo completo TX/RX bem-sucedido ✓\n");
}

void run_all_tests() {
    printf("Iniciando testes TDD...\n");
    
    test_calculate_checksum();
    test_protocol_init();
    test_rx_valid_packet();
    test_rx_invalid_checksum();
    test_rx_missing_etx();
    test_tx_transmission();
    test_full_cycle();
    
    printf("\n Todos os testes passaram!\n");
}

/**********************
 * FUNCIONAMENTO
 **********************/
int main() {
    // Executa todos os testes
    run_all_tests();
    
    // Exemplo de uso completo
    printf("\n=== Exemplo de uso completo ===\n");
    Protocol proto;
    protocol_init(&proto);
    
    // Dados para transmitir
    uint8_t data_to_send[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
    protocol_tx_begin(&proto, data_to_send, sizeof(data_to_send));
    
    // Simulação de transmissão e recepção
    uint8_t tx_buffer[256];
    int tx_index = 0;
    
    // Transmite
    printf("\nTransmitindo...\n");
    uint8_t tx_byte;
    bool tx_done;
    do {
        tx_done = protocol_tx_byte(&proto, &tx_byte);
        tx_buffer[tx_index++] = tx_byte;
        printf("TX: 0x%02X\n", tx_byte);
    } while (!tx_done);
    
    // Recebe
    printf("\nRecebendo...\n");
    protocol_init(&proto);
    bool rx_done = false;
    for (int i = 0; i < tx_index; i++) {
        rx_done = protocol_rx_byte(&proto, tx_buffer[i]);
        printf("RX: 0x%02X - Estado: %d\n", tx_buffer[i], proto.rx_state);
    }
    
    printf("\nPacote recebido com %s\n", rx_done ? "sucesso" : "erro");
    if (rx_done) {
        printf("Dados recebidos: ");
        for (int i = 0; i < proto.rx_received_bytes; i++) {
            printf("0x%02X ", proto.rx_data[i]);
        }
        printf("\n");
    }
    
    return 0;
}
