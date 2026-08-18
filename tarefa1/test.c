#include "buffer.h"
#include <stdio.h>
struct resultado_testes {
    char* mensagem;
    int testes_realizados;
};
//Usado para guardar pointeiros das funções de teste
typedef char* (*Test)();

//testes do buffer circular
//Criação do buffer
char* testar_criacao(){
    Ring* buffer = criar_buffer(10);
    if (buffer == 0) {
        return "Falha na criação";
    }
    deletar_buffer(&buffer);
    return 0;
}
//Inserção de um dado
char* testar_insert_1(){
    Ring* buffer = criar_buffer(10);
    push_buffer(1, buffer);
    if (buffer->fim != 1) {
        return "Falha na inserção de um dado";
    }
    deletar_buffer(&buffer);
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
    deletar_buffer(&buffer);
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
    deletar_buffer(&buffer);
    return 0;
}
//Testa um insert seguido de um pop
char* testar_acesso(){
    Ring* buffer = criar_buffer(5);
    push_buffer(1, buffer);
    if (pop_buffer(buffer) != 1) {
        return "Falha no pop de um elemento";
    }
    deletar_buffer(&buffer);
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
    deletar_buffer(&buffer);
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
    deletar_buffer(&buffer);
    return 0;
}

//Testa um pop após sobrecarga
char* testar_acesso_4(){
    Ring* buffer = criar_buffer(3);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    push_buffer(3, buffer);
    push_buffer(4, buffer);
    int pop1 = pop_buffer(buffer);
    int pop2 = pop_buffer(buffer);
    if (pop1 != 2 || pop2 != 3) {
        return "Falha no pop sequencial com sobrecapacidade";
    }
    deletar_buffer(&buffer);
    return 0;
}

//Testa um pop após sobrecarga
char* testar_acesso_5(){
    Ring* buffer = criar_buffer(10);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    push_buffer(3, buffer);
    push_buffer(4, buffer);
    int pop1 = pop_buffer(buffer);
    int pop2 = pop_buffer(buffer);
    if (pop1 != 1 || pop2 != 2) {
        return "Falha no pop sequencial";
    }
    deletar_buffer(&buffer);
    return 0;
}

char* testar_peek(){
    Ring* buffer = criar_buffer(5);
    push_buffer(1, buffer);
    if (peek_buffer(buffer) != 1) {
        return "Falha no peek de um elemento";
    }
    deletar_buffer(&buffer);
    return 0;
}
char* testar_peek_2(){
    Ring* buffer = criar_buffer(5);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    if (pop_buffer(buffer) != 1) {
        return "Falha no peek de varios elementos";
    }
    deletar_buffer(&buffer);
    return 0;
}
//Testa um pop após sobrecarga
char* testar_peek_3(){
    Ring* buffer = criar_buffer(3);
    push_buffer(1, buffer);
    push_buffer(2, buffer);
    push_buffer(3, buffer);
    push_buffer(4, buffer);
    if (peek_buffer(buffer) != 2) {
        return "Falha no peek de dados sobrecapacidade";
    }
    deletar_buffer(&buffer);
    return 0;
}


char* testar_delete(){
    Ring* buffer = criar_buffer(10);
    deletar_buffer(&buffer);
    buffer = NULL;
    if (buffer != 0) {
        return "Falha na deleção";
    }
    return 0;
}

struct resultado_testes executar_testes() {
    struct resultado_testes resultado = {0, 0};
    Test a_testar[] = {testar_criacao, 
        testar_insert_1, testar_insert_2, testar_insert_size,
        testar_acesso, testar_acesso_2, testar_acesso_3, testar_acesso_4, testar_acesso_5,
        testar_peek, testar_peek_2, testar_peek_3,
        testar_delete};
    int length = sizeof(a_testar) / sizeof(a_testar[0]);
    for (int i = 0; i < length; i++) {
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