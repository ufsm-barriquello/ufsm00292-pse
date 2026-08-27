#include "protocolo.h"
#include <string.h>

void TX_Init(TransmitterFSM_t *fsm) {
    fsm->state = TX_IDLE;
}

void TX_StartPacket(TransmitterFSM_t *fsm, uint8_t *data, uint8_t len) {
    fsm->data_len = len;
    fsm->data_index = 0;
    memcpy(fsm->data_to_send, data, len);
    fsm->state = TX_SEND_STX;
    fsm->calculated_chk = 0;
}

bool TX_ProcessByte(TransmitterFSM_t *fsm, uint8_t *byte_out) {
    switch (fsm->state) {
        case TX_IDLE:
            return false;
        case TX_SEND_STX:
            *byte_out = STX;
            fsm->state = TX_SEND_QTD;
            return true;
        case TX_SEND_QTD:
            *byte_out = fsm->data_len;
            fsm->calculated_chk ^= fsm->data_len;
            if (fsm->data_len > 0) fsm->state = TX_SEND_DATA;
            else fsm->state = TX_SEND_CHK;
            return true;
        case TX_SEND_DATA:
            *byte_out = fsm->data_to_send[fsm->data_index];
            fsm->calculated_chk ^= *byte_out;
            fsm->data_index++;
            if (fsm->data_index >= fsm->data_len) fsm->state = TX_SEND_CHK;
            return true;
        case TX_SEND_CHK:
            *byte_out = fsm->calculated_chk;
            fsm->state = TX_SEND_ETX;
            return true;
        case TX_SEND_ETX:
            *byte_out = ETX;
            fsm->state = TX_IDLE;
            return true;
        default:
            fsm->state = TX_IDLE;
            return false;
    }
}