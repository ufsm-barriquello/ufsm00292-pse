#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdlib.h>
#include <stdbool.h>
#include "protocol.h"

enum ReceiverState {
    RECEIVE_STX,
    RECEIVE_QUANTITY,
    RECEIVE_DATA,
    RECEIVE_CHK,
    RECEIVE_ETX,
    RECEIVE_SUCCESS,
    RECEIVE_ERROR
};

bool receiverFiniteStateMachine();
bool readByte(unsigned char* byte);
bool receiveByte();
void receiverReset();
void receiverChangeState(enum ReceiverState new_state);
int receiverGetState();

bool receiverHasCompleteMessage();
bool receiverHasError();
unsigned char* receiverGetData();
int receiverGetDataSize();

bool receiveMessage(unsigned char** output_data, int* output_size);

#endif // RECEIVER_H
