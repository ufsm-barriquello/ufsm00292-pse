#include <stdio.h>
#include <stdlib.h>

/* --- Macros de Teste fornecidas na aula_tdd.pdf --- */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;
static char * executa_testes (void);

/* ================================================================= *
 *                   IMPLEMENTAÇÃO DO BUFFER CIRCULAR                *
 * ================================================================= */

#define TAMANHO_BUFFER 3 // Tamanho pequeno para facilitar o teste de "buffer cheio"

typedef struct {
    int dados[TAMANHO_BUFFER];
    int start;
    int end;
    int count;
} BufferCircular;

void bc_init(BufferCircular *bc) {
    bc->start = 0;
    bc->end = 0;
    bc->count = 0;
}

int bc_is_empty(BufferCircular *bc) {
    return bc->count == 0;
}

int bc_is_full(BufferCircular *bc) {
    return bc->count == TAMANHO_BUFFER;
}

int bc_push(BufferCircular *bc, int valor) {
    if (bc_is_full(bc)) return -1; // Falha: buffer cheio
    bc->dados[bc->end] = valor;
    bc->end = (bc->end + 1) % TAMANHO_BUFFER; // O módulo (%) garante a circularidade
    bc->count++;
    return 0; // Sucesso
}

int bc_pop(BufferCircular *bc, int *valor) {
    if (bc_is_empty(bc)) return -1; // Falha: buffer vazio
    *valor = bc->dados[bc->start];
    bc->start = (bc->start + 1) % TAMANHO_BUFFER; // O módulo (%) garante a circularidade
    bc->count--;
    return 0; // Sucesso
}

/* ================================================================= *
 *                            TESTES (TDD)                           *
 * ================================================================= */

BufferCircular buffer; // Instância global para os testes

static char * teste_inicializacao(void) {
    bc_init(&buffer);
    verifica("erro: buffer recem inicializado deveria estar vazio", bc_is_empty(&buffer) == 1);
    verifica("erro: buffer recem inicializado nao deveria estar cheio", bc_is_full(&buffer) == 0);
    return 0;
}

static char * teste_insercao_simples(void) {
    bc_init(&buffer);
    verifica("erro: falha ao inserir primeiro elemento", bc_push(&buffer, 10) == 0);
    verifica("erro: buffer nao deveria estar vazio apos insercao", bc_is_empty(&buffer) == 0);
    return 0;
}

static char * teste_remocao_simples(void) {
    int valor_lido = 0;
    bc_init(&buffer);
    bc_push(&buffer, 42);
    verifica("erro: falha ao remover elemento", bc_pop(&buffer, &valor_lido) == 0);
    verifica("erro: valor lido diferente do inserido", valor_lido == 42);
    verifica("erro: buffer deveria voltar a estar vazio", bc_is_empty(&buffer) == 1);
    return 0;
}

static char * teste_buffer_cheio(void) {
    bc_init(&buffer);
    bc_push(&buffer, 1);
    bc_push(&buffer, 2);
    bc_push(&buffer, 3); // Preenche o buffer todo (TAMANHO_BUFFER = 3)
    
    verifica("erro: buffer deveria estar cheio", bc_is_full(&buffer) == 1);
    verifica("erro: buffer deveria recusar nova insercao", bc_push(&buffer, 4) == -1);
    return 0;
}

static char * teste_circularidade(void) {
    int valor;
    bc_init(&buffer);
    // Insere 3, remove 2 -> start avança
    bc_push(&buffer, 100);
    bc_push(&buffer, 200);
    bc_push(&buffer, 300);
    bc_pop(&buffer, &valor);
    bc_pop(&buffer, &valor);
    
    // Agora insere mais 2. Como o tamanho é 3, o índice de 'end' deve dar a volta no array (voltar pro zero).
    verifica("erro: falha ao inserir no fim circular", bc_push(&buffer, 400) == 0);
    verifica("erro: falha ao dar a volta no buffer", bc_push(&buffer, 500) == 0);
    
    // O próximo pop deve ser o 300 (que sobrou da primeira rodada)
    bc_pop(&buffer, &valor);
    verifica("erro: circularidade comprometeu a fila (FIFO)", valor == 300);
    return 0;
}

static char * executa_testes (void) {
    executa_teste(teste_inicializacao);
    executa_teste(teste_insercao_simples);
    executa_teste(teste_remocao_simples);
    executa_teste(teste_buffer_cheio);
    executa_teste(teste_circularidade);
    return 0;
}

int main() {
    char *resultado = executa_testes();
    if (resultado != 0) {
        printf("%s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);
    return resultado != 0;
}