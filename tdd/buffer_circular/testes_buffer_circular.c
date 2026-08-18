/*
 * testes_buffer_circular.c
 *
 * Testes unitarios do modulo de buffer circular, escritos segundo a
 * metodologia TDD (test driven development).
 *
 * UFSM00292 - Projeto de Sistemas Embarcados
 */

#include <stdio.h>
#include <stdlib.h>

#include "buffer_circular.h"

/* macros de testes */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

static char *executa_testes(void);

int main(void)
{
    char *resultado = executa_testes();

    if (resultado != 0)
    {
        printf("%s\n", resultado);
    }
    else
    {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    return resultado != 0;
}

/* ------------------------------------------------------------------ */
/* Ciclo 1 - criacao do buffer                                         */
/* ------------------------------------------------------------------ */

static char *teste_inicializa_retorna_ok(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    verifica("erro: bc_inicializa() deveria retornar BC_OK",
             bc_inicializa(&b, area, sizeof(area)) == BC_OK);
    return 0;
}

static char *teste_buffer_novo_esta_vazio(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    bc_inicializa(&b, area, sizeof(area));
    verifica("erro: buffer recem-inicializado deveria estar vazio",
             bc_vazio(&b) == 1);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Ciclo 2 - capacidade, ocupacao e leitura de buffer vazio            */
/* ------------------------------------------------------------------ */

static char *teste_buffer_novo_nao_esta_cheio(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    bc_inicializa(&b, area, sizeof(area));
    verifica("erro: buffer recem-inicializado nao deveria estar cheio",
             bc_cheio(&b) == 0);
    return 0;
}

static char *teste_capacidade_eh_o_tamanho_informado(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    bc_inicializa(&b, area, sizeof(area));
    verifica("erro: bc_capacidade() deveria retornar 8",
             bc_capacidade(&b) == 8);
    return 0;
}

static char *teste_ocupacao_inicial_eh_zero(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    bc_inicializa(&b, area, sizeof(area));
    verifica("erro: bc_ocupacao() de buffer novo deveria retornar 0",
             bc_ocupacao(&b) == 0);
    return 0;
}

static char *teste_le_buffer_vazio_retorna_erro(void)
{
    buffer_circular_t b;
    uint8_t area[8];
    uint8_t dado = 0xAA;

    bc_inicializa(&b, area, sizeof(area));
    verifica("erro: ler de buffer vazio deveria retornar BC_ERRO_VAZIO",
             bc_le(&b, &dado) == BC_ERRO_VAZIO);
    verifica("erro: ler de buffer vazio nao deveria alterar o destino",
             dado == 0xAA);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Ciclo 3 - escrita, leitura do dado e ordem FIFO                     */
/* ------------------------------------------------------------------ */

static char *teste_escreve_retorna_ok(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    bc_inicializa(&b, area, sizeof(area));
    verifica("erro: bc_escreve() deveria retornar BC_OK",
             bc_escreve(&b, 0x55) == BC_OK);
    return 0;
}

static char *teste_apos_escrever_buffer_nao_esta_vazio(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    bc_inicializa(&b, area, sizeof(area));
    bc_escreve(&b, 0x55);
    verifica("erro: apos escrever, o buffer nao deveria estar vazio",
             bc_vazio(&b) == 0);
    verifica("erro: apos escrever um dado, a ocupacao deveria ser 1",
             bc_ocupacao(&b) == 1);
    return 0;
}

static char *teste_le_o_dado_escrito(void)
{
    buffer_circular_t b;
    uint8_t area[8];
    uint8_t dado = 0;

    bc_inicializa(&b, area, sizeof(area));
    bc_escreve(&b, 0x55);
    verifica("erro: bc_le() deveria retornar BC_OK",
             bc_le(&b, &dado) == BC_OK);
    verifica("erro: bc_le() deveria devolver o dado escrito (0x55)",
             dado == 0x55);
    return 0;
}

static char *teste_ordem_fifo(void)
{
    buffer_circular_t b;
    uint8_t area[8];
    uint8_t dado = 0;

    bc_inicializa(&b, area, sizeof(area));
    bc_escreve(&b, 1);
    bc_escreve(&b, 2);
    bc_escreve(&b, 3);

    bc_le(&b, &dado);
    verifica("erro: o primeiro dado lido deveria ser 1", dado == 1);
    bc_le(&b, &dado);
    verifica("erro: o segundo dado lido deveria ser 2", dado == 2);
    bc_le(&b, &dado);
    verifica("erro: o terceiro dado lido deveria ser 3", dado == 3);
    return 0;
}

static char *teste_apos_ler_tudo_buffer_fica_vazio(void)
{
    buffer_circular_t b;
    uint8_t area[8];
    uint8_t dado = 0;

    bc_inicializa(&b, area, sizeof(area));
    bc_escreve(&b, 1);
    bc_escreve(&b, 2);
    bc_le(&b, &dado);
    bc_le(&b, &dado);

    verifica("erro: apos ler todos os dados o buffer deveria estar vazio",
             bc_vazio(&b) == 1);
    verifica("erro: apos ler todos os dados a ocupacao deveria ser 0",
             bc_ocupacao(&b) == 0);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Ciclo 4 - buffer cheio e protecao contra estouro                    */
/* ------------------------------------------------------------------ */

static char *teste_buffer_fica_cheio_na_capacidade(void)
{
    buffer_circular_t b;
    uint8_t area[4];
    int i;

    bc_inicializa(&b, area, sizeof(area));
    for (i = 0; i < 4; i++)
    {
        bc_escreve(&b, (uint8_t)i);
    }

    verifica("erro: apos escrever 4 dados em area de 4 bytes o buffer deveria estar cheio",
             bc_cheio(&b) == 1);
    verifica("erro: buffer cheio deveria ter ocupacao igual a capacidade",
             bc_ocupacao(&b) == bc_capacidade(&b));
    return 0;
}

static char *teste_escreve_em_buffer_cheio_retorna_erro(void)
{
    buffer_circular_t b;
    uint8_t area[4];
    int i;

    bc_inicializa(&b, area, sizeof(area));
    for (i = 0; i < 4; i++)
    {
        bc_escreve(&b, (uint8_t)i);
    }

    verifica("erro: escrever em buffer cheio deveria retornar BC_ERRO_CHEIO",
             bc_escreve(&b, 0xFF) == BC_ERRO_CHEIO);
    verifica("erro: escrita rejeitada nao deveria alterar a ocupacao",
             bc_ocupacao(&b) == 4);
    return 0;
}

static char *teste_buffer_cheio_nao_sobrescreve_dados(void)
{
    buffer_circular_t b;
    uint8_t area[4];
    uint8_t dado = 0;
    int i;

    bc_inicializa(&b, area, sizeof(area));
    for (i = 0; i < 4; i++)
    {
        bc_escreve(&b, (uint8_t)(i + 1));
    }
    bc_escreve(&b, 0xFF);   /* rejeitada */

    for (i = 0; i < 4; i++)
    {
        bc_le(&b, &dado);
        verifica("erro: a escrita rejeitada corrompeu os dados armazenados",
                 dado == (uint8_t)(i + 1));
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Ciclo 5 - comportamento circular (wrap-around)                      */
/* ------------------------------------------------------------------ */

static char *teste_indices_dao_a_volta(void)
{
    buffer_circular_t b;
    uint8_t area[4];
    uint8_t dado = 0;
    int i;

    bc_inicializa(&b, area, sizeof(area));

    /* enche o buffer */
    for (i = 1; i <= 4; i++)
    {
        bc_escreve(&b, (uint8_t)i);
    }

    /* libera duas posicoes no inicio da area */
    bc_le(&b, &dado);
    bc_le(&b, &dado);

    /* estas duas escritas devem dar a volta e ocupar area[0] e area[1] */
    verifica("erro: deveria ser possivel escrever apos liberar espaco",
             bc_escreve(&b, 5) == BC_OK);
    verifica("erro: deveria ser possivel escrever apos liberar espaco",
             bc_escreve(&b, 6) == BC_OK);
    verifica("erro: apos dar a volta o buffer deveria estar cheio novamente",
             bc_cheio(&b) == 1);

    /* a ordem FIFO deve ser mantida atraves da volta */
    for (i = 3; i <= 6; i++)
    {
        bc_le(&b, &dado);
        verifica("erro: a ordem FIFO nao foi mantida apos o wrap-around",
                 dado == (uint8_t)i);
    }
    verifica("erro: o buffer deveria estar vazio ao final do teste",
             bc_vazio(&b) == 1);
    return 0;
}

static char *teste_uso_continuo_por_varias_voltas(void)
{
    buffer_circular_t b;
    uint8_t area[4];
    uint8_t dado = 0;
    int i;

    bc_inicializa(&b, area, sizeof(area));

    /* 100 escritas/leituras intercaladas: os indices dao varias voltas */
    for (i = 0; i < 100; i++)
    {
        verifica("erro: escrita falhou durante o uso continuo",
                 bc_escreve(&b, (uint8_t)i) == BC_OK);
        verifica("erro: leitura falhou durante o uso continuo",
                 bc_le(&b, &dado) == BC_OK);
        verifica("erro: o dado lido nao corresponde ao escrito no uso continuo",
                 dado == (uint8_t)i);
    }
    verifica("erro: o buffer deveria estar vazio ao final do uso continuo",
             bc_vazio(&b) == 1);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Ciclo 6 - descarte do conteudo e casos limite                       */
/* ------------------------------------------------------------------ */

static char *teste_limpa_esvazia_o_buffer(void)
{
    buffer_circular_t b;
    uint8_t area[4];

    bc_inicializa(&b, area, sizeof(area));
    bc_escreve(&b, 1);
    bc_escreve(&b, 2);

    verifica("erro: bc_limpa() deveria retornar BC_OK",
             bc_limpa(&b) == BC_OK);
    verifica("erro: apos bc_limpa() o buffer deveria estar vazio",
             bc_vazio(&b) == 1);
    verifica("erro: apos bc_limpa() a ocupacao deveria ser 0",
             bc_ocupacao(&b) == 0);
    return 0;
}

static char *teste_buffer_utilizavel_apos_limpa(void)
{
    buffer_circular_t b;
    uint8_t area[4];
    uint8_t dado = 0;

    bc_inicializa(&b, area, sizeof(area));
    bc_escreve(&b, 1);
    bc_limpa(&b);

    verifica("erro: deveria ser possivel escrever apos bc_limpa()",
             bc_escreve(&b, 9) == BC_OK);
    bc_le(&b, &dado);
    verifica("erro: o dado lido apos bc_limpa() deveria ser 9", dado == 9);
    return 0;
}

static char *teste_buffer_de_uma_posicao(void)
{
    buffer_circular_t b;
    uint8_t area[1];
    uint8_t dado = 0;

    bc_inicializa(&b, area, sizeof(area));
    verifica("erro: buffer de 1 posicao deveria aceitar uma escrita",
             bc_escreve(&b, 7) == BC_OK);
    verifica("erro: buffer de 1 posicao deveria ficar cheio com um dado",
             bc_cheio(&b) == 1);
    verifica("erro: buffer de 1 posicao nao deveria aceitar a segunda escrita",
             bc_escreve(&b, 8) == BC_ERRO_CHEIO);

    bc_le(&b, &dado);
    verifica("erro: o dado lido do buffer de 1 posicao deveria ser 7",
             dado == 7);
    verifica("erro: buffer de 1 posicao deveria ficar vazio apos a leitura",
             bc_vazio(&b) == 1);
    verifica("erro: buffer de 1 posicao deveria aceitar nova escrita apos a leitura",
             bc_escreve(&b, 8) == BC_OK);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Ciclo 7 - robustez contra parametros invalidos                      */
/* ------------------------------------------------------------------ */

static char *teste_inicializa_rejeita_parametros_invalidos(void)
{
    buffer_circular_t b;
    uint8_t area[8];

    verifica("erro: bc_inicializa() deveria rejeitar buffer nulo",
             bc_inicializa(NULL, area, sizeof(area)) == BC_ERRO_PARAMETRO);
    verifica("erro: bc_inicializa() deveria rejeitar area nula",
             bc_inicializa(&b, NULL, sizeof(area)) == BC_ERRO_PARAMETRO);
    verifica("erro: bc_inicializa() deveria rejeitar tamanho zero",
             bc_inicializa(&b, area, 0) == BC_ERRO_PARAMETRO);
    return 0;
}

static char *teste_escreve_rejeita_buffer_nulo(void)
{
    verifica("erro: bc_escreve() deveria rejeitar buffer nulo",
             bc_escreve(NULL, 1) == BC_ERRO_PARAMETRO);
    return 0;
}

static char *teste_le_rejeita_parametros_nulos(void)
{
    buffer_circular_t b;
    uint8_t area[8];
    uint8_t dado = 0;

    bc_inicializa(&b, area, sizeof(area));
    bc_escreve(&b, 1);

    verifica("erro: bc_le() deveria rejeitar buffer nulo",
             bc_le(NULL, &dado) == BC_ERRO_PARAMETRO);
    verifica("erro: bc_le() deveria rejeitar destino nulo",
             bc_le(&b, NULL) == BC_ERRO_PARAMETRO);
    verifica("erro: as chamadas invalidas nao deveriam consumir dados",
             bc_ocupacao(&b) == 1);
    return 0;
}

static char *teste_consultas_com_buffer_nulo(void)
{
    verifica("erro: bc_vazio(NULL) deveria retornar 1 (nada a ler)",
             bc_vazio(NULL) == 1);
    verifica("erro: bc_cheio(NULL) deveria retornar 1 (nada pode ser escrito)",
             bc_cheio(NULL) == 1);
    verifica("erro: bc_ocupacao(NULL) deveria retornar 0",
             bc_ocupacao(NULL) == 0);
    verifica("erro: bc_capacidade(NULL) deveria retornar 0",
             bc_capacidade(NULL) == 0);
    verifica("erro: bc_limpa(NULL) deveria retornar BC_ERRO_PARAMETRO",
             bc_limpa(NULL) == BC_ERRO_PARAMETRO);
    return 0;
}

/* ------------------------------------------------------------------ */

static char *executa_testes(void)
{
    /* ciclo 1 */
    executa_teste(teste_inicializa_retorna_ok);
    executa_teste(teste_buffer_novo_esta_vazio);

    /* ciclo 2 */
    executa_teste(teste_buffer_novo_nao_esta_cheio);
    executa_teste(teste_capacidade_eh_o_tamanho_informado);
    executa_teste(teste_ocupacao_inicial_eh_zero);
    executa_teste(teste_le_buffer_vazio_retorna_erro);

    /* ciclo 3 */
    executa_teste(teste_escreve_retorna_ok);
    executa_teste(teste_apos_escrever_buffer_nao_esta_vazio);
    executa_teste(teste_le_o_dado_escrito);
    executa_teste(teste_ordem_fifo);
    executa_teste(teste_apos_ler_tudo_buffer_fica_vazio);

    /* ciclo 4 */
    executa_teste(teste_buffer_fica_cheio_na_capacidade);
    executa_teste(teste_escreve_em_buffer_cheio_retorna_erro);
    executa_teste(teste_buffer_cheio_nao_sobrescreve_dados);

    /* ciclo 5 */
    executa_teste(teste_indices_dao_a_volta);
    executa_teste(teste_uso_continuo_por_varias_voltas);

    /* ciclo 6 */
    executa_teste(teste_limpa_esvazia_o_buffer);
    executa_teste(teste_buffer_utilizavel_apos_limpa);
    executa_teste(teste_buffer_de_uma_posicao);

    /* ciclo 7 */
    executa_teste(teste_inicializa_rejeita_parametros_invalidos);
    executa_teste(teste_escreve_rejeita_buffer_nulo);
    executa_teste(teste_le_rejeita_parametros_nulos);
    executa_teste(teste_consultas_com_buffer_nulo);

    return 0;
}
