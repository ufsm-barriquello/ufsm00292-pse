#include "circular_buffer.h"

bool cb_init(CircularBuffer *cb, int *data, size_t capacity) {
    if (cb == NULL || data == NULL || capacity == 0) {
        return false;
    }
    cb->data = data;
    cb->capacity = capacity;
    cb_reset(cb);
    return true;
}

void cb_reset(CircularBuffer *cb) {
    cb->start = 0;
    cb->end = 0;
    cb->count = 0;
}

bool cb_push(CircularBuffer *cb, int value) {
    if (cb_is_full(cb)) {
        return false;
    }
    cb->data[cb->end] = value;
    cb->end = (cb->end + 1) % cb->capacity;
    cb->count++;
    return true;
}

bool cb_pop(CircularBuffer *cb, int *value) {
    if (cb_is_empty(cb)) {
        return false;
    }
    *value = cb->data[cb->start];
    cb->start = (cb->start + 1) % cb->capacity;
    cb->count--;
    return true;
}

bool cb_is_empty(const CircularBuffer *cb) {
    return cb->count == 0;
}

bool cb_is_full(const CircularBuffer *cb) {
    return cb->count == cb->capacity;
}

size_t cb_size(const CircularBuffer *cb) {
    return cb->count;
}

size_t cb_capacity(const CircularBuffer *cb) {
    return cb->capacity;
}