#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "string.h"
#include "pt.h"

#define MAX_BUFFER 32
#define STX 0x02
#define ETX 0x03

static uint8_t buffer[MAX_BUFFER];
static uint8_t ACK = 0;
static uint8_t PCT_REC = 0;

void envia_pacote(void)
{
    uint8_t success[] = {STX, 6, 0, 4, 1, 2, 1, 2, 4, ETX};

    uint8_t *dados = success;   //

    ACK = 0;
    memcpy(buffer, dados, sizeof(success));
    PCT_REC = 1;
}

#define MAX_TENT 2

PT_THREAD(sender(struct pt *pt))
{
    static int tentativas = 0;

    PT_BEGIN(pt);

    do {
        printf("TX\n\r");

        envia_pacote();

        PT_WAIT_UNTIL(pt, ACK == 1 || PCT_REC == 0);
    } while (ACK == 0 && ++tentativas < MAX_TENT);

    PT_END(pt);
}

uint8_t processa_pacote(uint8_t* dados_recebidos, uint8_t* buffer)
{
    uint8_t i = 0, chk = 0, cnt = 0;

    if(buffer[0] == STX)
    {
        cnt = buffer[1];
        while(i < cnt)
        {
            dados_recebidos[i] = buffer[2+i];
            chk ^= dados_recebidos[i];
            i++;
        }

        if(chk == buffer[2+cnt] && buffer[2+cnt+1] == ETX)
        {
            return 1;
        }
    }

    return 0;
}

PT_THREAD(receiver(struct pt *pt))
{
    static uint8_t resultado_esperado[] = {0, 4, 1, 2, 1, 2};
    static uint8_t dados_recebidos[MAX_BUFFER];

    PT_BEGIN(pt);

    PT_WAIT_UNTIL(pt, PCT_REC == 1);

    printf("RX\n\r");

    ACK = processa_pacote(dados_recebidos, buffer);

    PCT_REC = 0;

    if (ACK == 1)
    {
        if (memcmp(resultado_esperado, dados_recebidos, sizeof(resultado_esperado)) == 0)
        {
           printf("Teste: BEM-SUCEDIDO\n");
        }
    }
    else
    {
        printf("Teste: MAL-SUCEDIDO\n");
    }

    PT_END(pt);
}

int main()
{
    struct pt pt_tx;
    struct pt pt_rx;
    int exec = 0;

    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);

    do
    {
        exec = (receiver(&pt_rx) < PT_ENDED) | (sender(&pt_tx) < PT_ENDED);
    } while(exec);

    return 0;
}
