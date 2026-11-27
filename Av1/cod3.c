/* Exercício 3: módulo de buffer circular (fila FIFO de bytes), desenvolvido com TDD.
   Este arquivo reúne a versão final: interface, implementação e testes (T01 a T11). */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ===================== Interface (cb.h) ===================== */
typedef enum {
    CB_OK       =  0,
    CB_ERRO_ARG = -1,   /* argumento inválido (NULL, capacidade 0) */
    CB_CHEIO    = -2,   /* push em buffer cheio                    */
    CB_VAZIO    = -3    /* pop em buffer vazio                     */
} cb_status_t;

typedef struct {
    uint8_t *dados;      /* área de armazenamento                  */
    size_t   capacidade; /* nº máximo de elementos                 */
    size_t   inicio;     /* índice do elemento mais antigo (leitura) */
    size_t   tamanho;    /* nº de elementos armazenados            */
} cb_t;

cb_status_t cb_init(cb_t *cb, uint8_t *armazenamento, size_t capacidade);
cb_status_t cb_push(cb_t *cb, uint8_t valor);
cb_status_t cb_pop(cb_t *cb, uint8_t *saida);
size_t      cb_tamanho(const cb_t *cb);
size_t      cb_capacidade(const cb_t *cb);
bool        cb_vazio(const cb_t *cb);
bool        cb_cheio(const cb_t *cb);
void        cb_limpar(cb_t *cb);

/* ================== Implementação (cb.c) ================== */
/* Índice físico no vetor do elemento que está 'deslocamento' posições
   à frente do mais antigo. */
static size_t indice_fisico(const cb_t *cb, size_t deslocamento)
{
    return (cb->inicio + deslocamento) % cb->capacidade;
}

cb_status_t cb_init(cb_t *cb, uint8_t *armazenamento, size_t capacidade)
{
    if (cb == NULL || armazenamento == NULL || capacidade == 0)
        return CB_ERRO_ARG;

    cb->dados      = armazenamento;
    cb->capacidade = capacidade;
    cb_limpar(cb);
    return CB_OK;
}

size_t cb_tamanho(const cb_t *cb)
{
    return cb == NULL ? 0 : cb->tamanho;
}

size_t cb_capacidade(const cb_t *cb)
{
    return cb == NULL ? 0 : cb->capacidade;
}

bool cb_vazio(const cb_t *cb)
{
    return cb_tamanho(cb) == 0;
}

bool cb_cheio(const cb_t *cb)
{
    return cb != NULL && cb->tamanho == cb->capacidade;
}

void cb_limpar(cb_t *cb)
{
    if (cb == NULL)
        return;
    cb->inicio  = 0;
    cb->tamanho = 0;
}

cb_status_t cb_push(cb_t *cb, uint8_t valor)
{
    if (cb == NULL)
        return CB_ERRO_ARG;
    if (cb_cheio(cb))
        return CB_CHEIO;

    cb->dados[indice_fisico(cb, cb->tamanho)] = valor;
    cb->tamanho++;
    return CB_OK;
}

cb_status_t cb_pop(cb_t *cb, uint8_t *saida)
{
    if (cb == NULL || saida == NULL)
        return CB_ERRO_ARG;
    if (cb_vazio(cb))
        return CB_VAZIO;

    *saida = cb->dados[cb->inicio];
    cb->inicio = indice_fisico(cb, 1);
    cb->tamanho--;
    return CB_OK;
}

/* ===================== Testes (test_cb.c) ===================== */
#define CAP    4
#define GUARDA 16

/* Memória de teste: após a área útil há bytes-guarda (0xAA). Se o módulo
   escrever além da capacidade, os guardas são corrompidos e o teste falha. */
typedef struct {
    uint8_t area[CAP];
    uint8_t guarda[GUARDA];
} mem_t;

static void mem_prepara(mem_t *m)
{
    memset(m->area, 0, CAP);
    memset(m->guarda, 0xAA, GUARDA);
}

static int guarda_intacta(const mem_t *m)
{
    for (int i = 0; i < GUARDA; i++)
        if (m->guarda[i] != 0xAA)
            return 0;
    return 1;
}

static int total = 0, falhas = 0;

#define VERIFICA(cond) do { \
    total++; \
    if (!(cond)) { \
        falhas++; \
        printf("    FALHOU: %s (linha %d)\n", #cond, __LINE__); \
    } \
} while (0)

static void executa(const char *nome, void (*teste)(void))
{
    int antes = falhas;
    teste();
    printf("[%s] %s\n", falhas == antes ? "OK    " : "FALHOU", nome);
}

static void teste_01(void)
{
    mem_t m; cb_t cb; mem_prepara(&m);

    VERIFICA(cb_init(&cb, m.area, CAP) == CB_OK);
    VERIFICA(cb_tamanho(&cb) == 0);
    VERIFICA(cb_vazio(&cb));
    VERIFICA(!cb_cheio(&cb));
    VERIFICA(cb_capacidade(&cb) == CAP);
}

static void teste_02(void)
{
    mem_t m; cb_t cb; mem_prepara(&m);

    VERIFICA(cb_init(&cb, NULL, CAP) == CB_ERRO_ARG);
    VERIFICA(cb_init(&cb, m.area, 0) == CB_ERRO_ARG);
    VERIFICA(cb_init(NULL, m.area, CAP) == CB_ERRO_ARG);
}

static void teste_03(void)
{
    mem_t m; cb_t cb; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);

    VERIFICA(cb_push(&cb, 0x42) == CB_OK);
    VERIFICA(cb_tamanho(&cb) == 1);
    VERIFICA(!cb_vazio(&cb));
}

static void teste_04(void)
{
    mem_t m; cb_t cb; uint8_t saida = 0; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);
    cb_push(&cb, 0x42);

    VERIFICA(cb_pop(&cb, &saida) == CB_OK);
    VERIFICA(saida == 0x42);
    VERIFICA(cb_tamanho(&cb) == 0);
    VERIFICA(cb_vazio(&cb));
}

static void teste_05(void)
{
    mem_t m; cb_t cb; uint8_t s = 0; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);
    cb_push(&cb, 10);
    cb_push(&cb, 20);
    cb_push(&cb, 30);

    VERIFICA(cb_tamanho(&cb) == 3);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 10);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 20);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 30);
    VERIFICA(cb_vazio(&cb));
}

static void teste_06(void)
{
    mem_t m; cb_t cb; uint8_t s = 0; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);
    for (uint8_t i = 1; i <= CAP; i++)
        VERIFICA(cb_push(&cb, i) == CB_OK);

    VERIFICA(cb_push(&cb, 99) == CB_CHEIO);
    VERIFICA(cb_tamanho(&cb) == CAP);
    VERIFICA(guarda_intacta(&m));
    for (uint8_t i = 1; i <= CAP; i++)
        VERIFICA(cb_pop(&cb, &s) == CB_OK && s == i);
}

static void teste_07(void)
{
    mem_t m; cb_t cb; uint8_t s = 0x77; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);

    VERIFICA(cb_pop(&cb, &s) == CB_VAZIO);
    VERIFICA(s == 0x77);
    VERIFICA(cb_tamanho(&cb) == 0);

    cb_push(&cb, 1);
    cb_pop(&cb, &s);
    VERIFICA(cb_pop(&cb, &s) == CB_VAZIO);
    VERIFICA(cb_tamanho(&cb) == 0);
}

static void teste_08(void)
{
    mem_t m; cb_t cb; uint8_t s = 0; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);

    for (uint8_t i = 1; i <= 4; i++) cb_push(&cb, i);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 1);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 2);
    VERIFICA(cb_push(&cb, 5) == CB_OK);   /* ocupa posição 0 */
    VERIFICA(cb_push(&cb, 6) == CB_OK);   /* ocupa posição 1 */
    VERIFICA(cb_tamanho(&cb) == 4);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 3);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 4);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 5);
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 6);
    VERIFICA(cb_vazio(&cb));

    /* várias voltas completas */
    for (uint8_t i = 0; i < 10; i++) {
        VERIFICA(cb_push(&cb, i) == CB_OK);
        VERIFICA(cb_pop(&cb, &s) == CB_OK && s == i);
    }
    VERIFICA(guarda_intacta(&m));
}

static void teste_09(void)
{
    mem_t m; cb_t cb; uint8_t s; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);

    VERIFICA(!cb_cheio(&cb));
    for (uint8_t i = 0; i < CAP - 1; i++) cb_push(&cb, i);
    VERIFICA(!cb_cheio(&cb));
    cb_push(&cb, 7);
    VERIFICA(cb_cheio(&cb));
    cb_pop(&cb, &s);
    VERIFICA(!cb_cheio(&cb));
}

static void teste_10(void)
{
    mem_t m; cb_t cb; uint8_t s = 0; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);
    for (uint8_t i = 0; i < CAP; i++) cb_push(&cb, i);

    cb_limpar(&cb);
    VERIFICA(cb_vazio(&cb));
    VERIFICA(cb_tamanho(&cb) == 0);
    VERIFICA(!cb_cheio(&cb));
    VERIFICA(cb_capacidade(&cb) == CAP);

    VERIFICA(cb_push(&cb, 9) == CB_OK);   /* continua utilizável */
    VERIFICA(cb_pop(&cb, &s) == CB_OK && s == 9);
}

static void teste_11(void)
{
    mem_t m; cb_t cb; uint8_t s = 0; mem_prepara(&m);
    cb_init(&cb, m.area, CAP);
    cb_push(&cb, 5);

    VERIFICA(cb_push(NULL, 1) == CB_ERRO_ARG);
    VERIFICA(cb_pop(NULL, &s) == CB_ERRO_ARG);
    VERIFICA(cb_pop(&cb, NULL) == CB_ERRO_ARG);
    VERIFICA(cb_tamanho(&cb) == 1);       /* pop(NULL) não consumiu nada */
    VERIFICA(cb_tamanho(NULL) == 0);
    VERIFICA(cb_capacidade(NULL) == 0);
    VERIFICA(cb_vazio(NULL));
    VERIFICA(!cb_cheio(NULL));
    cb_limpar(NULL);                      /* não deve falhar */
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== Testes do buffer circular (T01..T11) ===\n");
    executa("T01 - Estado inicial após cb_init", teste_01);
    executa("T02 - cb_init rejeita argumentos inválidos", teste_02);
    executa("T03 - push de 1 elemento em buffer vazio", teste_03);
    executa("T04 - pop devolve o elemento inserido e esvazia", teste_04);
    executa("T05 - ordem FIFO com vários elementos", teste_05);
    executa("T06 - push em buffer cheio falha sem corromper dados", teste_06);
    executa("T07 - pop em buffer vazio falha sem alterar a saída", teste_07);
    executa("T08 - wrap-around (volta ao início do vetor)", teste_08);
    executa("T09 - cb_cheio reflete tamanho == capacidade", teste_09);
    executa("T10 - cb_limpar descarta o conteúdo", teste_10);
    executa("T11 - ponteiros NULL nas demais funções", teste_11);
    printf("\nVerificações: %d, falhas: %d\n", total, falhas);
    return falhas != 0;
}
