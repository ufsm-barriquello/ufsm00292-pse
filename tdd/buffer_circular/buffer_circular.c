/*
 * buffer_circular.c
 *
 * Implementacao de um buffer circular (fila FIFO) sobre uma area de
 * memoria fornecida pelo usuario.
 *
 * O estado do buffer e mantido por dois campos: o indice de leitura
 * (inicio) e a quantidade de dados armazenados (contador). Essa
 * representacao evita a ambiguidade classica entre "cheio" e "vazio"
 * de implementacoes com dois indices e permite usar toda a capacidade
 * da area de memoria.
 *
 * UFSM00292 - Projeto de Sistemas Embarcados
 */

#include "buffer_circular.h"

/* indice da proxima posicao de escrita (END) */
static size_t bc_indice_escrita(const buffer_circular_t *b)
{
    return (b->inicio + b->contador) % b->tamanho;
}

int bc_inicializa(buffer_circular_t *b, uint8_t *area, size_t tamanho)
{
    if ((b == NULL) || (area == NULL) || (tamanho == 0))
    {
        return BC_ERRO_PARAMETRO;
    }

    b->dados = area;
    b->tamanho = tamanho;
    b->inicio = 0;
    b->contador = 0;

    return BC_OK;
}

int bc_limpa(buffer_circular_t *b)
{
    if (b == NULL)
    {
        return BC_ERRO_PARAMETRO;
    }

    b->inicio = 0;
    b->contador = 0;

    return BC_OK;
}

int bc_escreve(buffer_circular_t *b, uint8_t dado)
{
    if (b == NULL)
    {
        return BC_ERRO_PARAMETRO;
    }
    if (bc_cheio(b))
    {
        return BC_ERRO_CHEIO;
    }

    b->dados[bc_indice_escrita(b)] = dado;
    b->contador++;

    return BC_OK;
}

int bc_le(buffer_circular_t *b, uint8_t *dado)
{
    if ((b == NULL) || (dado == NULL))
    {
        return BC_ERRO_PARAMETRO;
    }
    if (bc_vazio(b))
    {
        return BC_ERRO_VAZIO;
    }

    *dado = b->dados[b->inicio];
    b->inicio = (b->inicio + 1) % b->tamanho;
    b->contador--;

    return BC_OK;
}

/* Um buffer inexistente e tratado como "sem dados para ler" e
 * "sem espaco para escrever", de modo que os lacos de uso normal
 * terminem em vez de acessar memoria invalida. */
int bc_vazio(const buffer_circular_t *b)
{
    if (b == NULL)
    {
        return 1;
    }

    return (b->contador == 0);
}

int bc_cheio(const buffer_circular_t *b)
{
    if (b == NULL)
    {
        return 1;
    }

    return (b->contador == b->tamanho);
}

size_t bc_ocupacao(const buffer_circular_t *b)
{
    if (b == NULL)
    {
        return 0;
    }

    return b->contador;
}

size_t bc_capacidade(const buffer_circular_t *b)
{
    if (b == NULL)
    {
        return 0;
    }

    return b->tamanho;
}
