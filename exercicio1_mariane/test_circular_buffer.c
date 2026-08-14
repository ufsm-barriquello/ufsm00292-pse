#include <stdio.h>
#include <string.h>
#include "circular_buffer.h"

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT_TRUE(cond) do {                                          \
    tests_run++;                                                        \
    if (!(cond)) {                                                      \
        tests_failed++;                                                 \
        printf("  FALHOU [%s:%d]: %s\n", __FILE__, __LINE__, #cond);    \
    }                                                                   \
} while (0)

#define ASSERT_EQ(expected, actual) do {                                \
    tests_run++;                                                        \
    if ((expected) != (actual)) {                                       \
        tests_failed++;                                                 \
        printf("  FALHOU [%s:%d]: esperado %ld, obtido %ld\n",          \
               __FILE__, __LINE__, (long)(expected), (long)(actual));   \
    }                                                                   \
} while (0)

#define RUN_TEST(fn) do { printf("- %s\n", #fn); fn(); } while (0)

void test_buffer_novo_esta_vazio(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    ASSERT_TRUE(cb_is_empty(&cb));
}

void test_buffer_novo_nao_esta_cheio(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    ASSERT_TRUE(!cb_is_full(&cb));
}

void test_buffer_novo_tem_size_zero(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    ASSERT_EQ(0, cb_size(&cb));
}

void test_init_com_parametros_invalidos_falha(void) {
    CircularBuffer cb;
    ASSERT_TRUE(!cb_init(&cb, NULL, 5));
    int arr[5];
    ASSERT_TRUE(!cb_init(&cb, arr, 0));
    ASSERT_TRUE(!cb_init(NULL, arr, 5));
}

void test_push_torna_buffer_nao_vazio(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    cb_push(&cb, 10);
    ASSERT_TRUE(!cb_is_empty(&cb));
}

void test_push_incrementa_size(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    cb_push(&cb, 10);
    ASSERT_EQ(1, cb_size(&cb));
    cb_push(&cb, 20);
    ASSERT_EQ(2, cb_size(&cb));
}

void test_push_ate_capacidade_marca_cheio(void) {
    int arr[3];
    CircularBuffer cb;
    cb_init(&cb, arr, 3);
    cb_push(&cb, 1);
    cb_push(&cb, 2);
    cb_push(&cb, 3);
    ASSERT_TRUE(cb_is_full(&cb));
    ASSERT_EQ(3, cb_size(&cb));
}

void test_push_em_buffer_cheio_falha(void) {
    int arr[2];
    CircularBuffer cb;
    cb_init(&cb, arr, 2);
    cb_push(&cb, 1);
    cb_push(&cb, 2);
    ASSERT_TRUE(!cb_push(&cb, 3)); // deve rejeitar
    ASSERT_EQ(2, cb_size(&cb));    // size não muda
}

void test_pop_em_buffer_vazio_falha(void) {
    int arr[3];
    CircularBuffer cb;
    cb_init(&cb, arr, 3);
    int valor = -999;
    ASSERT_TRUE(!cb_pop(&cb, &valor));
    ASSERT_EQ(-999, valor); // não deve ter sido alterado
}

void test_pop_retorna_ordem_fifo(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    cb_push(&cb, 10);
    cb_push(&cb, 20);
    cb_push(&cb, 30);

    int valor;
    ASSERT_TRUE(cb_pop(&cb, &valor));
    ASSERT_EQ(10, valor);
    ASSERT_TRUE(cb_pop(&cb, &valor));
    ASSERT_EQ(20, valor);
    ASSERT_TRUE(cb_pop(&cb, &valor));
    ASSERT_EQ(30, valor);
}

void test_pop_decrementa_size(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    cb_push(&cb, 1);
    cb_push(&cb, 2);
    int valor;
    cb_pop(&cb, &valor);
    ASSERT_EQ(1, cb_size(&cb));
}

void test_pop_ultimo_elemento_esvazia_buffer(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    cb_push(&cb, 42);
    int valor;
    cb_pop(&cb, &valor);
    ASSERT_TRUE(cb_is_empty(&cb));
}

void test_wrap_around_mantem_ordem_correta(void) {
    int arr[3];
    CircularBuffer cb;
    cb_init(&cb, arr, 3);

    cb_push(&cb, 1);
    cb_push(&cb, 2);
    cb_push(&cb, 3); // cheio: [1,2,3]

    int valor;
    cb_pop(&cb, &valor); // remove 1 -> libera espaço no início
    ASSERT_EQ(1, valor);

    cb_push(&cb, 4); // deve dar a volta (wrap) e ocupar o slot do 1

    cb_pop(&cb, &valor); ASSERT_EQ(2, valor);
    cb_pop(&cb, &valor); ASSERT_EQ(3, valor);
    cb_pop(&cb, &valor); ASSERT_EQ(4, valor);
    ASSERT_TRUE(cb_is_empty(&cb));
}

void test_multiplos_ciclos_encher_esvaziar(void) {
    int arr[4];
    CircularBuffer cb;
    cb_init(&cb, arr, 4);

    for (int ciclo = 0; ciclo < 10; ciclo++) {
        for (int i = 0; i < 4; i++) {
            ASSERT_TRUE(cb_push(&cb, ciclo * 10 + i));
        }
        ASSERT_TRUE(cb_is_full(&cb));

        for (int i = 0; i < 4; i++) {
            int valor;
            ASSERT_TRUE(cb_pop(&cb, &valor));
            ASSERT_EQ(ciclo * 10 + i, valor);
        }
        ASSERT_TRUE(cb_is_empty(&cb));
    }
}
void test_reset_esvazia_buffer(void) {
    int arr[5];
    CircularBuffer cb;
    cb_init(&cb, arr, 5);
    cb_push(&cb, 1);
    cb_push(&cb, 2);
    cb_reset(&cb);
    ASSERT_TRUE(cb_is_empty(&cb));
    ASSERT_EQ(0, cb_size(&cb));
}

void test_reset_permite_novas_insercoes_corretas(void) {
    int arr[3];
    CircularBuffer cb;
    cb_init(&cb, arr, 3);
    cb_push(&cb, 1);
    cb_push(&cb, 2);
    cb_reset(&cb);

    cb_push(&cb, 99);
    int valor;
    cb_pop(&cb, &valor);
    ASSERT_EQ(99, valor);
}

int main(void) {
    RUN_TEST(test_buffer_novo_esta_vazio);
    RUN_TEST(test_buffer_novo_nao_esta_cheio);
    RUN_TEST(test_buffer_novo_tem_size_zero);
    RUN_TEST(test_init_com_parametros_invalidos_falha);

    RUN_TEST(test_push_torna_buffer_nao_vazio);
    RUN_TEST(test_push_incrementa_size);
    RUN_TEST(test_push_ate_capacidade_marca_cheio);
    RUN_TEST(test_push_em_buffer_cheio_falha);

    RUN_TEST(test_pop_em_buffer_vazio_falha);
    RUN_TEST(test_pop_retorna_ordem_fifo);
    RUN_TEST(test_pop_decrementa_size);
    RUN_TEST(test_pop_ultimo_elemento_esvazia_buffer);

    RUN_TEST(test_wrap_around_mantem_ordem_correta);
    RUN_TEST(test_multiplos_ciclos_encher_esvaziar);

    RUN_TEST(test_reset_esvazia_buffer);
    RUN_TEST(test_reset_permite_novas_insercoes_corretas);

    printf("\n%d asserts executados, %d falharam\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}