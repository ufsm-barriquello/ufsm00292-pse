#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "transmitter.h"

#define COMM_FILE "/tmp/uart_comm.dat"

typedef struct {
    unsigned char* data;
    int data_size;
    int message_index;
    unsigned char checksum;
    bool message_ready;
} MessageBuffer;

static enum TransmitterState current_state = WAITING_MESSAGE;
static MessageBuffer message_buffer = {NULL, 0, 0, 0, false};

void transmitByte(unsigned char byte) {
    FILE* file = fopen(COMM_FILE, "wb");
    if(file) {
        fwrite(&byte, 1, 1, file);
        fflush(file);
        fclose(file);
    }
}

void transmitterInit() {
    transmitterReset();
    FILE* file = fopen(COMM_FILE, "wb");
    if(file) {
        fclose(file);
    }
}

bool transmitterSetMessage(unsigned char* data, int size) {
    if (size < 0 || size > MAX_DATA_SIZE) {
        return false;
    }
    
    if (size > 0 && data == NULL) {
        return false;
    }
    
    message_buffer.data = data;
    message_buffer.data_size = size;
    message_buffer.message_index = 0;
    message_buffer.checksum = 0;
    message_buffer.message_ready = true;
    
    return true;
}

bool transmitterHasMessage() {
    return message_buffer.message_ready;
}

enum TransmitterState transmitterGetState() {
    return current_state;
}

void transmitterReset() {
    current_state = WAITING_MESSAGE;
    message_buffer.message_ready = false;
    message_buffer.data = NULL;
    message_buffer.data_size = 0;
    message_buffer.message_index = 0;
    message_buffer.checksum = 0;
}

bool transmitterFiniteStateMachine() {

    switch(current_state) {

        case WAITING_MESSAGE:
            if(message_buffer.message_ready){
                current_state = SEND_STX;
                return true;
            }
            return false;

        case SEND_STX:
            transmitByte(STX);
            current_state = SEND_QUANTITY;
            return true;  
        
        case SEND_QUANTITY:
            transmitByte((unsigned char)message_buffer.data_size);
            if(message_buffer.data_size > 0){
                message_buffer.message_index = 0;
                message_buffer.checksum = 0;
                current_state = SEND_DATA;
            } else {
                message_buffer.checksum = 0;
                current_state = SEND_CHK; 
            }
            return true;
            
        case SEND_DATA:
            if(message_buffer.message_index < message_buffer.data_size) {
                unsigned char byte = message_buffer.data[message_buffer.message_index];
                transmitByte(byte);
                message_buffer.checksum ^= byte;
                message_buffer.message_index++;
                
                if(message_buffer.message_index >= message_buffer.data_size) {
                    current_state = SEND_CHK;
                }
            }
            return true; 

        case SEND_CHK:
            transmitByte(message_buffer.checksum);
            current_state = SEND_ETX;
            return true;
        
        case SEND_ETX:
            transmitByte(ETX);
            current_state = SUCCESS;
            return true;
            
        case SUCCESS:
            transmitterReset();
            return false;

        default:
            transmitterReset();
            return false;
    }
}

void transmitterChangeState(enum TransmitterState new_state) {
    current_state = new_state;
}

bool sendMessage(unsigned char* data, int size) {
    if (!transmitterSetMessage(data, size)) {
        return false;
    }

    while(transmitterFiniteStateMachine()) {
        // Continua executando a máquina de estados até terminar
    }
    
    return true;
}