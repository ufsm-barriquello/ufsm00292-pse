#include <stdio.h>
#include <stdlib.h>
#include "../include/bufferCircle.h"

BufferCircle* bufferCircle_create(int size) {
    if (size <= 0) {
        return NULL;
    }

    BufferCircle *bufferCircle = (BufferCircle*)malloc(sizeof(BufferCircle));
    if (!bufferCircle) {
        return NULL;
    }

    bufferCircle->data = (int*)malloc(sizeof(int) * size);
    if (!bufferCircle->data) {
        free(bufferCircle);
        return NULL;
    }

    bufferCircle->size = size;
    bufferCircle->head = 0;
    bufferCircle->tail = 0;
    bufferCircle->full = 0;

    return bufferCircle;
}

void bufferCircle_destroy(BufferCircle *bufferCircle) {
    if (bufferCircle) {
        if (bufferCircle->data) {
            free(bufferCircle->data);
        }
        free(bufferCircle);
    }
}

void bufferCircle_reset(BufferCircle *bufferCircle) {
    if (bufferCircle) {
        bufferCircle->head = 0;
        bufferCircle->tail = 0;
        bufferCircle->full = 0;
    }
}

int bufferCircle_is_empty(BufferCircle *bufferCircle) {
    return (!bufferCircle->full && (bufferCircle->head == bufferCircle->tail));
}

int bufferCircle_is_full(BufferCircle *bufferCircle) {
    return bufferCircle->full;
}

int bufferCircle_enqueue(BufferCircle *bufferCircle, int value) {
    if (!bufferCircle || !bufferCircle->data) {
        return -1;
    }

    if (bufferCircle->full) {
        return -1; // Buffer cheio
    }

    bufferCircle->data[bufferCircle->tail] = value;
    bufferCircle->tail = (bufferCircle->tail + 1) % bufferCircle->size;

    if (bufferCircle->tail == bufferCircle->head) {
        bufferCircle->full = 1;
    }

    return 0;
}

int bufferCircle_dequeue(BufferCircle *bufferCircle, int *value) {
    if (!bufferCircle || !bufferCircle->data || !value) {
        return -1;
    }

    if (bufferCircle_is_empty(bufferCircle)) {
        return -1; // Buffer vazio
    }

    *value = bufferCircle->data[bufferCircle->head];
    bufferCircle->head = (bufferCircle->head + 1) % bufferCircle->size;
    bufferCircle->full = 0;

    return 0;
}
