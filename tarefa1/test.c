#include "buffer.h"
#include <stdio.h>
struct resultado_testes {
    char* mensagem;
    int testes_realizados;
};

typedef char* (*Test)();

//testes do buffer circular
//Criação do buffer
char* testar_criacao(){
    Ring* buffer = criar_buffer(10);
    if (buffer == 0) {
        return "Falha na criação";
    }
    return 0;
}
//Inserção de um dado
char* testar_insert_1(){
    Ring* buffer = criar_buffer(10);
    push_buffer(1, buffer);
    if (buffer->fim != 1) {
        return "Falha na inserção de um dado";
    }
    return 0;
}
//Testa inserção de dados acima da capacidade
char* testar_insert_2(){
    Ring* buffer = criar_buffer(3);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    push_buffer(3, buffer);
    push_buffer(4, buffer);
    if (tamanho_buffer(buffer) != buffer->capacidade) {
        return "Falha na inserção de dados sobrecapacidade";
    }
    return 0;
}
//Testa tamanho do buffer apos inserção
char* testar_insert_size(){
    Ring* buffer = criar_buffer(10);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    if (tamanho_buffer(buffer) != 2) {
        return "Falha no calculo de tamanho após insert";
    }
    return 0;
}
//Testa um insert seguido de um pop
char* testar_acesso(){
    Ring* buffer = criar_buffer(5);
    push_buffer(1, buffer);
    if (pop_buffer(buffer) != 1) {
        return "Falha no pop de um elemento";
    }
    return 0;
}
//Testa um pop após vários inserts
char* testar_acesso_2(){
    Ring* buffer = criar_buffer(5);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    if (pop_buffer(buffer) != 1) {
        return "Falha no pop de varios elementos";
    }
    return 0;
}
//Testa um pop após sobrecarga
char* testar_acesso_3(){
    Ring* buffer = criar_buffer(3);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    push_buffer(3, buffer);
    push_buffer(4, buffer);
    if (pop_buffer(buffer) != 2) {
        return "Falha no pop de dados sobrecapacidade";
    }
    return 0;
}
char* testar_peek(){
    Ring* buffer = criar_buffer(5);
    push_buffer(1, buffer);
    if (peek_buffer(buffer) != 1) {
        return "Falha no peek de um elemento";
    }
    return 0;
}

struct resultado_testes executar_testes() {
    struct resultado_testes resultado = {0, 0};
    Test a_testar[] = {testar_criacao, testar_insert_1, testar_insert_2, testar_insert_size, testar_acesso, testar_acesso_2, testar_acesso_3, testar_peek};
    for (int i = 0; a_testar[i] != 0; i++) {
        char* valor = a_testar[i]();
        resultado.testes_realizados++;
        if (valor) {
            resultado.mensagem = valor;
            break;
        }
    }
    return resultado;
}

int main() {
    printf("Testando...\n");
    struct resultado_testes resultado = executar_testes();
    printf("Realizados %d testes\n", resultado.testes_realizados);
    if(!resultado.mensagem) {
        printf("Testes completos!\n");
        return 0;
    } else {
        printf("%s\n", resultado.mensagem);
        return 1;
    }

    
}