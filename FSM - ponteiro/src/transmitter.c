#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "transmitter.h"

#define COMM_FILE "/tmp/uart_comm.dat"
#define MAX_STATES 7

typedef bool (*TransmitterAction)(void);

typedef struct {
    enum TransmitterState state;
    unsigned char* data;
    int data_size;
    int message_index;
    unsigned char checksum;
    bool message_ready;
} TransmitterStateMachine;

static TransmitterStateMachine sm = {WAITING_MESSAGE, NULL, 0, 0, 0, false};

void transmitByte(unsigned char byte) {
    FILE* file = fopen(COMM_FILE, "ab");
    if(file) {
        fwrite(&byte, 1, 1, file);
        fflush(file);
        fclose(file);
    }
}

static bool stateWaitingMessage(void) {
    if(sm.message_ready) {
        sm.state = SEND_STX;
        return true;
    }
    return false;
}

static bool stateSendStx(void) {
    transmitByte(STX);
    sm.state = SEND_QUANTITY;
    return true;
}

static bool stateSendQuantity(void) {
    transmitByte((unsigned char)sm.data_size);
    if(sm.data_size > 0) {
        sm.message_index = 0;
        sm.checksum = 0;
        sm.state = SEND_DATA;
    } else {
        sm.checksum = 0;
        sm.state = SEND_CHK;
    }
    return true;
}

static bool stateSendData(void) {
    if(sm.message_index < sm.data_size) {
        unsigned char byte = sm.data[sm.message_index];
        transmitByte(byte);
        sm.checksum ^= byte;
        sm.message_index++;

        if(sm.message_index >= sm.data_size) {
            sm.state = SEND_CHK;
        }
    }
    return true;
}

static bool stateSendChk(void) {
    transmitByte(sm.checksum);
    sm.state = SEND_ETX;
    return true;
}

static bool stateSendEtx(void) {
    transmitByte(ETX);
    sm.state = SUCCESS;
    return true;
}

static bool stateSuccess(void) {
    transmitterReset();
    return false;
}

static TransmitterAction stateTable[MAX_STATES] = {
    stateWaitingMessage,
    stateSendStx,
    stateSendQuantity,
    stateSendData,
    stateSendChk,
    stateSendEtx,
    stateSuccess
};

void transmitterInit() {
    sm.state = WAITING_MESSAGE;
    sm.data = NULL;
    sm.data_size = 0;
    sm.message_index = 0;
    sm.checksum = 0;
    sm.message_ready = false;

    FILE* file = fopen(COMM_FILE, "wb");
    if(file) fclose(file);
}

void transmitterReset() {
    sm.state = WAITING_MESSAGE;
    sm.message_ready = false;
    sm.data = NULL;
    sm.data_size = 0;
    sm.message_index = 0;
    sm.checksum = 0;
}

bool transmitterSetMessage(unsigned char* data, int size) {
    if(size < 0 || size > MAX_DATA_SIZE) return false;
    if(size > 0 && data == NULL) return false;

    sm.data = data;
    sm.data_size = size;
    sm.message_index = 0;
    sm.checksum = 0;
    sm.message_ready = true;

    return true;
}

bool transmitterHasMessage() { return sm.message_ready; }
enum TransmitterState transmitterGetState() { return sm.state; }

bool transmitterFiniteStateMachine() {
    return stateTable[sm.state]();
}

bool sendMessage(unsigned char* data, int size) {
    if(!transmitterSetMessage(data, size)) return false;

    while(transmitterFiniteStateMachine()) {
    }
    return true;
}
