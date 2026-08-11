#include <stdio.h>
#include "buffer_circular.h"

#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                  if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

static char * teste_buffer_novo_esta_vazio(void) {
    BufferCircular b;
    buffer_init(&b);
    verifica("Erro T01: Buffer novo deveria estar vazio", buffer_is_empty(&b) == true);
    verifica("Erro T01: Tamanho inicial deveria ser 0", buffer_size(&b) == 0);
    return 0;
}

static char * teste_pop_em_buffer_vazio_falha(void) {
    BufferCircular b;
    buffer_init(&b);
    int valor = 999;
    verifica("Erro T02: Pop em buffer vazio deveria retornar false", buffer_pop(&b, &valor) == false);
    verifica("Erro T02: Valor nao deve ser alterado em falha de pop", valor == 999);
    return 0;
}

static char * teste_push_e_pop_um_elemento(void) {
    BufferCircular b;
    buffer_init(&b);
    int valor_out = 0;
    verifica("Erro T03: Push falhou em buffer vazio", buffer_push(&b, 42) == true);
    verifica("Erro T03: Buffer nao deveria estar vazio apos push", buffer_is_empty(&b) == false);
    verifica("Erro T03: Pop falhou em buffer com elemento", buffer_pop(&b, &valor_out) == true);
    verifica("Erro T03: Valor extraido diferente do inserido", valor_out == 42);
    verifica("Erro T03: Buffer deveria estar vazio apos remover unico elemento", buffer_is_empty(&b) == true);
    return 0;
}

static char * teste_push_ate_encher_e_rejeitar(void) {
    BufferCircular b;
    buffer_init(&b);
    for (int i = 0; i < BUFFER_CAPACIDADE; i++) {
        verifica("Erro T04: Push valido rejeitado antes de encher", buffer_push(&b, i * 10) == true);
    }
    verifica("Erro T04: Buffer deveria estar cheio", buffer_is_full(&b) == true);
    verifica("Erro T05: Push adicional em buffer cheio deveria retornar false", buffer_push(&b, 999) == false);
    return 0;
}

static char * teste_comportamento_fifo_e_wrap_around(void) {
    BufferCircular b;
    buffer_init(&b);
    int val = 0;

    buffer_push(&b, 100);
    buffer_push(&b, 200);
    buffer_push(&b, 300);

    buffer_pop(&b, &val);
    verifica("Erro T06: Primeiro elemento retirado deve ser 100", val == 100);

    for (int i = 0; i < (BUFFER_CAPACIDADE - 2); i++) {
        buffer_push(&b, 10 + i);
    }

    buffer_pop(&b, &val);
    verifica("Erro T07: FIFO incorreto apos wrap-around", val == 200);

    return 0;
}

static char * executa_todos_testes(void) {
    executa_teste(teste_buffer_novo_esta_vazio);
    executa_teste(teste_pop_em_buffer_vazio_falha);
    executa_teste(teste_push_e_pop_um_elemento);
    executa_teste(teste_push_ate_encher_e_rejeitar);
    executa_teste(teste_comportamento_fifo_e_wrap_around);
    return 0;
}

int main(void) {
    printf("===========================================\n");
    printf("  EXECUTANDO SUITE DE TESTES (TDD - BUFFER)\n");
    printf("===========================================\n");
    
    char *resultado = executa_todos_testes();
    
    if (resultado != 0) {
        printf("[FALHA] %s\n", resultado);
    } else {
        printf("[SUCESSO] TODOS OS TESTES PASSARAM COM SUCESSO!\n");
    }
    printf("Total de testes executados: %d\n", testes_executados);
    printf("===========================================\n");

    return (resultado != 0);
}