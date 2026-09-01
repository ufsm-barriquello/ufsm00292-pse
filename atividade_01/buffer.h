#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stddef.h>

#define BUFFER_CAPACITY 8

typedef struct {
    int data[BUFFER_CAPACITY];
    size_t head; // Índice de leitura (START)
    size_t tail; // Índice de escrita (END)
    size_t count; // Quantidade atual de elementos
} CircularBuffer;

void buffer_init(CircularBuffer *cb);
bool buffer_is_empty(const CircularBuffer *cb);
bool buffer_is_full(const CircularBuffer *cb);
bool buffer_push(CircularBuffer *cb, int value);
bool buffer_pop(CircularBuffer *cb, int *value);

void buffer_print_debug(const CircularBuffer *cb);

#endif // BUFFER_H