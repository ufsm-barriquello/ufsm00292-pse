#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <stdlib.h>
#include <stdbool.h>
#include "protocol.h"
#include "../pt-1.4/pt.h"

struct transmitter {
    struct pt pt;
    unsigned char* data;
    int data_size;
    int index;
    unsigned char checksum;
    bool complete;
};

void transmitterInit(struct transmitter* t);
PT_THREAD(transmitterThread(struct transmitter* t, unsigned char* data, int size));

#endif // TRANSMITTER_H
