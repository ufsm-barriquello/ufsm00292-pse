#include "protocolo.h"

void RX_Init(ReceiverFSM_t *fsm) {
    fsm->state = RX_WAIT_STX;
    fsm->data_index = 0;
    fsm->packet_ready = false;
    fsm->calculated_chk = 0;
}

void RX_ProcessByte(ReceiverFSM_t *fsm, uint8_t byte) {
    switch (fsm->state) {
        case RX_WAIT_STX:
            if (byte == STX) {
                fsm->state = RX_WAIT_QTD;
                fsm->packet_ready = false;
                fsm->calculated_chk = 0;
            }
            break;
        case RX_WAIT_QTD:
            fsm->expected_qtd = byte;
            fsm->data_index = 0;
            fsm->calculated_chk ^= byte;
            if (fsm->expected_qtd > 0) fsm->state = RX_WAIT_DATA;
            else fsm->state = RX_WAIT_CHK;
            break;
        case RX_WAIT_DATA:
            fsm->data[fsm->data_index++] = byte;
            fsm->calculated_chk ^= byte;
            if (fsm->data_index >= fsm->expected_qtd) fsm->state = RX_WAIT_CHK;
            break;
        case RX_WAIT_CHK:
            if (byte == fsm->calculated_chk) fsm->state = RX_WAIT_ETX;
            else RX_Init(fsm); // Falha, reinicia
            break;
        case RX_WAIT_ETX:
            if (byte == ETX) {
                fsm->packet_ready = true;
                fsm->state = RX_WAIT_STX;
            } else {
                RX_Init(fsm);
            }
            break;
        default:
            RX_Init(fsm);
            break;
    }
}