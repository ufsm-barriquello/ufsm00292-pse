/*
 * Testes do tratamento do protocolo com protothreads.
 *
 * Os nove primeiros testes sao os mesmos da atividade anterior (decodificador
 * do protocolo com maquina de estados), agora rodando em cima da protothread
 * receptora: o protocolo tem que continuar sendo interpretado do mesmo jeito.
 *
 * Os seis ultimos cobrem o que a atividade acrescentou: ACK, tempo maximo de
 * espera, retransmissao e a troca completa entre as duas protothreads.
 *
 * Compile e rode: make test
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "protocolo_pt.h"

static struct pt pt_rx, pt_tx;

static void reinicia(void)
{
    canal_limpa(&canal_ida);
    canal_limpa(&canal_volta);
    tempo_ms = 0;
    rx_resultado = RX_NADA;
    rx_qtd = 0;
    PT_INIT(&pt_rx);
    PT_INIT(&pt_tx);
}

/* ================================================================== */
/* Testes herdados da atividade anterior (decodificacao do protocolo)  */
/* ================================================================== */

static void lixo_antes_do_stx_e_ignorado(void)
{
    reinicia();
    poe(&canal_ida, 0x41);
    poe(&canal_ida, 0xFF);
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_NADA);
}

static void mensagem_valida_e_decodificada(void)
{
    const uint8_t dados[3] = { 0x10, 0x20, 0x30 };

    reinicia();
    monta_quadro(&canal_ida, dados, 3, 0);
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_VALIDO);
    assert(rx_qtd == 3);
    assert(memcmp(rx_dados, dados, 3) == 0);
}

static void mensagem_so_fica_pronta_no_etx(void)
{
    const uint8_t dados[1] = { 0x55 };
    canal_t quadro;
    unsigned total, i;

    reinicia();
    canal_limpa(&quadro);
    monta_quadro(&quadro, dados, 1, 0);
    total = quantos_bytes(&quadro);

    /* entrega o quadro byte a byte: so o ETX pode dar a mensagem por pronta */
    for (i = 0; i < total - 1; i++) {
        poe(&canal_ida, tira(&quadro));
        protothread_receptora(&pt_rx);
        assert(rx_resultado == RX_NADA);
    }
    poe(&canal_ida, tira(&quadro));
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_VALIDO);
}

static void qtd_zero_e_descartada(void)
{
    reinicia();
    poe(&canal_ida, STX);
    poe(&canal_ida, 0x00);
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_QTD_INVALIDA);
}

static void checksum_errado_e_descartado(void)
{
    const uint8_t dados[2] = { 0x01, 0x02 };

    reinicia();
    monta_quadro(&canal_ida, dados, 2, 1);   /* 1 = corrompe o CHK */
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_CHK_ERRADO);
}

static void etx_ausente_e_descartado(void)
{
    const uint8_t dados[2] = { 0x01, 0x02 };

    reinicia();
    monta_quadro(&canal_ida, dados, 2, 0);
    canal_ida.buffer[canal_ida.entrada - 1] = 0x99;   /* lugar do ETX com lixo */
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_ETX_AUSENTE);
}

static void stx_e_etx_dentro_dos_dados_sao_dados(void)
{
    const uint8_t dados[3] = { STX, ETX, STX };

    reinicia();
    monta_quadro(&canal_ida, dados, 3, 0);
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_VALIDO);
    assert(rx_qtd == 3);
    assert(memcmp(rx_dados, dados, 3) == 0);
}

static void checksum_soma_modulo_256(void)
{
    const uint8_t dados[2] = { 0xFF, 0x02 };   /* soma 0x101 -> CHK 0x01 */

    reinicia();
    monta_quadro(&canal_ida, dados, 2, 0);

    assert(canal_ida.buffer[4] == 0x01);

    protothread_receptora(&pt_rx);
    assert(rx_resultado == RX_VALIDO);
}

static void duas_mensagens_seguidas(void)
{
    const uint8_t a[1] = { 0x7F };
    const uint8_t b[2] = { 0x01, 0xFF };

    reinicia();

    monta_quadro(&canal_ida, a, 1, 0);
    protothread_receptora(&pt_rx);
    assert(rx_resultado == RX_VALIDO);
    assert(rx_dados[0] == 0x7F);

    monta_quadro(&canal_ida, b, 2, 0);
    protothread_receptora(&pt_rx);
    assert(rx_resultado == RX_VALIDO);
    assert(rx_qtd == 2);
    assert(memcmp(rx_dados, b, 2) == 0);
}

/* ================================================================== */
/* Testes da atividade nova (ACK, tempo de espera e retransmissao)     */
/* ================================================================== */

static void receptora_responde_ack_no_quadro_valido(void)
{
    const uint8_t dados[1] = { 0x2A };

    reinicia();
    monta_quadro(&canal_ida, dados, 1, 0);
    protothread_receptora(&pt_rx);

    assert(quantos_bytes(&canal_volta) == 1);
    assert(tira(&canal_volta) == ACK);
}

static void receptora_nao_responde_ack_em_quadro_invalido(void)
{
    const uint8_t dados[1] = { 0x2A };

    reinicia();
    monta_quadro(&canal_ida, dados, 1, 1);   /* CHK corrompido */
    protothread_receptora(&pt_rx);

    assert(rx_resultado == RX_CHK_ERRADO);
    assert(quantos_bytes(&canal_volta) == 0);
}

static void transmissora_envia_quadro_completo(void)
{
    const uint8_t dados[2] = { 0x01, 0x02 };

    reinicia();
    transmissora_prepara(dados, 2, 0);
    protothread_transmissora(&pt_tx);

    /* STX + QTD + 2 dados + CHK + ETX */
    assert(quantos_bytes(&canal_ida) == 6);
    assert(canal_ida.buffer[0] == STX);
    assert(canal_ida.buffer[1] == 2);
    assert(canal_ida.buffer[4] == 0x03);
    assert(canal_ida.buffer[5] == ETX);
}

static void transmissora_encerra_ao_receber_ack(void)
{
    const uint8_t dados[1] = { 0x2A };

    reinicia();
    transmissora_prepara(dados, 1, 0);
    protothread_transmissora(&pt_tx);

    poe(&canal_volta, ACK);

    assert(protothread_transmissora(&pt_tx) == PT_ENCERRADA);
    assert(tx_resultado == TX_ACK);
    assert(tx_tentativas == 1);
}

static void transmissora_reenvia_apos_tempo_maximo(void)
{
    const uint8_t dados[1] = { 0x2A };

    reinicia();
    transmissora_prepara(dados, 1, 0);
    protothread_transmissora(&pt_tx);
    assert(quantos_bytes(&canal_ida) == 5);

    canal_limpa(&canal_ida);   /* finge que o quadro se perdeu no caminho */

    for (tempo_ms = 1; tempo_ms <= (unsigned)TIMEOUT_MS; tempo_ms++) {
        protothread_transmissora(&pt_tx);
    }

    assert(quantos_bytes(&canal_ida) == 5);   /* reenviou */
    assert(tx_tentativas == 2);
}

static void transmissora_desiste_apos_max_tentativas(void)
{
    const uint8_t dados[1] = { 0x2A };
    int estado = PT_ESPERANDO;

    reinicia();
    transmissora_prepara(dados, 1, 0);

    /* ninguem responde ACK */
    for (tempo_ms = 0;
         tempo_ms < 4u * TIMEOUT_MS && estado != PT_ENCERRADA;
         tempo_ms++) {
        estado = protothread_transmissora(&pt_tx);
    }

    assert(estado == PT_ENCERRADA);
    assert(tx_resultado == TX_DESISTIU);
}

static void as_duas_protothreads_trocam_a_mensagem(void)
{
    const uint8_t dados[3] = { 'O', 'l', 'a' };
    int estado_tx = PT_ESPERANDO;

    reinicia();
    transmissora_prepara(dados, 3, 1);   /* o primeiro envio sai corrompido */

    for (tempo_ms = 0; tempo_ms < 1000 && estado_tx != PT_ENCERRADA; tempo_ms++) {
        protothread_receptora(&pt_rx);
        estado_tx = protothread_transmissora(&pt_tx);
    }

    assert(tx_resultado == TX_ACK);
    assert(tx_tentativas == 2);          /* a primeira falhou, a segunda passou */
    assert(rx_resultado == RX_VALIDO);
    assert(memcmp(rx_dados, dados, 3) == 0);
}

/* ================================================================== */

#define RODA(teste) do { teste(); printf("  [ok] %s\n", #teste); } while (0)

int main(void)
{
    verboso = 0;   /* os testes nao imprimem o trafego do protocolo */

    printf("Testes do protocolo STX|QTD|DADOS|CHK|ETX com protothreads\n");

    printf("\n- Testes da atividade anterior (decodificacao do protocolo)\n");
    RODA(lixo_antes_do_stx_e_ignorado);
    RODA(mensagem_valida_e_decodificada);
    RODA(mensagem_so_fica_pronta_no_etx);
    RODA(qtd_zero_e_descartada);
    RODA(checksum_errado_e_descartado);
    RODA(etx_ausente_e_descartado);
    RODA(stx_e_etx_dentro_dos_dados_sao_dados);
    RODA(checksum_soma_modulo_256);
    RODA(duas_mensagens_seguidas);

    printf("\n- Testes da atividade nova (ACK, tempo de espera, retransmissao)\n");
    RODA(receptora_responde_ack_no_quadro_valido);
    RODA(receptora_nao_responde_ack_em_quadro_invalido);
    RODA(transmissora_envia_quadro_completo);
    RODA(transmissora_encerra_ao_receber_ack);
    RODA(transmissora_reenvia_apos_tempo_maximo);
    RODA(transmissora_desiste_apos_max_tentativas);
    RODA(as_duas_protothreads_trocam_a_mensagem);

    printf("\nTODOS OS TESTES PASSARAM\n");
    return 0;
}
