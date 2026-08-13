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
    return "Criação: não implementado";
}
//Inserção de um dado
char* testar_insert_1(){
    return "Insert: não implementado";
}
//Testa inserção de dados acima da capacidade
char* testar_insert_2(){
    return "Insert: não implementado";
}
//Testa tamanho do buffer apos inserção
char* testar_insert_size(){
    return "Insert: não implementado";
}
//Testa um insert seguido de um pop
char* testar_acesso(){
    return "Pop: não implementado";
}
//Testa um pop após vários inserts
char* testar_acesso_2(){
    return "Pop: não implementado";
}
//Testa um pop após sobrecarga
char* testar_acesso_3(){
    return "Pop: não implementado";
}

struct resultado_testes executar_testes() {
    struct resultado_testes resultado = {0, 0};
    Test a_testar[] = {testar_criacao, testar_insert_1, testar_insert_2, testar_insert_size, testar_acesso, testar_acesso_2, testar_acesso_3};
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