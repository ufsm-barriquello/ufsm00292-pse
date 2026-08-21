#include "buffer.h"
#include <stdio.h>

void buffer_init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

bool buffer_is_empty(const CircularBuffer *cb) {
    return cb->count == 0;
}

bool buffer_is_full(const CircularBuffer *cb) {
    return cb->count == BUFFER_CAPACITY;
}

bool buffer_push(CircularBuffer *cb, int value) {
    if (buffer_is_full(cb)) {
        return false;
    }
    cb->data[cb->tail] = value;
    cb->tail = (cb->tail + 1) % BUFFER_CAPACITY;
    cb->count++;
    return true;
}

bool buffer_pop(CircularBuffer *cb, int *value) {
    if (buffer_is_empty(cb)) {
        return false;
    }
    *value = cb->data[cb->head];
    cb->head = (cb->head + 1) % BUFFER_CAPACITY;
    cb->count--;
    return true;
}

#include <stdio.h>
#include "buffer.h"

void buffer_print_debug(const CircularBuffer *cb) {
    printf("\n--- DEBUG BUFFER --- [Count: %u / Cap: %d]\n", (unsigned int)cb->count, BUFFER_CAPACITY);
    
    // Imprime os índices
    printf("Idx: ");
    for (size_t i = 0; i < BUFFER_CAPACITY; i++) {
        printf("[%u] ", (unsigned int)i);
    }
    printf("\n");

    // Imprime os dados
    printf("Val: ");
    for (size_t i = 0; i < BUFFER_CAPACITY; i++) {
        printf(" %d  ", cb->data[i]);
    }
    printf("\n");

    // Imprime ponteiros
    printf("Ptr: ");
    for (size_t i = 0; i < BUFFER_CAPACITY; i++) {
        bool is_head = (i == cb->head);
        bool is_tail = (i == cb->tail);
        
        if (is_head && is_tail) {
            printf(" H&T ");
        } else if (is_head) {
            printf(" ST  ");
        } else if (is_tail) {
            printf(" EN  ");
        } else {
            printf("     ");
        }
    }
    printf("\n--------------------\n\n");
}