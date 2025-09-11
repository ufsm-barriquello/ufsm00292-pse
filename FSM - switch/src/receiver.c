#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "receiver.h"

#define COMM_FILE "/tmp/uart_comm.dat"

typedef struct {
    unsigned char* received_data;
    int expected_size;
    int received_index;
    unsigned char received_checksum;
    unsigned char calculated_checksum;
    bool message_complete;
    bool error_occurred;
} ReceiverBuffer;

static enum ReceiverState current_state = RECEIVE_STX;
static ReceiverBuffer receiver_buffer = {NULL, 0, 0, 0, 0, false, false};

bool readByte(unsigned char* byte) {
    FILE* file = fopen(COMM_FILE, "rb");
    if(!file) return false;
    
    fseek(file, 0, SEEK_SET);
    
    size_t read_bytes = fread(byte, 1, 1, file);
    fclose(file);
    
    if(read_bytes == 1) {
        FILE* clear_file = fopen(COMM_FILE, "wb");
        if(clear_file) {
            fclose(clear_file);
        }
        return true;
    }
    
    return false;
}

void receiverReset();

bool receiverFiniteStateMachine() {
    unsigned char input_byte;
    
    if(!readByte(&input_byte)) {
        return false;
    }
    
    switch(current_state) {
        
        case RECEIVE_STX:
            if(input_byte == STX) {
                current_state = RECEIVE_QUANTITY;
                receiver_buffer.error_occurred = false;
                receiver_buffer.message_complete = false;
                receiver_buffer.calculated_checksum = 0;
                return true;
            } else {
                current_state = RECEIVE_ERROR;
                receiver_buffer.error_occurred = true;
                return false;
            }
            
        case RECEIVE_QUANTITY:
            receiver_buffer.expected_size = input_byte;
            receiver_buffer.received_index = 0;
            
            if(receiver_buffer.expected_size > 0) {
                if(receiver_buffer.received_data != NULL) {
                    free(receiver_buffer.received_data);
                }
                receiver_buffer.received_data = (unsigned char*)malloc((size_t)receiver_buffer.expected_size);
                if(receiver_buffer.received_data == NULL) {
                    current_state = RECEIVE_ERROR;
                    receiver_buffer.error_occurred = true;
                    return false;
                }
                current_state = RECEIVE_DATA;
            } else {
                current_state = RECEIVE_CHK;
            }
            return true;
            
        case RECEIVE_DATA:
            if(receiver_buffer.received_index < receiver_buffer.expected_size) {
                receiver_buffer.received_data[receiver_buffer.received_index] = input_byte;
                receiver_buffer.calculated_checksum ^= input_byte;
                receiver_buffer.received_index++;
                
                if(receiver_buffer.received_index >= receiver_buffer.expected_size) {
                    current_state = RECEIVE_CHK;
                }
            }
            return true;
            
        case RECEIVE_CHK:
            receiver_buffer.received_checksum = input_byte;
            
            if(receiver_buffer.received_checksum == receiver_buffer.calculated_checksum) {
                current_state = RECEIVE_ETX;
                return true;
            } else {
                current_state = RECEIVE_ERROR;
                receiver_buffer.error_occurred = true;
                return false;
            }
            
        case RECEIVE_ETX:
            if(input_byte == ETX) {
                current_state = RECEIVE_SUCCESS;
                receiver_buffer.message_complete = true;
                return true;
            } else {
                current_state = RECEIVE_ERROR;
                receiver_buffer.error_occurred = true;
                return false;
            }
            
        case RECEIVE_SUCCESS:
            return false; 
            
        default:
            receiverReset();
            return false;
    }
}

void receiverReset() {
    current_state = RECEIVE_STX;
    if(receiver_buffer.received_data != NULL) {
        free(receiver_buffer.received_data);
        receiver_buffer.received_data = NULL;
    }
    receiver_buffer.expected_size = 0;
    receiver_buffer.received_index = 0;
    receiver_buffer.received_checksum = 0;
    receiver_buffer.calculated_checksum = 0;
    receiver_buffer.message_complete = false;
    receiver_buffer.error_occurred = false;
}

void receiverChangeState(enum ReceiverState new_state) {
    current_state = new_state;
}

int receiverGetState() {
    return current_state;
}

bool receiverHasCompleteMessage() {
    return receiver_buffer.message_complete;
}

bool receiverHasError() {
    return receiver_buffer.error_occurred;
}

unsigned char* receiverGetData() {
    return receiver_buffer.received_data;
}

int receiverGetDataSize() {
    return receiver_buffer.expected_size;
}

bool receiveByte() {
    return receiverFiniteStateMachine();
}

bool receiveMessage(unsigned char** output_data, int* output_size) {
    receiverReset();
    
    while(true) {
        bool continue_processing = receiverFiniteStateMachine();
        
        if(!continue_processing) {
            if(!receiverHasCompleteMessage()) {
                return false;
            }
            
            *output_size = receiverGetDataSize();
            
            if(*output_size <= 0) {
                *output_data = NULL;
                return true;
            }
            
            *output_data = (unsigned char*)malloc((size_t)*output_size);
            if(*output_data == NULL) {
                return false;
            }
            
            for(int j = 0; j < *output_size; j++) {
                (*output_data)[j] = receiver_buffer.received_data[j];
            }
            
            return true;
        }
        
        if(!continue_processing && !receiverHasCompleteMessage()) {
            break;
        }
    }
    
    return false;
}
