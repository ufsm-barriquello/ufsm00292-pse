#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "circular_buffer.h"

// Funções Auxiliares de Teste 
void test_suite_start(const char* name) {
    printf("--- Iniciando Suite de Testes: %s ---\n", name);
}

void test_suite_end(const char* name) {
    printf("--- Suite de Testes Concluida: %s ---\n\n", name);
}

void test_case_start(const char* name) {
    printf("  Executando Teste: %s\n", name);
}

void test_case_end(const char* name) {
    printf("  Teste Concluido: %s [OK]\n", name);
}

// Testes 

void test_creation_and_initial_state(void) {
    test_case_start("Criacao e Estado Inicial");
    circular_buffer_t* cb = cb_create(5);
    assert(cb != NULL); 
    assert(cb_is_empty(cb) == true); 
    cb_destroy(cb);
    test_case_end("Criacao e Estado Inicial");
}

void test_put_and_is_full(void) {
    test_case_start("Insercao Simples e Buffer Cheio");
    circular_buffer_t* cb = cb_create(3); 
    assert(cb != NULL);
    assert(cb_is_empty(cb) == true);
    assert(cb_is_full(cb) == false);

    char item1 = 'A';
    assert(cb_put(cb, &item1) == true);
    assert(cb_is_empty(cb) == false);
    assert(cb_is_full(cb) == false);

    char item2 = 'B';
    assert(cb_put(cb, &item2) == true);
    assert(cb_is_empty(cb) == false);
    assert(cb_is_full(cb) == false);

    char item3 = 'C';
    assert(cb_put(cb, &item3) == true);
    assert(cb_is_empty(cb) == false);
    assert(cb_is_full(cb) == true); // Agora deve estar cheio

    // Tentar adicionar a um buffer cheio deve falhar
    char item4 = 'D';
    assert(cb_put(cb, &item4) == false);
    assert(cb_is_full(cb) == true); // Ainda deve estar cheio

    cb_destroy(cb);
    test_case_end("Insercao Simples e Buffer Cheio");
}

void test_get_simple_and_underflow(void) {
    test_case_start("Remocao Simples e Underflow");
    circular_buffer_t* cb = cb_create(3);
    assert(cb != NULL);

    char received_item;
    // Teste de Underflow
    assert(cb_get(cb, &received_item) == false);
    assert(cb_is_empty(cb) == true);

    // Teste de Remocao Simples
    char item_to_put = 'X';
    assert(cb_put(cb, &item_to_put) == true);
    assert(cb_get(cb, &received_item) == true);
    assert(received_item == 'X'); 
    assert(cb_is_empty(cb) == true); 

    // Teste com multiplos itens 
    char item_a = 'A', item_b = 'B';
    cb_put(cb, &item_a);
    cb_put(cb, &item_b);
    cb_get(cb, &received_item); assert(received_item == 'A');
    cb_get(cb, &received_item); assert(received_item == 'B');
    assert(cb_is_empty(cb) == true);

    cb_destroy(cb);
    test_case_end("Remocao Simples e Underflow");
}

void test_wrap_around(void) {
    test_case_start("Comportamento Circular (Wrap-around)");
    circular_buffer_t* cb = cb_create(3);
    
    char val1 = '1', val2 = '2', val3 = '3', val4 = '4', received;
    
    // Enche o buffer: [1, 2, 3]
    cb_put(cb, &val1);
    cb_put(cb, &val2);
    cb_put(cb, &val3);
    
    // Remove o primeiro (abre espaço no começo do vetor interno)
    cb_get(cb, &received);
    assert(received == '1'); 
    
    // Insere um novo elemento. O ponteiro deve dar a volta (wrap-around)
    assert(cb_put(cb, &val4) == true); 
    assert(cb_is_full(cb) == true);
    
    // Remove o resto garantindo a ordem FIFO (First-In, First-Out)
    cb_get(cb, &received); assert(received == '2');
    cb_get(cb, &received); assert(received == '3');
    cb_get(cb, &received); assert(received == '4'); 
    
    cb_destroy(cb);
    test_case_end("Comportamento Circular (Wrap-around)");
}

// Função Principal de Teste
int main() {
    test_suite_start("Buffer Circular");

    test_creation_and_initial_state();
    test_put_and_is_full();
    test_get_simple_and_underflow();
    test_wrap_around(); // O novo teste essencial!

    test_suite_end("Buffer Circular");
    return 0;
}