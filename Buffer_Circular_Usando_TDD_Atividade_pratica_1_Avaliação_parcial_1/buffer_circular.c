#include "buffer_circular.h"

void buffer_init(BufferCircular *b) {
    if (b == NULL) return;
    b->head = 0;
    b->tail = 0;
    b->count = 0;
}

bool buffer_is_empty(const BufferCircular *b) {
    if (b == NULL) return true;
    return (b->count == 0);
}

bool buffer_is_full(const BufferCircular *b) {
    if (b == NULL) return false;
    return (b->count >= BUFFER_CAPACIDADE);
}

size_t buffer_size(const BufferCircular *b) {
    if (b == NULL) return 0;
    return b->count;
}

bool buffer_push(BufferCircular *b, int valor) {
    if (b == NULL || buffer_is_full(b)) {
        return false;
    }
    b->dados[b->head] = valor;
    b->head = (b->head + 1) % BUFFER_CAPACIDADE;
    b->count++;
    return true;
}

bool buffer_pop(BufferCircular *b, int *valor) {
    if (b == NULL || buffer_is_empty(b) || valor == NULL) {
        return false;
    }
    *valor = b->dados[b->tail];
    b->tail = (b->tail + 1) % BUFFER_CAPACIDADE;
    b->count--;
    return true;
}