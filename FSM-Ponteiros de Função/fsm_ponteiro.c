#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_DADOS 256

// ========== ESTRUTURAS E ESTADOS ==========
typedef enum {
    // Estados do Receptor
    RX_WAIT_STX = 0,
    RX_WAIT_QTD,
    RX_WAIT_DADOS,
    RX_WAIT_CHK,
    RX_WAIT_ETX,
    RX_PACKET_COMPLETE,
    RX_ERROR_STATE,
    
    // Estados do Transmissor  
    TX_IDLE,
    TX_SEND_STX,
    TX_SEND_QTD,
    TX_SEND_DADOS,
    TX_SEND_CHK,
    TX_SEND_ETX,
    TX_COMPLETE,
    TX_ERROR_STATE,
    
    NUM_STATES
} State;

typedef struct {
    unsigned char qtd;
    unsigned char dados[MAX_DADOS];
    unsigned char chk;
} Packet;

// ========== VARIÁVEIS GLOBAIS ==========
State rx_state;      // Estado do receptor
State tx_state;      // Estado do transmissor

Packet rx_packet;    // Pacote sendo recebido
Packet tx_packet;    // Pacote a ser transmitido

unsigned char rx_dataIndex;
unsigned char tx_dataIndex;
unsigned char rx_calculated_chk;
unsigned char tx_calculated_chk;

// ========== PROTÓTIPOS ==========
typedef void (*StateFunc)(unsigned char byte);

// Funções do Receptor
void rx_waitSTX(unsigned char byte);
void rx_waitQTD(unsigned char byte);
void rx_waitDADOS(unsigned char byte);
void rx_waitCHK(unsigned char byte);
void rx_waitETX(unsigned char byte);
void rx_packetComplete(unsigned char byte);
void rx_errorState(unsigned char byte);

// Funções do Transmissor
void tx_idle(unsigned char byte);
void tx_sendSTX(unsigned char byte);
void tx_sendQTD(unsigned char byte);
void tx_sendDADOS(unsigned char byte);
void tx_sendCHK(unsigned char byte);
void tx_sendETX(unsigned char byte);
void tx_complete(unsigned char byte);
void tx_errorState(unsigned char byte);

// Funções de obtenção de bytes (substituem o switch-case)
unsigned char tx_getSTX(void);
unsigned char tx_getQTD(void);
unsigned char tx_getDADOS(void);
unsigned char tx_getCHK(void);
unsigned char tx_getETX(void);
unsigned char tx_getIdle(void);
unsigned char tx_getError(void);
unsigned char tx_getComplete(void);

typedef unsigned char (*TxByteFunc)(void);

void processRxByte(unsigned char byte);
void prepareTxPacket(const unsigned char* data, unsigned char size);
unsigned char getTxByte(void);
void advanceTxState(void);
void resetFSM(void);

// ========== FSM ==========
StateFunc rx_fsm[] = { 
    rx_waitSTX, rx_waitQTD, rx_waitDADOS, rx_waitCHK, rx_waitETX, 
    rx_packetComplete, rx_errorState
};

StateFunc tx_fsm[] = {
    tx_idle, tx_sendSTX, tx_sendQTD, tx_sendDADOS, tx_sendCHK, 
    tx_sendETX, tx_complete, tx_errorState
};

// ========== FUNÇÕES DE OBTER BYTES (PONTEIROS) ==========
TxByteFunc tx_byte_funcs[] = {
    tx_getIdle,      // TX_IDLE
    tx_getSTX,       // TX_SEND_STX
    tx_getQTD,       // TX_SEND_QTD
    tx_getDADOS,     // TX_SEND_DADOS
    tx_getCHK,       // TX_SEND_CHK
    tx_getETX,       // TX_SEND_ETX
    tx_getComplete,  // TX_COMPLETE
    tx_getError      // TX_ERROR_STATE
};

// ========== FUNÇÕES AUXILIARES ==========
void resetFSM(void) {
    rx_state = RX_WAIT_STX;
    tx_state = TX_IDLE;
    rx_dataIndex = 0;
    tx_dataIndex = 0;
    memset(&rx_packet, 0, sizeof(Packet));
    memset(&tx_packet, 0, sizeof(Packet));
    rx_calculated_chk = 0;
    tx_calculated_chk = 0;
}

// ========== RECEPTOR ==========
void rx_waitSTX(unsigned char byte) {
    if(byte == 0x02) {
        rx_calculated_chk = byte;
        rx_state = RX_WAIT_QTD;
    }
}

void rx_waitQTD(unsigned char byte) {
    rx_packet.qtd = byte;
    rx_calculated_chk ^= byte;
    
    if(rx_packet.qtd <= MAX_DADOS) {
        if(rx_packet.qtd == 0) {
            rx_state = RX_WAIT_CHK;
        } else {
            rx_state = RX_WAIT_DADOS;
        }
    } else {
        rx_state = RX_ERROR_STATE;
    }
}

void rx_waitDADOS(unsigned char byte) {
    if(rx_dataIndex < MAX_DADOS) {
        rx_packet.dados[rx_dataIndex++] = byte;
        rx_calculated_chk ^= byte;
        
        if(rx_dataIndex >= rx_packet.qtd) {
            rx_state = RX_WAIT_CHK;
        }
    } else {
        rx_state = RX_ERROR_STATE;
    }
}

void rx_waitCHK(unsigned char byte) {
    rx_packet.chk = byte;
    if(rx_calculated_chk == byte) {
        rx_state = RX_WAIT_ETX;
    } else {
        rx_state = RX_ERROR_STATE;
    }
}

void rx_waitETX(unsigned char byte) {
    if(byte == 0x03) {
        rx_state = RX_PACKET_COMPLETE;
    } else {
        rx_state = RX_ERROR_STATE;
    }
}

void rx_packetComplete(unsigned char byte) {
    // Permanece neste estado
}

void rx_errorState(unsigned char byte) {
    // Permanece em estado de erro
}

void processRxByte(unsigned char byte) {
    if(rx_state < NUM_STATES) {
        rx_fsm[rx_state](byte);
    }
}

// ========== TRANSMISSOR ==========
void prepareTxPacket(const unsigned char* data, unsigned char size) {
    if(size <= MAX_DADOS) {
        tx_packet.qtd = size;
        memcpy(tx_packet.dados, data, size);
        
        // Calcula checksum: STX + QTD + DADOS
        tx_calculated_chk = 0x02; // STX
        tx_calculated_chk ^= size; // QTD
        for(int i = 0; i < size; i++) {
            tx_calculated_chk ^= data[i]; // DADOS
        }
        tx_packet.chk = tx_calculated_chk;
        
        tx_state = TX_SEND_STX;
        tx_dataIndex = 0;
    } else {
        tx_state = TX_ERROR_STATE;
    }
}

void tx_idle(unsigned char byte) {
    // Aguarda comando para iniciar transmissão
}

void tx_sendSTX(unsigned char byte) {
    tx_state = TX_SEND_QTD;
}

void tx_sendQTD(unsigned char byte) {
    tx_state = TX_SEND_DADOS;
}

void tx_sendDADOS(unsigned char byte) {
    tx_dataIndex++;
    if(tx_dataIndex >= tx_packet.qtd) {
        tx_state = TX_SEND_CHK;
    }
}

void tx_sendCHK(unsigned char byte) {
    tx_state = TX_SEND_ETX;
}

void tx_sendETX(unsigned char byte) {
    tx_state = TX_COMPLETE;
}

void tx_complete(unsigned char byte) {
    // Transmissão completa
}

void tx_errorState(unsigned char byte) {
    // Erro na transmissão
}

// ========== FUNÇÕES DE OBTER BYTES (USANDO PONTEIROS) ==========
unsigned char tx_getIdle(void) { return 0x00; }
unsigned char tx_getSTX(void) { return 0x02; }
unsigned char tx_getQTD(void) { return tx_packet.qtd; }
unsigned char tx_getDADOS(void) { 
    return (tx_dataIndex < tx_packet.qtd) ? tx_packet.dados[tx_dataIndex] : 0x00; 
}
unsigned char tx_getCHK(void) { return tx_packet.chk; }
unsigned char tx_getETX(void) { return 0x03; }
unsigned char tx_getComplete(void) { return 0x00; }
unsigned char tx_getError(void) { return 0x00; }

unsigned char getTxByte(void) {
    if(tx_state >= TX_IDLE && tx_state <= TX_ERROR_STATE) {
        return tx_byte_funcs[tx_state - TX_IDLE]();
    }
    return 0x00;
}

void advanceTxState(void) {
    if(tx_state >= TX_IDLE && tx_state <= TX_ERROR_STATE) {
        tx_fsm[tx_state - TX_IDLE](0);
    }
}

// ========== TESTES TDD ==========
void testReceptor() {
    printf("=== TESTE RECEPTOR ===\n");
    
    // Teste pacote válido
    unsigned char manual_chk = 0x02 ^ 0x03 ^ 0x10 ^ 0x20 ^ 0x30;
    unsigned char stream[] = {0x02, 0x03, 0x10, 0x20, 0x30, manual_chk, 0x03};
    
    resetFSM();
    for(int i = 0; i < sizeof(stream); i++) {
        processRxByte(stream[i]);
    }
    
    assert(rx_state == RX_PACKET_COMPLETE);
    assert(rx_packet.qtd == 3);
    assert(memcmp(rx_packet.dados, (unsigned char[]){0x10, 0x20, 0x30}, 3) == 0);
    printf("Receptor: Teste passou!\n\n");
}

void testTransmissor() {
    printf("=== TESTE TRANSMISSOR ===\n");
    
    unsigned char dados[] = {0x41, 0x42, 0x43};
    prepareTxPacket(dados, 3);
    
    // Simula transmissão byte a byte
    unsigned char tx_bytes[7];
    for(int i = 0; i < 7; i++) {
        tx_bytes[i] = getTxByte();
        advanceTxState();
    }
    
    // Verifica bytes transmitidos
    assert(tx_bytes[0] == 0x02); // STX
    assert(tx_bytes[1] == 0x03); // QTD
    assert(tx_bytes[2] == 0x41); // Dado 1
    assert(tx_bytes[3] == 0x42); // Dado 2  
    assert(tx_bytes[4] == 0x43); // Dado 3
    
    unsigned char expected_chk = 0x02 ^ 0x03 ^ 0x41 ^ 0x42 ^ 0x43;
    assert(tx_bytes[5] == expected_chk); // CHK
    assert(tx_bytes[6] == 0x03); // ETX
    assert(tx_state == TX_COMPLETE);
    
    printf("Transmissor: Teste passou!\n\n");
}

void runAllTests() {
    printf("Iniciando testes TDD...\n\n");
    testReceptor();
    testTransmissor(); 
    printf("✅ Todos os testes passaram!\n");
}

// ========== MAIN ==========
int main() {
    runAllTests();
    return 0;
}