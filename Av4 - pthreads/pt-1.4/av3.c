/*
 * av3.c - Protocolo STX | QTD | DADOS | CHK | ETX com Protothreads (pt-1.4)
 *
 * Terceira versao, comparada com:
 *   av1.c -> maquina de estados com switch/case
 *   av2.c -> maquina de estados com ponteiros de funcao
 *   av3.c -> protothreads: o fluxo e escrito de forma SEQUENCIAL e a
 *            biblioteca cuida de "lembrar" onde a thread parou.
 *
 * Transmissor e Receptor sao duas protothreads concorrentes, ligadas por
 * um canal de 1 byte (simulando uma UART). Cada uma bloqueia com
 * PT_WAIT_UNTIL quando o canal nao permite avancar, e o main faz o
 * escalonamento cooperativo (round-robin).
 *
 * Compilar:  gcc -Wall -std=c99 -o av3 av3.c
 * (pt.h, lc.h e lc-switch.h devem estar na mesma pasta)
 */

#include <stdio.h>
#include "pt.h"

#define STX 0x02
#define ETX 0x03

#define MAX_DADOS 255


/* ============================================================
   CANAL (simula uma UART de 1 byte)
   ============================================================ */

typedef struct
{
    unsigned char byte;          /* byte em transito                    */
    unsigned char cheio;         /* 1 = existe byte aguardando leitura  */
    unsigned int  total;         /* total de bytes ja escritos          */
    long          byte_a_corromper; /* indice do byte a corromper (-1 = nenhum) */
} Canal;


static void canal_escrever(Canal *c, unsigned char valor)
{
    if ((long)c->total == c->byte_a_corromper)
        valor ^= 0xFF;           /* injeta ruido na linha */

    c->byte = valor;
    c->cheio = 1;
    c->total++;
}


static unsigned char canal_ler(Canal *c)
{
    c->cheio = 0;
    return c->byte;
}


/*
 * Macros de E/S bloqueante sobre o canal. Sao usadas SOMENTE dentro de
 * uma protothread, pois expandem para PT_WAIT_UNTIL.
 * Importante: nao usar duas na mesma linha do codigo-fonte (o
 * lc-switch.h usa __LINE__ como identificador do ponto de retomada).
 */
#define CANAL_ENVIAR(pt, canal, valor)              \
    do {                                            \
        PT_WAIT_UNTIL((pt), !(canal)->cheio);       \
        canal_escrever((canal), (valor));           \
    } while (0)

#define CANAL_RECEBER(pt, canal, destino)           \
    do {                                            \
        PT_WAIT_UNTIL((pt), (canal)->cheio);        \
        (destino) = canal_ler(canal);               \
    } while (0)


/* ============================================================
   CHECKSUM
   ============================================================ */

unsigned char calcular_checksum(
    const unsigned char *dados,
    unsigned char qtd)
{
    unsigned char chk = 0;
    unsigned char i;

    for (i = 0; i < qtd; i++)
        chk ^= dados[i];

    return chk;
}


/* ============================================================
   TRANSMISSOR
   ============================================================ */

typedef struct
{
    const unsigned char *dados;
    unsigned char qtd;
} Mensagem;


/*
 * Em protothreads as variaveis locais NAO sao preservadas entre um
 * bloqueio e outro (nao ha pilha). Por isso todo o contexto vive na
 * struct, exatamente como no av2 - mas aqui nao existe campo "estado"
 * nem ponteiro de funcao: a posicao de retomada fica em 'pt'.
 */
typedef struct
{
    struct pt pt;                /* contexto da protothread (2 bytes) */

    Canal *canal;

    const Mensagem *msgs;
    unsigned int n_msgs;
    unsigned int m;              /* mensagem atual */

    unsigned char i;
    unsigned char chk;
} Transmissor;


static PT_THREAD(tx_thread(Transmissor *tx))
{
    PT_BEGIN(&tx->pt);

    for (tx->m = 0; tx->m < tx->n_msgs; tx->m++)
    {
        tx->chk = calcular_checksum(tx->msgs[tx->m].dados,
                                    tx->msgs[tx->m].qtd);

        printf("TX: ");

        CANAL_ENVIAR(&tx->pt, tx->canal, STX);
        printf("%02X ", STX);

        CANAL_ENVIAR(&tx->pt, tx->canal, tx->msgs[tx->m].qtd);
        printf("%02X ", tx->msgs[tx->m].qtd);

        for (tx->i = 0; tx->i < tx->msgs[tx->m].qtd; tx->i++)
        {
            CANAL_ENVIAR(&tx->pt, tx->canal, tx->msgs[tx->m].dados[tx->i]);
            printf("%02X ", tx->msgs[tx->m].dados[tx->i]);
        }

        CANAL_ENVIAR(&tx->pt, tx->canal, tx->chk);
        printf("%02X ", tx->chk);

        CANAL_ENVIAR(&tx->pt, tx->canal, ETX);
        printf("%02X\n", ETX);

        /* so inicia o proximo frame depois que o receptor leu o ETX */
        PT_WAIT_UNTIL(&tx->pt, !tx->canal->cheio);
    }

    PT_END(&tx->pt);
}


/* ============================================================
   RECEPTOR
   ============================================================ */

typedef struct
{
    struct pt pt;

    Canal *canal;

    unsigned int frames_esperados;
    unsigned int n;              /* frames processados */

    unsigned char b;             /* ultimo byte lido */
    unsigned char qtd;
    unsigned char i;
    unsigned char dados[MAX_DADOS];

    unsigned char chk;
    unsigned char chk_calculado;
    unsigned char etx;
} Receptor;


static PT_THREAD(rx_thread(Receptor *rx))
{
    PT_BEGIN(&rx->pt);

    for (rx->n = 0; rx->n < rx->frames_esperados; rx->n++)
    {
        /* Estado "RX_STX": descarta bytes ate achar o STX
           (ressincroniza sozinho depois de um frame ruim) */
        rx->b = 0;
        while (rx->b != STX)
        {
            CANAL_RECEBER(&rx->pt, rx->canal, rx->b);
        }

        /* Estado "RX_QTD_DADOS" */
        CANAL_RECEBER(&rx->pt, rx->canal, rx->qtd);

        /* Estado "RX_DADOS" - contador proprio, independente da
           posicao do STX (corrige o bug do dados[i - 2] do av1/av2) */
        rx->chk_calculado = 0;

        for (rx->i = 0; rx->i < rx->qtd; rx->i++)
        {
            CANAL_RECEBER(&rx->pt, rx->canal, rx->dados[rx->i]);
            rx->chk_calculado ^= rx->dados[rx->i];
        }

        /* Estado "RX_CHK" */
        CANAL_RECEBER(&rx->pt, rx->canal, rx->chk);

        /* Estado "RX_ETX" */
        CANAL_RECEBER(&rx->pt, rx->canal, rx->etx);

        /* Validacao e saida */
        if (rx->chk != rx->chk_calculado)
        {
            printf("Checksum invalido\n");
        }
        else if (rx->etx != ETX)
        {
            printf("ETX invalido\n");
        }
        else
        {
            printf("Frame recebido:\n");

            for (rx->i = 0; rx->i < rx->qtd; rx->i++)
                printf("%02X ", rx->dados[rx->i]);

            printf("\n");
        }
    }

    PT_END(&rx->pt);
}


/* ============================================================
   MAIN - escalonador cooperativo
   ============================================================ */

int main(void)
{
    static const unsigned char dados1[] = { 0x10, 0x20, 0x30 };
    static const unsigned char dados2[] = { 0xAA, 0xBB };

    static const Mensagem msgs[] =
    {
        { dados1, sizeof(dados1) },
        { dados2, sizeof(dados2) }
    };

    Canal canal;
    Transmissor tx;
    Receptor rx;

    int tx_ativo = 1;
    int rx_ativo = 1;

    canal.byte = 0;
    canal.cheio = 0;
    canal.total = 0;

    /* O frame 1 ocupa os bytes 0..6 do canal; o frame 2 comeca no 7.
       Corrompe o byte 9 (primeiro dado do frame 2) para testar o CHK. */
    canal.byte_a_corromper = 9;

    tx.canal = &canal;
    tx.msgs = msgs;
    tx.n_msgs = sizeof(msgs) / sizeof(msgs[0]);
    PT_INIT(&tx.pt);

    rx.canal = &canal;
    rx.frames_esperados = tx.n_msgs;
    PT_INIT(&rx.pt);

    printf("Transmissor e Receptor (protothreads):\n\n");

    /*
     * Cada protothread so deve ser chamada enquanto estiver ativa:
     * depois de PT_END ela se reinicializa, e chama-la de novo a
     * faria recomecar do zero.
     */
    while (tx_ativo || rx_ativo)
    {
        if (tx_ativo)
            tx_ativo = PT_SCHEDULE(tx_thread(&tx));

        if (rx_ativo)
            rx_ativo = PT_SCHEDULE(rx_thread(&rx));

        /* Transmissor terminou, nao ha mais bytes e o receptor ainda
           espera: frame truncado, nada mais vai chegar. */
        if (!tx_ativo && rx_ativo && !canal.cheio)
        {
            printf("Frame incompleto: receptor abortado\n");
            break;
        }
    }

    return 0;
}
