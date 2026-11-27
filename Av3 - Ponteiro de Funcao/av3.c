#include <stdio.h>

#define STX 0x02
#define ETX 0x03

#define MAX_DADOS 255

/* Estados do transmissor */
typedef enum
{
    TX_STX,
    TX_QTD_DADOS,
    TX_DADOS,
    TX_CHK,
    TX_ETX
} EstadoTX;

/* Estados do receptor */
typedef enum
{
    RX_STX,
    RX_QTD_DADOS,
    RX_DADOS,
    RX_CHK,
    RX_ETX
} EstadoRX;


/* ============================================================
   TRANSMISSOR
   ============================================================ */

typedef struct Transmissor Transmissor;

struct Transmissor
{
    EstadoTX estado;

    unsigned char *dados;
    unsigned char qtd;

    unsigned char i;
    unsigned char chk;

    void (*funcao_estado)(Transmissor *);
};


unsigned char calcular_checksum(
    unsigned char *dados,
    unsigned char qtd)
{
    unsigned char chk = 0;
    unsigned char i;

    for (i = 0; i < qtd; i++)
        chk ^= dados[i];

    return chk;
}


void tx_stx(Transmissor *tx);
void tx_qtd_dados(Transmissor *tx);
void tx_dados(Transmissor *tx);
void tx_chk(Transmissor *tx);
void tx_etx(Transmissor *tx);


void tx_stx(Transmissor *tx)
{
    printf("%02X ", STX);

    tx->estado = TX_QTD_DADOS;
    tx->funcao_estado = tx_qtd_dados;
}


void tx_qtd_dados(Transmissor *tx)
{
    printf("%02X ", tx->qtd);

    tx->estado = TX_DADOS;
    tx->funcao_estado = tx_dados;
}


void tx_dados(Transmissor *tx)
{
    if (tx->i < tx->qtd)
    {
        printf("%02X ", tx->dados[tx->i]);

        tx->i++;
    }
    else
    {
        tx->estado = TX_CHK;
        tx->funcao_estado = tx_chk;
    }
}


void tx_chk(Transmissor *tx)
{
    printf("%02X ", tx->chk);

    tx->estado = TX_ETX;
    tx->funcao_estado = tx_etx;
}


void tx_etx(Transmissor *tx)
{
    printf("%02X\n", ETX);

    tx->funcao_estado = NULL;
}


void transmitir(
    unsigned char *dados,
    unsigned char qtd)
{
    Transmissor tx;

    tx.estado = TX_STX;
    tx.dados = dados;
    tx.qtd = qtd;
    tx.i = 0;
    tx.chk = calcular_checksum(dados, qtd);

    tx.funcao_estado = tx_stx;

    while (tx.funcao_estado != NULL)
    {
        tx.funcao_estado(&tx);
    }
}


/* ============================================================
   RECEPTOR
   ============================================================ */

typedef struct Receptor Receptor;

struct Receptor
{
    EstadoRX estado;

    unsigned char *frame;
    unsigned int tamanho;

    unsigned int i;

    unsigned char qtd;
    unsigned char dados[MAX_DADOS];

    unsigned char chk;
    unsigned char chk_calculado;

    void (*funcao_estado)(Receptor *);
};


void rx_stx(Receptor *rx);
void rx_qtd_dados(Receptor *rx);
void rx_dados(Receptor *rx);
void rx_chk(Receptor *rx);
void rx_etx(Receptor *rx);


void rx_stx(Receptor *rx)
{
    if (rx->i >= rx->tamanho)
    {
        rx->funcao_estado = NULL;
        return;
    }

    if (rx->frame[rx->i] == STX)
    {
        rx->funcao_estado = rx_qtd_dados;
    }

    rx->i++;
}


void rx_qtd_dados(Receptor *rx)
{
    if (rx->i >= rx->tamanho)
    {
        rx->funcao_estado = NULL;
        return;
    }

    rx->qtd = rx->frame[rx->i];

    rx->i++;
    rx->chk_calculado = 0;

    rx->funcao_estado = rx_dados;
}


void rx_dados(Receptor *rx)
{
    if (rx->i >= rx->tamanho)
    {
        rx->funcao_estado = NULL;
        return;
    }

    if (rx->i < (unsigned int)(rx->qtd + 2))
    {
        rx->dados[rx->i - 2] = rx->frame[rx->i];

        rx->chk_calculado ^= rx->frame[rx->i];

        rx->i++;
    }
    else
    {
        rx->funcao_estado = rx_chk;
    }
}


void rx_chk(Receptor *rx)
{
    if (rx->i >= rx->tamanho)
    {
        rx->funcao_estado = NULL;
        return;
    }

    rx->chk = rx->frame[rx->i];

    rx->i++;

    if (rx->chk == rx->chk_calculado)
    {
        rx->funcao_estado = rx_etx;
    }
    else
    {
        printf("Checksum invalido\n");

        rx->funcao_estado = NULL;
    }
}


void rx_etx(Receptor *rx)
{
    if (rx->i >= rx->tamanho)
    {
        rx->funcao_estado = NULL;
        return;
    }

    if (rx->frame[rx->i] == ETX)
    {
        unsigned char j;

        printf("Frame recebido:\n");

        for (j = 0; j < rx->qtd; j++)
        {
            printf("%02X ", rx->dados[j]);
        }

        printf("\n");
    }
    else
    {
        printf("ETX invalido\n");
    }

    rx->funcao_estado = NULL;
}


void receber(
    unsigned char *frame,
    unsigned int tamanho)
{
    Receptor rx;

    rx.estado = RX_STX;
    rx.frame = frame;
    rx.tamanho = tamanho;
    rx.i = 0;
    rx.qtd = 0;
    rx.chk = 0;
    rx.chk_calculado = 0;

    rx.funcao_estado = rx_stx;

    while (rx.funcao_estado != NULL)
    {
        rx.funcao_estado(&rx);
    }
}


/* ============================================================
   MAIN
   ============================================================ */

int main(void)
{
    unsigned char dados[] =
    {
        0x10,
        0x20,
        0x30
    };

    unsigned char frame[] =
    {
        STX,
        0x03,
        0x10,
        0x20,
        0x30,
        0x00,
        ETX
    };

    printf("Transmissor:\n");

    transmitir(
        dados,
        3
    );

    printf("Receptor:\n");

    receber(
        frame,
        sizeof(frame)
    );

    return 0;
}