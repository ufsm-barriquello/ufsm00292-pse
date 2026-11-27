#include <stdio.h>

#define STX 0x02
#define ETX 0x03

#define MAX_DADOS 255

/* Estados do transmissor */
enum estado_tx {
    TX_STX,
    TX_QTD_DADOS,
    TX_DADOS,
    TX_CHK,
    TX_ETX
};

/* Estados do receptor */
enum estado_rx {
    RX_STX,
    RX_QTD_DADOS,
    RX_DADOS,
    RX_CHK,
    RX_ETX
};


/* Calcula o checksum */
unsigned char checksum(
    unsigned char *dados,
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

void transmitir(
    unsigned char *dados,
    unsigned char qtd)
{
    enum estado_tx estado = TX_STX;

    unsigned char i = 0;
    unsigned char chk;

    chk = checksum(dados, qtd);

    while (1)
    {
        switch (estado)
        {
            case TX_STX:

                printf("%02X ", STX);

                estado = TX_QTD_DADOS;

                break;


            case TX_QTD_DADOS:

                printf("%02X ", qtd);

                estado = TX_DADOS;

                break;


            case TX_DADOS:

                if (i < qtd)
                {
                    printf("%02X ", dados[i]);

                    i++;
                }
                else
                {
                    estado = TX_CHK;
                }

                break;


            case TX_CHK:

                printf("%02X ", chk);

                estado = TX_ETX;

                break;


            case TX_ETX:

                printf("%02X\n", ETX);

                return;
        }
    }
}


/* ============================================================
   RECEPTOR
   ============================================================ */

void receber(
    unsigned char *frame,
    unsigned int tamanho)
{
    enum estado_rx estado = RX_STX;

    unsigned int i = 0;
    unsigned char qtd;
    unsigned char dados[MAX_DADOS];
    unsigned char chk;
    unsigned char chk_calculado = 0;

    while (i < tamanho)
    {
        switch (estado)
        {
            case RX_STX:

                if (frame[i] == STX)
                {
                    estado = RX_QTD_DADOS;
                }

                i++;

                break;


            case RX_QTD_DADOS:

                qtd = frame[i];

                i = i + 1;

                estado = RX_DADOS;

                break;


            case RX_DADOS:

                if (i < (unsigned int)(qtd + 2))
                {
                    dados[i - 2] = frame[i];

                    chk_calculado ^= frame[i];

                    i++;
                }
                else
                {
                    estado = RX_CHK;
                }

                break;


            case RX_CHK:

                chk = frame[i];

                i++;

                if (chk == chk_calculado)
                {
                    estado = RX_ETX;
                }
                else
                {
                    printf("Checksum invalido\n");

                    return;
                }

                break;


            case RX_ETX:

                if (frame[i] == ETX)
                {
                    printf("Frame recebido:\n");

                    for (unsigned int j = 0; j < qtd; j++)
                    {
                        printf("%02X ", dados[j]);
                    }

                    printf("\n");
                }
                else
                {
                    printf("ETX invalido\n");
                }

                return;
        }
    }
}


/* ============================================================
   MAIN
   ============================================================ */

int main(void)
{
    unsigned char dados[] = {
        0x10,
        0x20,
        0x30
    };

    unsigned char frame[] = {
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