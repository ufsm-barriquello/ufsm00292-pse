#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "string.h"

#define MAX_BUFFER 100

#define STX 0x02
#define ETX 0x03

uint8_t pacote_recebido[MAX_BUFFER];
int pacote_size = 0;



typedef enum {
    ST_STX = 0, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} States;

typedef void (*Action) (unsigned char pacote);

struct StateMachine {
    States state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chkBuffer;
    int indBuffer;
    int qtdBuffer;
    Action action[5];
} sm;


void handlePackage(unsigned char *pacote, int tamanho);


void stSTX(unsigned char pacote)
{
    if (pacote == STX) {
        sm.indBuffer = sm.qtdBuffer = 0;
        sm.chkBuffer = 0;
        sm.state = ST_QTD;
    }
}

void stQtd(unsigned char pacote)
{
    sm.qtdBuffer = pacote;
    sm.state = ST_DATA;
}

void stData(unsigned char pacote)
{
    sm.buffer[sm.indBuffer++] = pacote;
    sm.chkBuffer ^= pacote;
    if (--sm.qtdBuffer == 0) {
        sm.state = ST_CHK;
    }
}

void stChk(unsigned char pacote)
{
    if (pacote == sm.chkBuffer) {
        sm.state = ST_ETX;
    }
    else {

        sm.state = ST_STX;
        sm.indBuffer = 0;
    }
}

void stETX(unsigned char pacote)
{
    if (pacote == ETX) {
        handlePackage(sm.buffer, sm.indBuffer);
    }
    sm.state = ST_STX;
}


void handlePackage(unsigned char *pacote, int tamanho)
{
    pacote_size = tamanho;
    for (int i = 0; i < tamanho; i++)
        pacote_recebido[i] = pacote[i];
}


void handleRx(unsigned char *data, int qtd)
{
    for (int i = 0; i < qtd; i++)
    {
        sm.action[sm.state](data[i]);
    }
}


void iniciaMaquina(void)
{
    sm.state = ST_STX;
    sm.buffer[0] = 0;
    sm.chkBuffer = 0;
    sm.indBuffer = 0;
    sm.qtdBuffer = 0;
    sm.action[ST_STX] = stSTX;
    sm.action[ST_QTD] = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK] = stChk;
    sm.action[ST_ETX] = stETX;
}


void verificaConteudoPacote(uint8_t pacote1[],int t1,uint8_t pacote2[], int t2, uint8_t conteudo[], int t3)
{
    pacote_size = 0;
    iniciaMaquina();

    handleRx(pacote1, t1);
    handleRx(pacote2, t2);

   if(pacote_size == t3 &&
       memcmp(conteudo, pacote_recebido,t3)==0)
    {
        printf("Teste: BEM-SUCEDIDO\n");
    } else {
        printf("Teste: MAL-SUCEDIDO\n");
    }

}

int main()
{
    uint8_t p1[] = {STX, 6, 0, 4, 1, 2, 1};
    uint8_t p2[] = {2, 4, ETX};
    uint8_t c[] = {0, 4, 1, 2, 1, 2};

    verificaConteudoPacote(p1,
                           sizeof(p1),
                           p2,
                           sizeof(p2),
                           c,
                           sizeof(c));

    return 0;
}
