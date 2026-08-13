#include "buffer.h"
#include <stdlib.h>

Ring* criar_buffer(int capacidade) {
    Ring* to_ret = malloc(sizeof(Ring));
    to_ret->inicio = 0;
    to_ret->fim = 0;
    to_ret->capacidade = capacidade;
    to_ret->armazenagem = malloc(sizeof(int) * capacidade);
    return to_ret;
}
void push_buffer(int elemento, Ring* buffer){

}
int pop_buffer(Ring* buffer) {

}
int peek_buffer(Ring* buffer) {

}
int tamanho_buffer(Ring* buffer){

}
void deletar_buffer(Ring* buffer){
    
}