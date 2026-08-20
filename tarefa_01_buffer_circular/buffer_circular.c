#include "buffer_circular.h"
#include <stddef.h>

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
    // Um buffer inexistente nunca tem espaco para escrever -> tratado como cheio.
    // Junto com buffer_is_empty(NULL) == true, garante que nenhuma operacao
    // tenha sucesso sobre um ponteiro invalido.
    if (b == NULL) return true;
    return (b->count == BUFFER_SIZE);
}

bool buffer_push(BufferCircular *b, uint8_t valor) {
    if (b == NULL || buffer_is_full(b)) {
        return false;
    }
    
    b->data[b->head] = valor;
    b->head = (b->head + 1) % BUFFER_SIZE; // Aritmética modular para circularidade
    b->count++;
    
    return true;
}

bool buffer_pop(BufferCircular *b, uint8_t *valor) {
    if (b == NULL || valor == NULL || buffer_is_empty(b)) {
        return false;
    }
    
    *valor = b->data[b->tail];
    b->tail = (b->tail + 1) % BUFFER_SIZE; // Aritmética modular para circularidade
    b->count--;
    
    return true;
}
