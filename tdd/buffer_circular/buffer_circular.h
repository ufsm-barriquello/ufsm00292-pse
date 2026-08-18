/*
 * buffer_circular.h
 *
 * Modulo de manipulacao de um buffer circular (fila FIFO) sobre uma
 * area de memoria fornecida pelo usuario, sem alocacao dinamica.
 *
 * Uso tipico:
 *
 *     buffer_circular_t b;
 *     uint8_t area[16];
 *     uint8_t dado;
 *
 *     bc_inicializa(&b, area, sizeof(area));
 *     bc_escreve(&b, 0x55);
 *     bc_le(&b, &dado);
 *
 * UFSM00292 - Projeto de Sistemas Embarcados
 */

#ifndef BUFFER_CIRCULAR_H
#define BUFFER_CIRCULAR_H

#include <stddef.h>
#include <stdint.h>

/* codigos de retorno */
#define BC_OK               0   /* operacao realizada com sucesso     */
#define BC_ERRO_PARAMETRO  -1   /* ponteiro nulo ou tamanho invalido  */
#define BC_ERRO_CHEIO      -2   /* escrita em buffer cheio            */
#define BC_ERRO_VAZIO      -3   /* leitura de buffer vazio            */

typedef struct
{
    uint8_t *dados;     /* area de memoria fornecida pelo usuario */
    size_t   tamanho;   /* capacidade, em bytes                   */
    size_t   inicio;    /* indice de leitura (START)              */
    size_t   contador;  /* quantidade de dados armazenados        */
} buffer_circular_t;

/* Prepara o buffer para uso sobre a area indicada.
 * Retorna BC_OK ou BC_ERRO_PARAMETRO. */
int bc_inicializa(buffer_circular_t *b, uint8_t *area, size_t tamanho);

/* Descarta todo o conteudo armazenado.
 * Retorna BC_OK ou BC_ERRO_PARAMETRO. */
int bc_limpa(buffer_circular_t *b);

/* Insere um dado no fim da fila.
 * Retorna BC_OK, BC_ERRO_CHEIO ou BC_ERRO_PARAMETRO. */
int bc_escreve(buffer_circular_t *b, uint8_t dado);

/* Remove o dado mais antigo da fila e o copia para *dado.
 * Retorna BC_OK, BC_ERRO_VAZIO ou BC_ERRO_PARAMETRO. */
int bc_le(buffer_circular_t *b, uint8_t *dado);

/* Retorna 1 se o buffer estiver vazio, 0 caso contrario. */
int bc_vazio(const buffer_circular_t *b);

/* Retorna 1 se o buffer estiver cheio, 0 caso contrario. */
int bc_cheio(const buffer_circular_t *b);

/* Retorna a quantidade de dados armazenados. */
size_t bc_ocupacao(const buffer_circular_t *b);

/* Retorna a capacidade total do buffer, em bytes. */
size_t bc_capacidade(const buffer_circular_t *b);

#endif /* BUFFER_CIRCULAR_H */
