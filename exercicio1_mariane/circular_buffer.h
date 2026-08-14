#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    int *data; // ponteiro pro array de armazenamento
    size_t capacity; // tamanho total do array
    size_t start; // indice do elemento mais antigo
    size_t end; // indice da prox posicao livre p inserir
    size_t count; // numero de elementos armazenados
} CircularBuffer;

bool cb_init(CircularBuffer *cb, int *data, size_t capacity);

void cb_reset(CircularBuffer *cb);

bool cb_push(CircularBuffer *cb, int value);

bool cb_pop(CircularBuffer *cb, int *value);

bool cb_is_empty(const CircularBuffer *cb);
bool cb_is_full(const CircularBuffer *cb);
size_t cb_size(const CircularBuffer *cb);
size_t cb_capacity(const CircularBuffer *cb);

#endif