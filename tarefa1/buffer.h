#ifndef BUFFER_H
#define BUFFER_H
typedef struct buffer_circular
{
    int inicio;
    int fim;
    int capacidade;
    int tamanho; //armazenado aqui para evitar ambiguidade no calculo para determinar se está vazio ou cheio
    int* armazenagem;
    
} Ring;

Ring* criar_buffer(int capacidade);
void push_buffer(int elemento, Ring* buffer);
int pop_buffer(Ring* buffer);
int peek_buffer(Ring* buffer);
int tamanho_buffer(Ring* buffer);
void deletar_buffer(Ring** buffer);

#endif