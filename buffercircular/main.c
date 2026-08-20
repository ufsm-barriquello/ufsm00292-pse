#include <stdio.h>
#include <stdbool.h>

#define BUFFER_SIZE 5

// Structure to encapsulate the circular buffer
typedef struct {
    int data[BUFFER_SIZE]; // Static array holding the data
    int head;              // Index where the next item will be written
    int tail;              // Index where the next item will be read
    int count;             // Current number of elements inside the buffer
} CircularBuffer;

// Initialize the buffer state
void cb_init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

// Check if the buffer is full
bool cb_is_full(const CircularBuffer *cb) {
    return cb->count == BUFFER_SIZE;
}

// Check if the buffer is empty
bool cb_is_empty(const CircularBuffer *cb) {
    return cb->count == 0;
}

// Insert an item into the buffer (Enqueue)
bool cb_push(CircularBuffer *cb, int value) {
    if (cb_is_full(cb)) {
        printf("Buffer full! Cannot push %d\n", value);
        return false; // Overflow error
    }

    cb->data[cb->head] = value;
    cb->head = (cb->head + 1) % BUFFER_SIZE; // Circular wrap-around
    cb->count++;
    return true;
}

// Remove and retrieve an item from the buffer (Dequeue)
bool cb_pop(CircularBuffer *cb, int *value) {
    if (cb_is_empty(cb)) {
        printf("Buffer empty! Cannot pop\n");
        return false; // Underflow error
    }

    *value = cb->data[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE; // Circular wrap-around
    cb->count--;
    return true;
}

int main(void) {
    CircularBuffer cb;
    cb_init(&cb);

    for (int i = 1; i < 6; i ++) {
        cb_push(&cb, i);
    }

    //teste 1: tentar push em buffer cheio
    cb_push(&cb, 60);


    int popped_val;
    while (cb_pop(&cb, &popped_val)) {
        printf("Popped: %d\n", popped_val);
    }

    printf("TESTE 2:\n");
    //preenche o buffer de novo
    for (int i = 1; i < 6; i ++) {
        cb_push(&cb, i);
    }
    //teste 2: deleta itens
    cb_pop(&cb, &popped_val);
    printf("T2: Popped: %d\n", popped_val);

    cb_pop(&cb, &popped_val);
    printf("T2: Popped: %d\n", popped_val);

    cb_push(&cb, 60);
    while (cb_pop(&cb, &popped_val)) {
        printf("Final Popped: %d\n", popped_val);
    }


    return 0;
}
