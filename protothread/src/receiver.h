#ifndef RECEIVER_H
#define RECEIVER_H

#include "../pt-1.4/pt.h"
#include <stdbool.h>
#include <stdio.h>

#define STX 0x02
#define ETX 0x03

struct receiver {
    struct pt pt;
    unsigned char* data;
    int expected_size;
    int received_index;
    unsigned char received_checksum;
    unsigned char calculated_checksum;
    bool complete;
    bool error;
    int read_offset;
    FILE* file;
};

void receiverReset(struct receiver* r);
PT_THREAD(receiverThread(struct receiver* r));

#endif
