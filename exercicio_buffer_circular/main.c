#include <stdio.h>
#include "buffer.h"

// Variáveis globais para contagem
int testes_executados = 0;
int testes_passaram = 0;

// Função auxiliar para imprimir os resultados no estilo TDD
void verifica(char* nome_teste, int condicao) {
    testes_executados++;
    if (condicao) {
        printf("[PASS] %s\n", nome_teste);
        testes_passaram++;
    } else {
        printf("[FAIL] %s\n", nome_teste);
    }
}

int main() {
    CircularBuffer cb;
    int valor;

    printf("--- INICIANDO TESTES DO BUFFER CIRCULAR ---\n\n");

    // Teste 1: Inicialização
    buffer_init(&cb);
    verifica("Buffer inicia vazio", buffer_is_empty(&cb) == 1);

    // Teste 2: Inserção (Put)
    buffer_put(&cb, 10);
    verifica("Buffer nao esta vazio apos insercao", buffer_is_empty(&cb) == 0);

    // Teste 3: Remoção (Get)
    buffer_get(&cb, &valor);
    verifica("Valor removido e o correto (10)", valor == 10);
    verifica("Buffer volta a ficar vazio apos remover o unico item", buffer_is_empty(&cb) == 1);

    // Teste 4: Capacidade Máxima
    buffer_init(&cb); // Reseta o buffer
    for(int i = 0; i < BUFFER_SIZE; i++) {
        buffer_put(&cb, i * 10);
    }
    verifica("Buffer reporta que esta cheio", buffer_is_full(&cb) == 1);

    // Teste 5: Overflow
    verifica("Falha ao tentar inserir em buffer cheio (Overflow)", buffer_put(&cb, 99) == 0);

    // Teste 6: Underflow
    buffer_init(&cb); // Reseta o buffer para testar o vazio
    verifica("Falha ao tentar remover de buffer vazio (Underflow)", buffer_get(&cb, &valor) == 0);

    // Resumo final
    printf("\n--- RESULTADO FINAL ---\n");
    printf("Testes executados: %d\n", testes_executados);
    printf("Testes aprovados: %d\n", testes_passaram);

    if (testes_executados == testes_passaram) {
        printf("\nTODOS OS TESTES PASSARAM!\n");
    }

    return 0;
}