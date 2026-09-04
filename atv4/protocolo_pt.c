#include <stdio.h>

#include "protocolo_pt.h"

canal_t  canal_ida;
canal_t  canal_volta;
unsigned tempo_ms;
int      verboso = 1;

/* ------------------------------------------------------------------ */
/* Canal                                                               */
/* ------------------------------------------------------------------ */

void poe(canal_t *canal, uint8_t byte)
{
    canal->buffer[canal->entrada % 512] = byte;
    canal->entrada++;
}

int tem_byte(const canal_t *canal)
{
    return canal->entrada != canal->saida;
}

uint8_t tira(canal_t *canal)
{
    uint8_t byte = canal->buffer[canal->saida % 512];
    canal->saida++;
    return byte;
}

unsigned quantos_bytes(const canal_t *canal)
{
    return canal->entrada - canal->saida;
}

void canal_limpa(canal_t *canal)
{
    canal->entrada = 0;
    canal->saida = 0;
}

void monta_quadro(canal_t *canal, const uint8_t *dados, uint8_t qtd, int corrompe)
{
    uint8_t soma = 0;
    uint8_t i;

    poe(canal, STX);
    poe(canal, qtd);
    for (i = 0; i < qtd; i++) {
        poe(canal, dados[i]);
        soma = (uint8_t)(soma + dados[i]);
    }
    poe(canal, corrompe ? (uint8_t)(soma + 1) : soma);
    poe(canal, ETX);
}

/* ------------------------------------------------------------------ */
/* Protothread receptora                                               */
/*                                                                     */
/* Com protothread o protocolo fica escrito na ordem em que os campos  */
/* chegam, sem precisar de uma maquina de estados explicita: cada      */
/* PT_WAIT_UNTIL para a tarefa ate o proximo byte aparecer.            */
/* ------------------------------------------------------------------ */

rx_resultado_t rx_resultado;
uint8_t        rx_dados[MAX_DADOS + 1];
uint8_t        rx_qtd;

int protothread_receptora(struct pt *pt)
{
    static uint8_t byte, chk, soma;
    static unsigned i;

    PT_BEGIN(pt);

    for (;;) {
        /* descarta tudo ate achar um STX */
        do {
            PT_WAIT_UNTIL(pt, tem_byte(&canal_ida));
            byte = tira(&canal_ida);
        } while (byte != STX);

        rx_resultado = RX_NADA;

        PT_WAIT_UNTIL(pt, tem_byte(&canal_ida));
        rx_qtd = tira(&canal_ida);

        if (rx_qtd == 0) {
            rx_resultado = RX_QTD_INVALIDA;
            if (verboso) {
                printf("%4u ms [rx] QTD igual a zero, quadro descartado\n", tempo_ms);
            }
            continue;
        }

        soma = 0;
        for (i = 0; i < rx_qtd; i++) {
            PT_WAIT_UNTIL(pt, tem_byte(&canal_ida));
            rx_dados[i] = tira(&canal_ida);
            soma = (uint8_t)(soma + rx_dados[i]);
        }

        PT_WAIT_UNTIL(pt, tem_byte(&canal_ida));
        chk = tira(&canal_ida);

        PT_WAIT_UNTIL(pt, tem_byte(&canal_ida));
        byte = tira(&canal_ida);

        if (chk != soma) {
            rx_resultado = RX_CHK_ERRADO;
            if (verboso) {
                printf("%4u ms [rx] checksum errado (recebi 0x%02X, calculei 0x%02X),"
                       " quadro descartado\n", tempo_ms, chk, soma);
            }
        } else if (byte != ETX) {
            rx_resultado = RX_ETX_AUSENTE;
            if (verboso) {
                printf("%4u ms [rx] ETX ausente, quadro descartado\n", tempo_ms);
            }
        } else {
            rx_resultado = RX_VALIDO;
            poe(&canal_volta, ACK);
            if (verboso) {
                rx_dados[rx_qtd] = '\0';
                printf("%4u ms [rx] quadro valido: \"%s\", respondendo ACK\n",
                       tempo_ms, (char *)rx_dados);
            }
        }
    }

    PT_END(pt);
}

/* ------------------------------------------------------------------ */
/* Protothread transmissora                                            */
/* ------------------------------------------------------------------ */

const uint8_t *tx_dados;
uint8_t        tx_qtd;
int            tx_corrompe;
int            tx_tentativas;
tx_resultado_t tx_resultado;

void transmissora_prepara(const uint8_t *dados, uint8_t qtd, int corrompe)
{
    tx_dados = dados;
    tx_qtd = qtd;
    tx_corrompe = corrompe;
    tx_tentativas = 0;
    tx_resultado = TX_ENVIANDO;
}

int protothread_transmissora(struct pt *pt)
{
    static unsigned t0;
    static uint8_t resposta;

    PT_BEGIN(pt);

    for (tx_tentativas = 1; tx_tentativas <= MAX_TENTATIVAS; tx_tentativas++) {

        monta_quadro(&canal_ida, tx_dados, tx_qtd, tx_corrompe > 0);
        if (verboso) {
            printf("%4u ms [tx] quadro enviado (tentativa %d, %u bytes de dados)%s\n",
                   tempo_ms, tx_tentativas, tx_qtd,
                   tx_corrompe > 0 ? "  <-- checksum corrompido de proposito" : "");
        }
        if (tx_corrompe > 0) {
            tx_corrompe--;
        }
        t0 = tempo_ms;

        /* espera o ACK ou o estouro do tempo maximo */
        PT_WAIT_UNTIL(pt, tem_byte(&canal_volta) || (tempo_ms - t0) >= TIMEOUT_MS);

        if (tem_byte(&canal_volta)) {
            resposta = tira(&canal_volta);
            if (resposta == ACK) {
                tx_resultado = TX_ACK;
                if (verboso) {
                    printf("%4u ms [tx] ACK recebido na tentativa %d\n",
                           tempo_ms, tx_tentativas);
                }
                PT_EXIT(pt);
            }
        }

        if (verboso) {
            printf("%4u ms [tx] sem ACK depois de %d ms, vou reenviar\n",
                   tempo_ms, TIMEOUT_MS);
        }
    }

    tx_resultado = TX_DESISTIU;
    if (verboso) {
        printf("%4u ms [tx] desisti apos %d tentativas\n", tempo_ms, MAX_TENTATIVAS);
    }

    PT_END(pt);
}
