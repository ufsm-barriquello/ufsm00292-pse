#ifndef BUFFER_CIRCLE_H
#define BUFFER_CIRCLE_H

typedef struct {
    int *data;      
    int head;       
    int tail;       
    int size;      
    int full;      
} BufferCircle;

BufferCircle* bufferCircle_create(int size);
void bufferCircle_destroy(BufferCircle *bufferCircle);
void bufferCircle_reset(BufferCircle *bufferCircle);
int bufferCircle_is_empty(BufferCircle *bufferCircle);
int bufferCircle_is_full(BufferCircle *bufferCircle);
int bufferCircle_enqueue(BufferCircle *bufferCircle, int value);
int bufferCircle_dequeue(BufferCircle *bufferCircle, int *value);

#endif
