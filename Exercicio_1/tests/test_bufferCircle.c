#include <stdio.h>
#include <stdlib.h>
#include "../src/minunit.h"
#include "../include/bufferCircle.h"

int tests_run = 0;

static char * test_create_buffer_valid_size() {
    BufferCircle *buffer = bufferCircle_create(5);
    mu_assert("Erro: O buffer não deveria ser NULL", buffer != NULL);
    mu_assert("Erro: Tamanho do buffer incorreto", buffer->size == 5);
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_create_buffer_zero_size() {
    BufferCircle *buffer = bufferCircle_create(0);
    mu_assert("Erro: O buffer deveria ser NULL para tamanho 0", buffer == NULL);
    return 0;
}

static char * test_enqueue_into_empty_buffer() {
    BufferCircle *buffer = bufferCircle_create(3);
    int result = bufferCircle_enqueue(buffer, 10);
    mu_assert("Erro: Enqueue deveria ter sucesso em buffer vazio", result == 0);
    mu_assert("Erro: O buffer não deveria estar vazio após enqueue", !bufferCircle_is_empty(buffer));
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_enqueue_until_full() {
    BufferCircle *buffer = bufferCircle_create(2);
    bufferCircle_enqueue(buffer, 1);
    bufferCircle_enqueue(buffer, 2);
    mu_assert("Erro: O buffer deveria estar cheio após enfileirar 2 elementos", bufferCircle_is_full(buffer));
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_enqueue_into_full_buffer() {
    BufferCircle *buffer = bufferCircle_create(1);
    bufferCircle_enqueue(buffer, 1);
    int result = bufferCircle_enqueue(buffer, 2);
    mu_assert("Erro: Enqueue deveria falhar quando o buffer está cheio", result == -1);
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_dequeue_from_single_element_buffer() {
    BufferCircle *buffer = bufferCircle_create(1);
    bufferCircle_enqueue(buffer, 100);
    int value;
    int result = bufferCircle_dequeue(buffer, &value);
    mu_assert("Erro: Dequeue deveria ter sucesso", result == 0);
    mu_assert("Erro: Valor removido incorreto", value == 100);
    mu_assert("Erro: O buffer deveria estar vazio após dequeue", bufferCircle_is_empty(buffer));
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_dequeue_multiple_elements() {
    BufferCircle *buffer = bufferCircle_create(3);
    bufferCircle_enqueue(buffer, 1);
    bufferCircle_enqueue(buffer, 2);
    bufferCircle_enqueue(buffer, 3);

    int value1, value2;
    bufferCircle_dequeue(buffer, &value1);
    bufferCircle_dequeue(buffer, &value2);

    mu_assert("Erro: Primeiro valor removido incorreto", value1 == 1);
    mu_assert("Erro: Segundo valor removido incorreto", value2 == 2);
    mu_assert("Erro: O buffer não deveria estar cheio após remover 2 elementos", !bufferCircle_is_full(buffer));
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_dequeue_from_empty_buffer() {
    BufferCircle *buffer = bufferCircle_create(2);
    int value;
    int result = bufferCircle_dequeue(buffer, &value);
    mu_assert("Erro: Dequeue deveria falhar em buffer vazio", result == -1);
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_check_buffer_empty() {
    BufferCircle *buffer = bufferCircle_create(2);
    mu_assert("Erro: O buffer deveria estar vazio inicialmente", bufferCircle_is_empty(buffer));
    bufferCircle_enqueue(buffer, 5);
    mu_assert("Erro: O buffer não deveria estar vazio após enqueue", !bufferCircle_is_empty(buffer));
    int value;
    bufferCircle_dequeue(buffer, &value);
    mu_assert("Erro: O buffer deveria estar vazio após dequeue", bufferCircle_is_empty(buffer));
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_check_buffer_full() {
    BufferCircle *buffer = bufferCircle_create(2);
    bufferCircle_enqueue(buffer, 1);
    mu_assert("Erro: O buffer não deveria estar cheio após uma inserção", !bufferCircle_is_full(buffer));
    bufferCircle_enqueue(buffer, 2);
    mu_assert("Erro: O buffer deveria estar cheio após duas inserções", bufferCircle_is_full(buffer));
    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_circular_enqueue_dequeue() {
    BufferCircle *buffer = bufferCircle_create(3);
    bufferCircle_enqueue(buffer, 1);
    bufferCircle_enqueue(buffer, 2);
    bufferCircle_enqueue(buffer, 3);
    mu_assert("Erro: O buffer deveria estar cheio", bufferCircle_is_full(buffer));

    int value;
    bufferCircle_dequeue(buffer, &value);
    mu_assert("Erro: Primeiro valor removido incorreto", value == 1);

    bufferCircle_enqueue(buffer, 4);
    mu_assert("Erro: O buffer deveria estar cheio após enqueue circular", bufferCircle_is_full(buffer));

    int value2, value3, value4;
    bufferCircle_dequeue(buffer, &value2);
    bufferCircle_dequeue(buffer, &value3);
    bufferCircle_dequeue(buffer, &value4);

    mu_assert("Erro: Segundo valor removido incorreto", value2 == 2);
    mu_assert("Erro: Terceiro valor removido incorreto", value3 == 3);
    mu_assert("Erro: Quarto valor removido incorreto", value4 == 4);
    mu_assert("Erro: O buffer deveria estar vazio após todas as remoções", bufferCircle_is_empty(buffer));

    bufferCircle_destroy(buffer);
    return 0;
}

static char * test_data_integrity_after_operations() {
    BufferCircle *buffer = bufferCircle_create(5);
    for (int i = 1; i <= 5; i++) {
        bufferCircle_enqueue(buffer, i);
    }

    int value;
    bufferCircle_dequeue(buffer, &value);
    mu_assert("Erro: Primeiro valor removido incorreto", value == 1);
    bufferCircle_enqueue(buffer, 6);

    int expected[] = {2, 3, 4, 5, 6};
    for (int i = 0; i < 5; i++) {
        bufferCircle_dequeue(buffer, &value);
        mu_assert("Erro: Integridade dos dados comprometida", value == expected[i]);
    }

    mu_assert("Erro: O buffer deveria estar vazio após remover todos os elementos", bufferCircle_is_empty(buffer));
    bufferCircle_destroy(buffer);
    return 0;
}

static char * all_tests() {
    mu_run_test(test_create_buffer_valid_size);
    mu_run_test(test_create_buffer_zero_size);
    mu_run_test(test_enqueue_into_empty_buffer);
    mu_run_test(test_enqueue_until_full);
    mu_run_test(test_enqueue_into_full_buffer);
    mu_run_test(test_dequeue_from_single_element_buffer);
    mu_run_test(test_dequeue_multiple_elements);
    mu_run_test(test_dequeue_from_empty_buffer);
    mu_run_test(test_check_buffer_empty);
    mu_run_test(test_check_buffer_full);
    mu_run_test(test_circular_enqueue_dequeue);
    mu_run_test(test_data_integrity_after_operations);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", tests_run);

    return result != 0;
}
