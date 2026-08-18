#include "circular_buffer.h"
#include <stdlib.h>

// Estrutura interna do buffer circular
struct circular_buffer_t {
    void*   buffer;         // Ponteiro para a área de memória que armazena os dados
    size_t  capacity;       // Capacidade máxima do buffer
    size_t  head;           // Índice do próximo local para escrita
    size_t  tail;           // Índice do próximo local para leitura
    size_t  size;           // Número atual de itens no buffer
    size_t  element_size;   // Tamanho de cada elemento
};

circular_buffer_t* cb_create(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }

    circular_buffer_t* cb = (circular_buffer_t*)malloc(sizeof(circular_buffer_t));
    if (cb == NULL) {
        return NULL;
    }

    // Assumindo armazenamento de bytes/chars (element_size = 1)
    cb->buffer = malloc(capacity * sizeof(char)); 
    if (cb->buffer == NULL) {
        free(cb);
        return NULL;
    }

    cb->capacity = capacity;
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
    cb->element_size = sizeof(char); 

    return cb;
}

void cb_destroy(circular_buffer_t* cb) {
    if (cb == NULL) {
        return;
    }
    free(cb->buffer); // Libera a memória dos dados
    free(cb);         // Libera a memória da estrutura de controle
}

bool cb_is_empty(const circular_buffer_t* cb) {
    if (cb == NULL) {
        return true;
    }
    return cb->size == 0;
}

bool cb_is_full(const circular_buffer_t* cb) {
    if (cb == NULL) {
        return false; 
    }
    return cb->size == cb->capacity;
}

bool cb_put(circular_buffer_t* cb, const void* item) {
    if (cb == NULL || item == NULL) {
        return false;
    }

    if (cb_is_full(cb)) {
        return false; // Buffer cheio, não pode adicionar
    }

    // Copia o item para a posição 'head' (assumindo 1 byte)
    ((char*)cb->buffer)[cb->head] = *(const char*)item;

    cb->head = (cb->head + 1) % cb->capacity; // Avança o head, com wrap-around
    cb->size++;                               // Incrementa o número de itens

    return true;
}

bool cb_get(circular_buffer_t* cb, void* item_out) {
    if (cb == NULL || item_out == NULL) {
        return false;
    }

    if (cb_is_empty(cb)) {
        return false; // Buffer vazio, não pode remover
    }

    // Copia o item da posição 'tail' para item_out (assumindo 1 byte)
    *(char*)item_out = ((char*)cb->buffer)[cb->tail];

    cb->tail = (cb->tail + 1) % cb->capacity; // Avança o tail, com wrap-around
    cb->size--;                               // Decrementa o número de itens

    return true;
}