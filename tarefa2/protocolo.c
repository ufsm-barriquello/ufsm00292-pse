#include "protocolo.h"

void rundata_reset(Rundata r) {
    r.chksum = 0;
    r.n = 0;
    r.qtd = 0;
}

void setup() {

}

void loop() {

}

Estado process_byte(uint8_t entrada, Estado atual, Rundata armazenamento){

}
