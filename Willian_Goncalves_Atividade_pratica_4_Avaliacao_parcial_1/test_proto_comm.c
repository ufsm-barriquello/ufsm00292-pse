#include <stdio.h>
#include "proto_comm.h"

#define verifica(msg, teste) do { if (!(teste)) return msg; } while (0)
#define executa_teste(teste) do { char *m = teste(); testes_executados++; if (m) return m; } while (0)

int testes_executados = 0;

static char * teste_pt_comunicacao_sucesso(void) {
    CanalComunicacao canal;
    PT_Transmissor tx;
    PT_Receptor rx;

    uint8_t dados_envio[] = {0xAA, 0xBB, 0xCC};
    canal_init(&canal);
    pt_tx_init(&tx, dados_envio, 3);
    pt_rx_init(&rx);

    /* Executa o scheduler cooperativo dando tempo suficiente para a troca completa */
    for (int cycle = 0; cycle < 500; cycle++) {
        pt_transmissor_thread(&tx, &canal);
        pt_receptor_thread(&rx, &canal);

        if (tx.transmissao_concluida) break;
    }

    verifica("Erro T01: Transmissao nao foi concluida", tx.transmissao_concluida == true);
    verifica("Erro T01: ACK nao foi recebido pelo TX", tx.ack_recebido == true);
    verifica("Erro T01: RX nao validou o pacote", rx.pacote_valido == true);
    verifica("Erro T01: Dado 0 corrompido no RX", rx.buffer_recebido[0] == 0xAA);
    verifica("Erro T01: Dado 1 corrompido no RX", rx.buffer_recebido[1] == 0xBB);
    verifica("Erro T01: Dado 2 corrompido no RX", rx.buffer_recebido[2] == 0xCC);

    return 0;
}

static char * teste_pt_retransmissao_por_erro(void) {
    CanalComunicacao canal;
    PT_Transmissor tx;
    PT_Receptor rx;

    uint8_t dados_envio[] = {0x10, 0x20};
    canal_init(&canal);
    pt_tx_init(&tx, dados_envio, 2);
    pt_rx_init(&rx);

    /* Injeta erro no canal no primeiro envio */
    for (int cycle = 0; cycle < 500; cycle++) {
        pt_transmissor_thread(&tx, &canal);
        pt_receptor_thread(&rx, &canal);

        /* Quando o RX gera o primeiro ACK, nos corrompemos para NACK para forcar retransmissao */
        if (canal.novo_ack_disponivel && tx.retransmissoes == 0) {
            canal.canal_ack = NACK_BYTE;
        }

        if (tx.transmissao_concluida) break;
    }

    verifica("Erro T02: TX deveria ter realizado retransmissao", tx.retransmissoes > 0);
    return 0;
}

static char * executa_todos_testes(void) {
    executa_teste(teste_pt_comunicacao_sucesso);
    executa_teste(teste_pt_retransmissao_por_erro);
    return 0;
}

int main(void) {
    printf("===================================================\n");
    printf("  EXECUTANDO TESTES PROJETO 4 (PROTOTHREADS + TDD) \n");
    printf("===================================================\n");

    char *resultado = executa_todos_testes();

    if (resultado != 0) {
        printf("[FALHA] %s\n", resultado);
    } else {
        printf("[SUCESSO] TODOS OS TESTES PASSARAM COM SUCESSO!\n");
    }
    printf("Total de testes executados: %d\n", testes_executados);
    printf("===================================================\n");

    return (resultado != 0);
}