#include "buffer.h"
#include <stdlib.h>
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
//cria ponteiro para o buffer
Ring* criar_buffer(int capacidade) {
    Ring* to_ret = malloc(sizeof(Ring));
    to_ret->inicio = 0;
    to_ret->fim = 0;
    to_ret->tamanho = 0;
    to_ret->capacidade = capacidade;
    to_ret->armazenagem = malloc(sizeof(int) * capacidade);
    return to_ret;
}
//insere elemento no fim
void push_buffer(int elemento, Ring* buffer){
    buffer->armazenagem[buffer->fim] = elemento;
    buffer->fim = (buffer->fim + 1) % buffer->capacidade;
    if(tamanho_buffer(buffer) == buffer->capacidade) {
        buffer->inicio = (buffer->inicio + 1) % buffer->capacidade;
    } else {
        buffer->tamanho++;
    }
}
//Remove o elemento no inicio
int pop_buffer(Ring* buffer) {
    if(tamanho_buffer(buffer) == 0) {
        return 0;
    }
    int valor = buffer->armazenagem[buffer->inicio];
    buffer->inicio = (buffer->inicio + 1) % buffer->capacidade;
    buffer->tamanho = MAX(0, buffer->tamanho-1);
    return valor;
}
//Permite ver o item no topo sem remove-lo
int peek_buffer(Ring* buffer) {
    if(tamanho_buffer(buffer) == 0) {
        return 0;
    }
    return buffer->armazenagem[buffer->inicio];
}
//Evita ambiguidade quando computando o valor quando inicio == fim, que pode ser vazio ou cheio.
int tamanho_buffer(Ring* buffer){
    return buffer->tamanho;
}
//Também requer que o ponteiro do buffer seja limpo após uso
void deletar_buffer_conteudo(Ring *buffer){
    free(buffer->armazenagem);
}

void deletar_buffer(Ring **bufferPtr){
    deletar_buffer_conteudo(*bufferPtr);
    free(*bufferPtr);
}