#include <stdio.h>
#include "buffer_circular.h"

/* Macros de testes clássicas da UFSM */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                  if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

// Teste 1: Inicialização do Buffer
static char * teste_inicializacao(void) {
    BufferCircular b;
    buffer_init(&b);

    verifica("Erro: Buffer recém-inicializado deveria estar vazio", buffer_is_empty(&b) == true);
    verifica("Erro: Buffer recém-inicializado não deveria estar cheio", buffer_is_full(&b) == false);
    return NULL; // NULL indica sucesso
}

// Teste 2: Inserção Única (Push)
static char * teste_push_unico(void) {
    BufferCircular b;
    buffer_init(&b);

    bool sucesso = buffer_push(&b, 100);
    verifica("Erro: Push deveria retornar true para um buffer com espaço", sucesso == true);
    verifica("Erro: Buffer não deveria estar vazio após push", buffer_is_empty(&b) == false);
    verifica("Erro: Buffer com 1 de 5 elementos não deveria estar cheio", buffer_is_full(&b) == false);
    return NULL;
}

// Teste 3: Remoção Única (Pop)
static char * teste_pop_unico(void) {
    BufferCircular b;
    buffer_init(&b);
    buffer_push(&b, 200);

    uint8_t valor = 0;
    bool sucesso = buffer_pop(&b, &valor);
    verifica("Erro: Pop deveria retornar true para um buffer com dados", sucesso == true);
    verifica("Erro: Valor lido incorreto (esperava 200)", valor == 200);
    verifica("Erro: Buffer deveria voltar a ficar vazio", buffer_is_empty(&b) == true);
    return NULL;
}

// Teste 4: Comportamento FIFO (First-In, First-Out)
static char * teste_fifo_multiplo(void) {
    BufferCircular b;
    buffer_init(&b);

    buffer_push(&b, 10);
    buffer_push(&b, 20);
    buffer_push(&b, 30);

    uint8_t valor = 0;
    buffer_pop(&b, &valor);
    verifica("Erro FIFO: Primeiro elemento deveria ser 10", valor == 10);

    buffer_pop(&b, &valor);
    verifica("Erro FIFO: Segundo elemento deveria ser 20", valor == 20);

    buffer_pop(&b, &valor);
    verifica("Erro FIFO: Terceiro elemento deveria ser 30", valor == 30);

    return NULL;
}

// Teste 5: Estouro de capacidade (Overflow/Full)
static char * teste_buffer_cheio(void) {
    BufferCircular b;
    buffer_init(&b);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        verifica("Erro: Falha ao encher o buffer", buffer_push(&b, (uint8_t)(i + 1)) == true);
    }

    verifica("Erro: Buffer deveria reportar cheio", buffer_is_full(&b) == true);
    verifica("Erro: Push em buffer cheio deveria retornar false", buffer_push(&b, 99) == false);
    return NULL;
}

// Teste 6: Proteção contra Underflow (Pop em buffer vazio)
static char * teste_pop_vazio(void) {
    BufferCircular b;
    buffer_init(&b);

    uint8_t valor = 0;
    bool sucesso = buffer_pop(&b, &valor);
    verifica("Erro: Pop em buffer vazio deveria retornar false", sucesso == false);
    return NULL;
}

// Teste 7: Circularidade e Wrap-around dos índices (Ponteiros dando a volta)
static char * teste_circularidade(void) {
    BufferCircular b;
    buffer_init(&b);

    // 1. Enche o buffer completamente com {1, 2, 3, 4, 5}
    for (uint8_t i = 1; i <= BUFFER_SIZE; i++) {
        buffer_push(&b, i);
    }

    // 2. Libera dois espaços (remoção do 1 e 2)
    uint8_t valor = 0;
    buffer_pop(&b, &valor); // Remove 1
    buffer_pop(&b, &valor); // Remove 2

    // 3. Insere 6 e 7. Isso força head a "dar a volta" (wrap-around) para as posições 0 e 1.
    verifica("Erro: Falha no push circular de 6", buffer_push(&b, 6) == true);
    verifica("Erro: Falha no push circular de 7", buffer_push(&b, 7) == true);

    // 4. Valida a sequência FIFO restante: {3, 4, 5, 6, 7}
    buffer_pop(&b, &valor); verifica("Erro circularidade: Esperava 3", valor == 3);
    buffer_pop(&b, &valor); verifica("Erro circularidade: Esperava 4", valor == 4);
    buffer_pop(&b, &valor); verifica("Erro circularidade: Esperava 5", valor == 5);
    buffer_pop(&b, &valor); verifica("Erro circularidade: Esperava 6", valor == 6);
    buffer_pop(&b, &valor); verifica("Erro circularidade: Esperava 7", valor == 7);

    verifica("Erro: Buffer deveria estar vazio no final", buffer_is_empty(&b) == true);
    return NULL;
}

// Teste 8: Push em buffer cheio não deve corromper os dados já armazenados
static char * teste_integridade_apos_push_cheio(void) {
    BufferCircular b;
    buffer_init(&b);

    // Enche com {1, 2, 3, 4, 5}
    for (uint8_t i = 1; i <= BUFFER_SIZE; i++) {
        buffer_push(&b, i);
    }

    // Tenta 3 pushes que devem ser todos rejeitados
    verifica("Erro: 1o push em cheio deveria falhar", buffer_push(&b, 99) == false);
    verifica("Erro: 2o push em cheio deveria falhar", buffer_push(&b, 98) == false);
    verifica("Erro: 3o push em cheio deveria falhar", buffer_push(&b, 97) == false);

    // O conteúdo original deve estar 100% intacto, na ordem original
    uint8_t valor = 0;
    for (uint8_t i = 1; i <= BUFFER_SIZE; i++) {
        verifica("Erro: Pop deveria funcionar após pushes rejeitados", buffer_pop(&b, &valor) == true);
        verifica("Erro: Push rejeitado corrompeu os dados do buffer", valor == i);
    }

    verifica("Erro: Buffer deveria estar vazio ao final", buffer_is_empty(&b) == true);
    return NULL;
}

// Teste 9: Múltiplas voltas completas (stress de wrap-around)
static char * teste_multiplas_voltas(void) {
    BufferCircular b;
    buffer_init(&b);

    // 4 ciclos de encher-totalmente / esvaziar-totalmente.
    // Cada ciclo faz head e tail darem uma volta inteira no array.
    for (int volta = 0; volta < 4; volta++) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            verifica("Erro multi-volta: push deveria funcionar", buffer_push(&b, (uint8_t)(volta * 10 + i)) == true);
        }
        verifica("Erro multi-volta: buffer deveria estar cheio", buffer_is_full(&b) == true);

        uint8_t valor = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            verifica("Erro multi-volta: pop deveria funcionar", buffer_pop(&b, &valor) == true);
            verifica("Erro multi-volta: valor fora de ordem FIFO", valor == (uint8_t)(volta * 10 + i));
        }
        verifica("Erro multi-volta: buffer deveria estar vazio", buffer_is_empty(&b) == true);
    }
    return NULL;
}

// Teste 10: Reuso do buffer após ser totalmente esvaziado
static char * teste_reuso_apos_esvaziar(void) {
    BufferCircular b;
    buffer_init(&b);

    // Enche e esvazia parcialmente para deslocar os índices para o "meio" do array
    buffer_push(&b, 1);
    buffer_push(&b, 2);
    buffer_push(&b, 3);
    uint8_t valor = 0;
    buffer_pop(&b, &valor);
    buffer_pop(&b, &valor);
    buffer_pop(&b, &valor);

    // Com os índices deslocados, o buffer vazio deve aceitar a capacidade TOTAL novamente
    verifica("Erro: Buffer esvaziado deveria estar vazio", buffer_is_empty(&b) == true);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        verifica("Erro: Buffer reusado deveria aceitar BUFFER_SIZE elementos", buffer_push(&b, (uint8_t)i) == true);
    }
    verifica("Erro: Buffer reusado deveria estar cheio", buffer_is_full(&b) == true);
    return NULL;
}

// Teste 11: Valores de borda do uint8_t (0 e 255)
static char * teste_valores_borda(void) {
    BufferCircular b;
    buffer_init(&b);

    // O 0 é traiçoeiro: é o valor "natural de sentinela", então uma implementação
    // que confundisse "posição vazia" com "valor zero" falharia aqui.
    verifica("Erro: Deveria aceitar o valor 0", buffer_push(&b, 0) == true);
    verifica("Erro: Deveria aceitar o valor 255", buffer_push(&b, 255) == true);
    verifica("Erro: Buffer com o valor 0 dentro não está vazio", buffer_is_empty(&b) == false);

    uint8_t valor = 123; // valor inicial diferente de 0 para provar que o pop escreveu
    verifica("Erro: Pop do valor 0 deveria funcionar", buffer_pop(&b, &valor) == true);
    verifica("Erro: Esperava recuperar o valor 0", valor == 0);
    verifica("Erro: Pop do valor 255 deveria funcionar", buffer_pop(&b, &valor) == true);
    verifica("Erro: Esperava recuperar o valor 255", valor == 255);
    return NULL;
}

// Teste 12: Robustez contra ponteiro de buffer NULL (nenhuma chamada deve travar)
static char * teste_protecao_buffer_null(void) {
    uint8_t valor = 0;

    // Semântica defensiva: um buffer inexistente não tem nada para ler
    // E também não tem espaço para escrever -> nenhuma operação pode ter sucesso.
    verifica("Erro: Buffer NULL deveria ser reportado como vazio", buffer_is_empty(NULL) == true);
    verifica("Erro: Buffer NULL deveria ser reportado como cheio", buffer_is_full(NULL) == true);
    verifica("Erro: Push em buffer NULL deveria retornar false", buffer_push(NULL, 42) == false);
    verifica("Erro: Pop em buffer NULL deveria retornar false", buffer_pop(NULL, &valor) == false);

    buffer_init(NULL); // não deve causar segmentation fault
    return NULL;
}

// Teste 13: Robustez contra ponteiro de saída NULL no pop
static char * teste_pop_com_saida_null(void) {
    BufferCircular b;
    buffer_init(&b);
    buffer_push(&b, 77);

    verifica("Erro: Pop com ponteiro de saída NULL deveria retornar false", buffer_pop(&b, NULL) == false);

    // O elemento não pode ter sido consumido pela chamada inválida
    uint8_t valor = 0;
    verifica("Erro: Pop válido após pop inválido deveria funcionar", buffer_pop(&b, &valor) == true);
    verifica("Erro: Pop inválido consumiu o elemento indevidamente", valor == 77);
    return NULL;
}

// Agrupador dos casos de teste
static char * executa_todos_testes(void) {
    executa_teste(teste_inicializacao);
    executa_teste(teste_push_unico);
    executa_teste(teste_pop_unico);
    executa_teste(teste_fifo_multiplo);
    executa_teste(teste_buffer_cheio);
    executa_teste(teste_pop_vazio);
    executa_teste(teste_circularidade);
    executa_teste(teste_integridade_apos_push_cheio);
    executa_teste(teste_multiplas_voltas);
    executa_teste(teste_reuso_apos_esvaziar);
    executa_teste(teste_valores_borda);
    executa_teste(teste_protecao_buffer_null);
    executa_teste(teste_pop_com_saida_null);
    return NULL;
}

int main(void) {
    char *resultado = executa_todos_testes();

    printf("\n=========================================\n");
    if (resultado != NULL) {
        printf("FALHA NOS TESTES: %s\n", resultado);
    } else {
        printf("SUCESSO: Todos os testes passaram!\n");
    }
    printf("Total de testes unitários executados: %d\n", testes_executados);
    printf("=========================================\n\n");

    return (resultado != NULL); // Retorna código de erro se algum teste falhou
}
