/* Testes do decodificador (TDD). Compile e rode: make test */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "protocolo.h"

/* Monta STX|QTD|DADOS|CHK|ETX com checksum correto. Devolve o tamanho. */
static int monta(uint8_t *quadro, const uint8_t *dados, int qtd)
{
    uint8_t soma = 0;
    int i;

    quadro[0] = STX;
    quadro[1] = (uint8_t)qtd;
    for (i = 0; i < qtd; i++) {
        quadro[2 + i] = dados[i];
        soma = (uint8_t)(soma + dados[i]);
    }
    quadro[2 + qtd] = soma;
    quadro[3 + qtd] = ETX;
    return qtd + 4;
}

static void envia(fsm_t *fsm, const uint8_t *quadro, int tamanho)
{
    int i;
    for (i = 0; i < tamanho; i++) {
        fsm_processa(fsm, quadro[i]);
    }
}

static void lixo_antes_do_stx_e_ignorado(void)
{
    fsm_t fsm;

    fsm_init(&fsm);
    fsm_processa(&fsm, 0x41);
    fsm_processa(&fsm, 0xFF);

    assert(fsm.estado == AGUARDA_STX);
    assert(fsm.pronto == 0);
}

static void mensagem_valida_e_decodificada(void)
{
    const uint8_t dados[3] = { 0x10, 0x20, 0x30 };
    uint8_t quadro[16];
    fsm_t fsm;

    fsm_init(&fsm);
    envia(&fsm, quadro, monta(quadro, dados, 3));

    assert(fsm.pronto == 1);
    assert(fsm.qtd == 3);
    assert(memcmp(fsm.dados, dados, 3) == 0);
}

static void mensagem_so_fica_pronta_no_etx(void)
{
    const uint8_t dados[1] = { 0x55 };
    uint8_t quadro[8];
    int tamanho, i;
    fsm_t fsm;

    fsm_init(&fsm);
    tamanho = monta(quadro, dados, 1);
    for (i = 0; i < tamanho - 1; i++) {
        fsm_processa(&fsm, quadro[i]);
        assert(fsm.pronto == 0);
    }
    fsm_processa(&fsm, quadro[tamanho - 1]);

    assert(fsm.pronto == 1);
}

static void qtd_zero_e_descartada(void)
{
    fsm_t fsm;

    fsm_init(&fsm);
    fsm_processa(&fsm, STX);
    fsm_processa(&fsm, 0x00);

    assert(fsm.estado == AGUARDA_STX);
    assert(fsm.pronto == 0);
}

static void checksum_errado_e_descartado(void)
{
    const uint8_t dados[2] = { 0x01, 0x02 };
    uint8_t quadro[8];
    int tamanho;
    fsm_t fsm;

    fsm_init(&fsm);
    tamanho = monta(quadro, dados, 2);
    quadro[4]++;                        /* corrompe o CHK */
    envia(&fsm, quadro, tamanho);

    assert(fsm.pronto == 0);
}

static void etx_ausente_e_descartado(void)
{
    const uint8_t dados[2] = { 0x01, 0x02 };
    uint8_t quadro[8];
    int tamanho;
    fsm_t fsm;

    fsm_init(&fsm);
    tamanho = monta(quadro, dados, 2);
    quadro[tamanho - 1] = 0x99;         /* lugar do ETX com lixo */
    envia(&fsm, quadro, tamanho);

    assert(fsm.pronto == 0);
    assert(fsm.estado == AGUARDA_STX);
}

static void stx_e_etx_dentro_dos_dados_sao_dados(void)
{
    const uint8_t dados[3] = { STX, ETX, STX };
    uint8_t quadro[16];
    fsm_t fsm;

    fsm_init(&fsm);
    envia(&fsm, quadro, monta(quadro, dados, 3));

    assert(fsm.pronto == 1);
    assert(memcmp(fsm.dados, dados, 3) == 0);
}

static void checksum_soma_modulo_256(void)
{
    const uint8_t dados[2] = { 0xFF, 0x02 };   /* soma 0x101 -> CHK 0x01 */
    uint8_t quadro[8];
    fsm_t fsm;

    fsm_init(&fsm);
    envia(&fsm, quadro, monta(quadro, dados, 2));

    assert(quadro[4] == 0x01);
    assert(fsm.pronto == 1);
}

static void duas_mensagens_seguidas(void)
{
    const uint8_t a[1] = { 0x7F };
    const uint8_t b[2] = { 0x01, 0xFF };
    uint8_t quadro[8];
    fsm_t fsm;

    fsm_init(&fsm);

    envia(&fsm, quadro, monta(quadro, a, 1));
    assert(fsm.pronto == 1);
    assert(fsm.dados[0] == 0x7F);

    envia(&fsm, quadro, monta(quadro, b, 2));
    assert(fsm.pronto == 1);
    assert(fsm.qtd == 2);
    assert(memcmp(fsm.dados, b, 2) == 0);
}

#define RODA(teste) do { teste(); printf("  [ok] %s\n", #teste); } while (0)

int main(void)
{
    printf("Testes do decodificador STX|QTD|DADOS|CHK|ETX\n\n");

    RODA(lixo_antes_do_stx_e_ignorado);
    RODA(mensagem_valida_e_decodificada);
    RODA(mensagem_so_fica_pronta_no_etx);
    RODA(qtd_zero_e_descartada);
    RODA(checksum_errado_e_descartado);
    RODA(etx_ausente_e_descartado);
    RODA(stx_e_etx_dentro_dos_dados_sao_dados);
    RODA(checksum_soma_modulo_256);
    RODA(duas_mensagens_seguidas);

    printf("\nTODOS OS TESTES PASSARAM\n");
    return 0;
}
