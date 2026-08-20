#ifndef PROTOCOL_H
#include <stdint.h>
#define PROTOCOL_H

typedef enum {
    STATE_START,
    STATE_QTD,
    STATE_DATA,
    STATE_CHKSUM,
    STATE_END,
} Estado;

typedef struct {
    uint8_t n;
    uint8_t qtd;
    uint8_t chksum;
} Rundata;

void rundata_reset(Rundata r);
Estado process_byte(uint8_t entrada, Estado atual, Rundata armazenamento);


#endif