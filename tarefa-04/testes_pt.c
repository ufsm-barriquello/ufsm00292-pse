#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pt_protocolo.h"

#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                  if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

// Teste 1: Validação direta do Codificador de Quadros
static char * teste_codificador_pacote(void) {
    uint8_t dados[] = { 10, 20, 30 };
    uint8_t frame[MAX_FRAME_SIZE];

    int len = codificar_pacote(dados, 3, frame);

    verifica("Erro: Tamanho do quadro codificado incorreto (esperado 7)", len == 7);
    verifica("Erro: Delimitador STX incorreto no inicio do quadro", frame[0] == STX);
    verifica("Erro: Campo QTD incorreto", frame[1] == 3);
    verifica("Erro: Dado 0 incorreto no quadro", frame[2] == 10);
    verifica("Erro: Dado 1 incorreto no quadro", frame[3] == 20);
    verifica("Erro: Dado 2 incorreto no quadro", frame[4] == 30);
    verifica("Erro: Checksum XOR incorreto", frame[5] == (10 ^ 20 ^ 30));
    verifica("Erro: Delimitador ETX incorreto no fim do quadro", frame[6] == ETX);
    return 0;
}

// Teste 2: Transmissao e Recepcao com Protothreads e ACK
static char * teste_protothreads_sucesso_completo(void) {
    CanalComunicacao tx_to_rx, rx_to_tx;
    canal_init(&tx_to_rx);
    canal_init(&rx_to_tx);

    uint8_t dados[] = { 0xAA, 0xBB, 0xCC, 0xDD };
    TransmissorPT tx;
    ReceptorPT rx;

    tx_init(&tx, dados, 4, 10);
    rx_init(&rx);

    int ciclos = 0;
    while (!tx.concluido && ciclos < 100) {
        pt_transmissora_run(&tx, &tx_to_rx, &rx_to_tx);
        pt_receptora_run(&rx, &tx_to_rx, &rx_to_tx);
        ciclos++;
    }

    verifica("Erro: Transmissor deveria ter marcado concluido = true", tx.concluido == true);
    verifica("Erro: Transmissor deveria ter recebido ACK", tx.ack_recebido == true);
    verifica("Erro: Receptor deveria ter validado o pacote", rx.pacote_valido == true);
    verifica("Erro: Contador de pacotes recebidos deveria ser 1", rx.pacotes_recebidos_count == 1);
    verifica("Erro: Quantidade de dados extraida incorreta", rx.ind_rx == 4);
    verifica("Erro: Dado 0 extraido incorreto", rx.buffer_rx[0] == 0xAA);
    verifica("Erro: Dado 1 extraido incorreto", rx.buffer_rx[1] == 0xBB);
    verifica("Erro: Dado 2 extraido incorreto", rx.buffer_rx[2] == 0xCC);
    verifica("Erro: Dado 3 extraido incorreto", rx.buffer_rx[3] == 0xDD);
    return 0;
}

// Teste 3: Retransmissao por Timeout devido a perda de ACK
static char * teste_retransmissao_por_timeout(void) {
    CanalComunicacao tx_to_rx, rx_to_tx;
    canal_init(&tx_to_rx);
    canal_init(&rx_to_tx);

    uint8_t dados[] = { 42 };
    TransmissorPT tx;
    ReceptorPT rx;

    tx_init(&tx, dados, 1, 5); // Timeout em 5 ciclos
    rx_init(&rx);

    // Passo 1: Transmissor envia o quadro para o canal
    while (tx.tx_index < tx.frame_len) {
        pt_transmissora_run(&tx, &tx_to_rx, &rx_to_tx);
    }

    // Passo 2: Receptor processa e envia ACK para o canal rx_to_tx
    pt_receptora_run(&rx, &tx_to_rx, &rx_to_tx);
    verifica("Erro: Receptor deveria ter validado o pacote", rx.pacote_valido == true);

    // Efeito de FALHA DE CANAL: Esvaziamos o canal rx_to_tx simulando perda do ACK
    uint8_t lixo;
    while (canal_read(&rx_to_tx, &lixo));

    // Passo 3: Transmissor avanca o tempo e detecta timeout, forcando retransmissao
    int retrans_inicial = tx.retentativas;
    for (int i = 0; i < 10; i++) {
        pt_transmissora_run(&tx, &tx_to_rx, &rx_to_tx);
    }
    verifica("Erro: Transmissor deveria ter incrementado retentativas", tx.retentativas > retrans_inicial);

    // Passo 4: O receptor re-processa o pacote retransmittedo e envia ACK novamente
    pt_receptora_run(&rx, &tx_to_rx, &rx_to_tx);

    // Transmissor finalmente recebe o ACK
    while (!tx.concluido && !tx.erro_timeout) {
        pt_transmissora_run(&tx, &tx_to_rx, &rx_to_tx);
    }

    verifica("Erro: Transmissor deveria ter concluido com sucesso apos retransmissao", tx.concluido == true);
    return 0;
}

// Teste 4: Estouro de Retentativas (Erro de Timeout definitivo)
static char * teste_estouro_retentativas(void) {
    CanalComunicacao tx_to_rx, rx_to_tx;
    canal_init(&tx_to_rx);
    canal_init(&rx_to_tx);

    uint8_t dados[] = { 1, 2, 3 };
    TransmissorPT tx;
    tx_init(&tx, dados, 3, 3); // Limit limit curto = 3

    // Executa a protothread do transmissor sem receptora para simular canal morto
    for (int i = 0; i < 100; i++) {
        pt_transmissora_run(&tx, &tx_to_rx, &rx_to_tx);
        // Descarta bytes gerados no canal tx_to_rx
        uint8_t dummy;
        while (canal_read(&tx_to_rx, &dummy));
        if (tx.erro_timeout) break;
    }

    verifica("Erro: Transmissor deveria ter disparado erro_timeout = true", tx.erro_timeout == true);
    verifica("Erro: Transmissor deveria ter atingido o maximo de retentativas", tx.retentativas >= MAX_RETENTATIVAS);
    verifica("Erro: Transmissor NAO deveria estar marcado como concluido", tx.concluido == false);
    return 0;
}

// Suite completa de testes TDD da Tarefa-04
static char * executa_todos_testes(void) {
    executa_teste(teste_codificador_pacote);
    executa_teste(teste_protothreads_sucesso_completo);
    executa_teste(teste_retransmissao_por_timeout);
    executa_teste(teste_estouro_retentativas);
    return 0;
}

int main() {
    char *resultado = executa_todos_testes();
    printf("\n========================================================\n");
    printf("  RESULTADO DA SUITE DE TESTES - TAREFA-04 (PROTOTHREADS)\n");
    printf("========================================================\n");
    if (resultado != 0) {
        printf("FALHA DETECTADA: %s\n", resultado);
    } else {
        printf("SENSACIONAL! TODOS OS TESTES PASSARAM COM SUCESSO!\n");
        printf("Mensagem para Print da Entrega: [TDD PROTOTHREADS SUCCESS]\n");
    }
    printf("Total de testes unitarios executados: %d\n", testes_executados);
    printf("========================================================\n\n");
    return resultado != 0;
}
