#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stddef.h>  // Para size_t
#include <stdbool.h> // Para bool

// Tipo opaco para o buffer circular (Encapsulamento)
typedef struct circular_buffer_t circular_buffer_t;

// Funções da API 
// Cria um buffer circular com a capacidade especificada (em número de elementos).
circular_buffer_t* cb_create(size_t capacity);

// Libera a memória alocada para o buffer circular.
void cb_destroy(circular_buffer_t* cb);

// Verifica se o buffer circular está vazio.
bool cb_is_empty(const circular_buffer_t* cb);

// Verifica se o buffer circular está cheio.
bool cb_is_full(const circular_buffer_t* cb);

// Adiciona um item ao buffer circular. Retorna true se a operação foi bem-sucedida, false se o buffer estiver cheio.
bool cb_put(circular_buffer_t* cb, const void* item);

// Remove um item do buffer circular. Retorna true se a operação foi bem-sucedida, false se o buffer estiver vazio.
bool cb_get(circular_buffer_t* cb, void* item_out);

#endif // CIRCULAR_BUFFER_H