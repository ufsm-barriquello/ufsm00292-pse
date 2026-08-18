#include "buffer.h"

void buffer_init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

int buffer_is_empty(CircularBuffer *cb) {
    return (cb->count == 0);
}

int buffer_is_full(CircularBuffer *cb) {
    return (cb->count == BUFFER_SIZE);
}

// Retorna 1 se sucesso, 0 se falha (overflow)
int buffer_put(CircularBuffer *cb, int value) {
    if (buffer_is_full(cb)) {
        return 0; 
    }
    cb->data[cb->head] = value;
    cb->head = (cb->head + 1) % BUFFER_SIZE; // wrap-around
    cb->count++;
    return 1;
}

// Retorna 1 se sucesso, 0 se falha (underflow)
int buffer_get(CircularBuffer *cb, int *value) {
    if (buffer_is_empty(cb)) {
        return 0; 
    }
    *value = cb->data[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE; // wrap-around
    cb->count--;
    return 1;
}