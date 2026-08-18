#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 5

typedef struct {
    int data[BUFFER_SIZE];
    int head; // índice de escrita
    int tail; // índice de leitura
    int count; // quantidade atual
} CircularBuffer;

void buffer_init(CircularBuffer *cb);
int buffer_is_empty(CircularBuffer *cb);
int buffer_is_full(CircularBuffer *cb);
int buffer_put(CircularBuffer *cb, int value);
int buffer_get(CircularBuffer *cb, int *value);

#endif