#ifndef BUFFER_CIRCULAR_H
#define BUFFER_CIRCULAR_H

#include <stdbool.h>
#include <stddef.h>

#define BUFFER_CAPACIDADE 8

typedef struct {
    int dados[BUFFER_CAPACIDADE];
    size_t head;   /* Índice de escrita */
    size_t tail;   /* Índice de leitura */
    size_t count;  /* Quantidade de elementos armazenados */
} BufferCircular;

void buffer_init(BufferCircular *b);
bool buffer_push(BufferCircular *b, int valor);
bool buffer_pop(BufferCircular *b, int *valor);
bool buffer_is_empty(const BufferCircular *b);
bool buffer_is_full(const BufferCircular *b);
size_t buffer_size(const BufferCircular *b);

#endif /* BUFFER_CIRCULAR_H */