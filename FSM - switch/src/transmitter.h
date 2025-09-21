#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <stdlib.h>
#include <stdbool.h>
#include "protocol.h"

enum TransmitterState {
    WAITING_MESSAGE,
    SEND_STX,
    SEND_QUANTITY,
    SEND_DATA,
    SEND_CHK,
    SEND_ETX,
    SUCCESS
};

void transmitterInit();
bool transmitterFiniteStateMachine();
void transmitterReset();
void transmitterChangeState(enum TransmitterState new_state);
bool transmitterSetMessage(unsigned char* data, int size);
bool transmitterHasMessage();
bool sendMessage(unsigned char* data, int size);
enum TransmitterState transmitterGetState();

#endif // TRANSMITTER_H