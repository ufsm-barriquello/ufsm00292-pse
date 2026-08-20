#include "protocolo.h"
#include <stdio.h>
struct resultado_testes {
    char* mensagem;
    int testes_realizados;
};
//Usado para guardar pointeiros das funções de teste
typedef char* (*Test)();



struct resultado_testes executar_testes() {
    struct resultado_testes resultado = {0, 0};
    Test a_testar[] = {};
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