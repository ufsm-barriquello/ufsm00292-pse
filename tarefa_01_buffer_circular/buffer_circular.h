#ifndef BUFFER_CIRCULAR_H
#define BUFFER_CIRCULAR_H

#include <stdint.h>
#include <stdbool.h>

// Tamanho máximo do buffer estático
#define BUFFER_SIZE 5

typedef struct {
    uint8_t data[BUFFER_SIZE]; // Array estático de armazenamento
    int head;                  // Índice de escrita (entrada de dados)
    int tail;                  // Índice de leitura (saída de dados)
    int count;                 // Contador de elementos no buffer
} BufferCircular;

// Inicializa o buffer circular zerando os ponteiros e contador
void buffer_init(BufferCircular *b);

// Insere um elemento no buffer. Retorna true em caso de sucesso e false se estiver cheio.
bool buffer_push(BufferCircular *b, uint8_t valor);

// Remove o elemento mais antigo do buffer. Retorna true e guarda o valor em *valor se sucesso.
bool buffer_pop(BufferCircular *b, uint8_t *valor);

// Verifica se o buffer está vazio. Um ponteiro NULL é considerado vazio.
bool buffer_is_empty(const BufferCircular *b);

// Verifica se o buffer está cheio. Um ponteiro NULL é considerado cheio (sem espaço).
bool buffer_is_full(const BufferCircular *b);

#endif // BUFFER_CIRCULAR_H
