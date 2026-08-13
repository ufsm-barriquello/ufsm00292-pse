#ifndef BUFFER_H
#define BUFFER_H
typedef struct buffer_circular
{
    int inicio;
    int fim;
    int capacidade;
    int* armazenagem;
    
} Ring;

Ring* criar_buffer(int capacidade);
void push_buffer(int elemento, Ring* buffer);
int pop_buffer(Ring* buffer);
int peek_buffer(Ring* buffer);
int tamanho_buffer(Ring* buffer);
void deletar_buffer(Ring* buffer);

#endif