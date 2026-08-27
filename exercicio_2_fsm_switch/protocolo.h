#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03
#define MAX_DATA_LEN 255

// --- RECEPTOR (RX) ---
typedef enum {
    RX_WAIT_STX,
    RX_WAIT_QTD,
    RX_WAIT_DATA,
    RX_WAIT_CHK,
    RX_WAIT_ETX
} RxState_t;

typedef struct {
    RxState_t state;
    uint8_t expected_qtd;
    uint8_t data[MAX_DATA_LEN];
    uint8_t data_index;
    uint8_t calculated_chk;
    bool packet_ready;
} ReceiverFSM_t;

void RX_Init(ReceiverFSM_t *fsm);
void RX_ProcessByte(ReceiverFSM_t *fsm, uint8_t byte);

// --- TRANSMISSOR (TX) ---
typedef enum {
    TX_IDLE,
    TX_SEND_STX,
    TX_SEND_QTD,
    TX_SEND_DATA,
    TX_SEND_CHK,
    TX_SEND_ETX
} TxState_t;

typedef struct {
    TxState_t state;
    uint8_t data_to_send[MAX_DATA_LEN];
    uint8_t data_len;
    uint8_t data_index;
    uint8_t calculated_chk;
} TransmitterFSM_t;

void TX_Init(TransmitterFSM_t *fsm);
void TX_StartPacket(TransmitterFSM_t *fsm, uint8_t *data, uint8_t len);
bool TX_ProcessByte(TransmitterFSM_t *fsm, uint8_t *byte_out);

#endif // PROTOCOLO_H