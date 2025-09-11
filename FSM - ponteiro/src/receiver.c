#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "receiver.h"

#define COMM_FILE "/tmp/uart_comm.dat"
#define MAX_STATES 7

typedef bool (*ReceiverAction)(unsigned char byte);

typedef struct {
    enum ReceiverState state;
    unsigned char* received_data;
    int expected_size;
    int received_index;
    unsigned char received_checksum;
    unsigned char calculated_checksum;
    bool message_complete;
    bool error_occurred;
    ReceiverAction action[MAX_STATES];
} ReceiverStateMachine;

static ReceiverStateMachine sm = {0};


bool readByte(unsigned char* byte) {
    FILE* file = fopen(COMM_FILE, "rb");
    if(!file) return false;

    size_t read_bytes = fread(byte, 1, 1, file);
    fclose(file);

    if(read_bytes == 1) {
        FILE* clear_file = fopen(COMM_FILE, "wb");
        if(clear_file) fclose(clear_file);
        return true;
    }

    return false;
}

static bool stateReceiveStx(unsigned char byte) {
    if(byte == STX) {
        sm.state = RECEIVE_QUANTITY;
        sm.error_occurred = false;
        sm.message_complete = false;
        sm.calculated_checksum = 0;
        return true;
    }
    sm.state = RECEIVE_ERROR;
    sm.error_occurred = true;
    return false;
}

static bool stateReceiveQuantity(unsigned char byte) {
    sm.expected_size = byte;
    sm.received_index = 0;

    if(sm.expected_size > 0) {
        if(sm.received_data != NULL) {
            free(sm.received_data);
        }
        sm.received_data = (unsigned char*)malloc((size_t)sm.expected_size);
        if(sm.received_data == NULL) {
            sm.state = RECEIVE_ERROR;
            sm.error_occurred = true;
            return false;
        }
        sm.state = RECEIVE_DATA;
    } else {
        sm.state = RECEIVE_CHK;
    }
    return true;
}

static bool stateReceiveData(unsigned char byte) {
    if(sm.received_index < sm.expected_size) {
        sm.received_data[sm.received_index] = byte;
        sm.calculated_checksum ^= byte;
        sm.received_index++;

        if(sm.received_index >= sm.expected_size) {
            sm.state = RECEIVE_CHK;
        }
    }
    return true;
}

static bool stateReceiveChk(unsigned char byte) {
    sm.received_checksum = byte;

    if(sm.received_checksum == sm.calculated_checksum) {
        sm.state = RECEIVE_ETX;
        return true;
    }
    sm.state = RECEIVE_ERROR;
    sm.error_occurred = true;
    return false;
}

static bool stateReceiveEtx(unsigned char byte) {
    if(byte == ETX) {
        sm.state = RECEIVE_SUCCESS;
        sm.message_complete = true;
        return true;
    }
    sm.state = RECEIVE_ERROR;
    sm.error_occurred = true;
    return false;
}

static bool stateReceiveSuccess(unsigned char byte) {
    (void)byte;
    return false;
}

static bool stateReceiveError(unsigned char byte) {
    (void)byte;
    return false;
}

void receiverInit() {
    sm.state = RECEIVE_STX;
    sm.received_data = NULL;
    sm.expected_size = 0;
    sm.received_index = 0;
    sm.received_checksum = 0;
    sm.calculated_checksum = 0;
    sm.message_complete = false;
    sm.error_occurred = false;

    sm.action[RECEIVE_STX]     = stateReceiveStx;
    sm.action[RECEIVE_QUANTITY]= stateReceiveQuantity;
    sm.action[RECEIVE_DATA]    = stateReceiveData;
    sm.action[RECEIVE_CHK]     = stateReceiveChk;
    sm.action[RECEIVE_ETX]     = stateReceiveEtx;
    sm.action[RECEIVE_SUCCESS] = stateReceiveSuccess;
    sm.action[RECEIVE_ERROR]   = stateReceiveError;
}

void receiverReset() {
    if(sm.received_data != NULL) {
        free(sm.received_data);
        sm.received_data = NULL;
    }
    receiverInit();
}

bool receiverFiniteStateMachine() {
    unsigned char input_byte;
    if(!readByte(&input_byte)) {
        return false;
    }
    return sm.action[sm.state](input_byte);
}

bool receiveByte() {
    return receiverFiniteStateMachine();
}

bool receiveMessage(unsigned char** output_data, int* output_size) {
    receiverReset();

    while(true) {
        bool cont = receiverFiniteStateMachine();

        if(!cont) {
            if(!sm.message_complete) {
                return false;
            }

            *output_size = sm.expected_size;
            if(*output_size <= 0) {
                *output_data = NULL;
                return true;
            }

            *output_data = (unsigned char*)malloc((size_t)*output_size);
            if(*output_data == NULL) return false;

            for(int j = 0; j < *output_size; j++) {
                (*output_data)[j] = sm.received_data[j];
            }
            return true;
        }
    }
}

bool receiverHasCompleteMessage() { return sm.message_complete; }
bool receiverHasError() { return sm.error_occurred; }
int receiverGetState() { return sm.state; }
unsigned char* receiverGetData() { return sm.received_data; }
int receiverGetDataSize() { return sm.expected_size; }
