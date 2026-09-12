#include <stdio.h>
#include <assert.h>
#include "buffer.h"

void run_tests() {
    CircularBuffer cb;
    int val;

    printf(">>> INICIANDO TESTES DETALHADOS <<<\n");

    // Teste 1: Inicialização
    buffer_init(&cb);
    buffer_print_debug(&cb); // Mostra estado inicial vazio
    assert(buffer_is_empty(&cb) == true);
    printf("[PASS] Teste 1: Inicializacao\n");

    // Teste 2: Inserção Única
    printf("\n--- Executando Push(10) ---\n");
    assert(buffer_push(&cb, 10) == true);
    buffer_print_debug(&cb); // Mostra dados e Tail avançado
    printf("[PASS] Teste 2: Push simples\n");

    // Teste 3: Remoção Única
    printf("\n--- Executando Pop() ---\n");
    assert(buffer_pop(&cb, &val) == true);
    assert(val == 10);
    buffer_print_debug(&cb); // Mostra Head avançado (voltando a ficar vazio)
    printf("[PASS] Teste 3: Pop simples\n");

    // Teste 4: Demonstrando a Circularidade (Wrap-around)
    printf("\n--- Teste 4: Demonstração de Wrap-around ---\n");
    // Preenche quase tudo (Capacidade 8)
    for (int i = 1; i <= 6; i++) {
        buffer_push(&cb, i * 100);
    }
    printf("Buffer quase cheio (Tail no idx 6):\n");
    buffer_print_debug(&cb);

    // Remove 2 elementos
    buffer_pop(&cb, &val);
    buffer_pop(&cb, &val);
    printf("Apos remover 2 (Head no idx 2, Tail no idx 6):\n");
    buffer_print_debug(&cb);

    // Insere mais 3. Isso forçará o Tail a voltar para o índice 0.
    printf("Inserindo 777, 888, 999 (Forçando Wrap-around do Tail):\n");
    buffer_push(&cb, 777);
    buffer_push(&cb, 888);
    buffer_push(&cb, 999);
    buffer_print_debug(&cb); // Visualmente verifique o EN (Tail) no início

    printf("\nTodos os testes detalhados passaram!\n");
}

int main() {
    run_tests();
    return 0;
}