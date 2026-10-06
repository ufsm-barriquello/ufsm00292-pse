#include "protothread_protocolo.h"

/* Instanciando as variáveis globais */
unsigned char canal_comunicacao;
int tem_dado_no_canal = 0;
int tx_concluido = 0;
int pacote_recebido_com_sucesso = 0;

struct pt pt_tx, pt_rx;
unsigned char payload_tx[256], buffer_rx[256];
int qtd_tx = 0, qtd_rx = 0;
int idx_tx = 0, idx_rx = 0;
unsigned char chk_tx = 0, chk_rx = 0;

void reset_sistema(void) {
    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);
    tem_dado_no_canal = tx_concluido = pacote_recebido_com_sucesso = 0;
}

/* Transmissora (Codificador)[cite: 6] */
int thread_transmissora(struct pt *pt) {
    PT_BEGIN(pt);

    PT_WAIT_UNTIL(pt, tem_dado_no_canal == 0);
    canal_comunicacao = STX; tem_dado_no_canal = 1;

    PT_WAIT_UNTIL(pt, tem_dado_no_canal == 0);
    canal_comunicacao = qtd_tx; tem_dado_no_canal = 1;

    chk_tx = 0;
    for(idx_tx = 0; idx_tx < qtd_tx; idx_tx++) {
        PT_WAIT_UNTIL(pt, tem_dado_no_canal == 0);
        canal_comunicacao = payload_tx[idx_tx];
        chk_tx ^= payload_tx[idx_tx];
        tem_dado_no_canal = 1;
    }

    PT_WAIT_UNTIL(pt, tem_dado_no_canal == 0);
    canal_comunicacao = chk_tx; tem_dado_no_canal = 1;

    PT_WAIT_UNTIL(pt, tem_dado_no_canal == 0);
    canal_comunicacao = ETX; tem_dado_no_canal = 1;

    tx_concluido = 1;
    PT_END(pt);
}

/* Receptora (Decodificador)[cite: 6] */
int thread_receptora(struct pt *pt) {
    PT_BEGIN(pt);

    PT_WAIT_UNTIL(pt, tem_dado_no_canal == 1);
    if(canal_comunicacao == STX) {
        tem_dado_no_canal = 0;

        PT_WAIT_UNTIL(pt, tem_dado_no_canal == 1);
        qtd_rx = canal_comunicacao; 
        tem_dado_no_canal = 0;
        chk_rx = 0;

        for(idx_rx = 0; idx_rx < qtd_rx; idx_rx++) {
            PT_WAIT_UNTIL(pt, tem_dado_no_canal == 1);
            buffer_rx[idx_rx] = canal_comunicacao;
            chk_rx ^= canal_comunicacao;
            tem_dado_no_canal = 0;
        }

        PT_WAIT_UNTIL(pt, tem_dado_no_canal == 1);
        if(canal_comunicacao == chk_rx) {
            tem_dado_no_canal = 0;

            PT_WAIT_UNTIL(pt, tem_dado_no_canal == 1);
            if(canal_comunicacao == ETX) {
                pacote_recebido_com_sucesso = 1; /* Sucesso: pacote formatado corretamente[cite: 6] */
            }
        }
    }
    tem_dado_no_canal = 0;
    PT_END(pt);
}