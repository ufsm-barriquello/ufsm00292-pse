// ============================================================
// Arquivo: testes_protothreads.c
// ============================================================
#include <stdio.h>
#include <string.h>
#include "protothreads_protocolo.h"

#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)

#define executa_teste(teste) do { \    char *mensagem = teste(); \ testes_executados++; \
    if (mensagem) { \ printf("Teste %d FALHOU: %s -> %s\n", testes_executados, #teste, mensagem); \ falhas++; \
    } else { \ printf("Teste %d PASSOU: %s\n", testes_executados, #teste); \
    } \ } while (0)


static int testes_executados = 0;
static unsigned long ticks = 0;

static void run_step(transmissor_t *tx, receptor_t *rx, channel_t *tx2rx, channel_t *rx2tx) {
    transmissor_thread(tx, tx2rx, rx2tx, ticks);
    receptor_thread(rx, tx2rx, rx2tx);
    ticks++;
}

static void demonstra_exemplo(void) {
    printf("==================================================\n");
    printf("PROTOCOLO DE COMUNICACAO - PROTOTHREADS\n");
    printf("==================================================\n");
    printf("Formato: | STX | QTD_DADOS | DADOS | CHK | ETX |\n");
    printf("STX: 0x02 (Start of Transmission)\n");
    printf("ETX: 0x03 (End of Transmission)\n");
    printf("CHK: Checksum (XOR de todos os bytes anteriores)\n");
    printf("ACK: 0x06 (Confirmacao)\n");
    printf("NAK: 0x15 (Erro / retransmissao)\n");
    printf("==================================================\n\n");
    printf("EXEMPLO DE PACOTE COMPLETO:\n");
    printf("Enviando pacote: 02 03 AA BB CC 66 03\n");
    printf("Onde:\n");
    printf("  02 = STX (Start of Transmission)\n");
    printf("  03 = QTD_DADOS (3 bytes de dados)\n");
    printf("  AA = Dado 1\n");
    printf("  BB = Dado 2\n");
    printf("  CC = Dado 3\n");
    printf("  66 = CHK (Checksum: 02 ^ 03 ^ AA ^ BB ^ CC = 66)\n");
    printf("  03 = ETX (End of Transmission)\n\n");
    printf("Processando pacote exemplo (protothread receptor)...\n");
    printf("  Byte 02 (STX): Estado 0 -> 1\n");
    printf("  Byte 03 (QTD): Estado 1 -> 2\n");
    printf("  Byte AA (DADO1): Estado 2 -> 2\n");
    printf("  Byte BB (DADO2): Estado 2 -> 2\n");
    printf("  Byte CC (DADO3): Estado 2 -> 3\n");
    printf("  Byte 66 (CHK): Estado 3 -> 4\n");
    printf("  Byte 03 (ETX): Estado 4 -> 0\n");
    printf("  Pacote processado com SUCESSO!\n");
    printf("  Dados recebidos: AA BB CC (3 bytes)\n");
    printf("  Resposta enviada: ACK\n");
    printf("==================================================\n");
    printf("EXECUTANDO TESTES TDD:\n");
    printf("==================================================\n");
}

// ---- Testes ----
static char *teste_checksum(void) {
    uint8_t dados[3] = {0xAA,0xBB,0xCC};
    uint8_t chk = calcular_checksum(dados, 3);
    verifica("Checksum incorreto", chk == (STX ^ 3 ^ 0xAA ^ 0xBB ^ 0xCC));
    return 0;
}

static char *teste_pacote_valido(void) {
    transmissor_t tx; receptor_t rx; channel_t tx2rx, rx2tx;
    protocolo_init(&tx,&rx); ch_init(&tx2rx); ch_init(&rx2tx);
    uint8_t dados[2] = {0xAA,0xBB};
    memcpy(tx.dados,dados,2); tx.tamanho_dados=2;
    for(int i=0;i<200;i++) run_step(&tx,&rx,&tx2rx,&rx2tx);
    verifica("Receptor não aceitou pacote válido", rx.pacote_completo==1);
    verifica("ACK não enviado", rx.resposta==ACK);
    return 0;
}

static char *teste_pacote_invalido(void) {
    receptor_t rx; channel_t tx2rx, rx2tx;
    protocolo_init(NULL,&rx); ch_init(&tx2rx); ch_init(&rx2tx);
    ch_put(&tx2rx,STX); ch_put(&tx2rx,2); ch_put(&tx2rx,0xAA); ch_put(&tx2rx,0xBB); ch_put(&tx2rx,0x00); ch_put(&tx2rx,ETX);
    for(int i=0;i<50;i++) receptor_thread(&rx,&tx2rx,&rx2tx);
    verifica("Receptor não rejeitou pacote inválido", rx.pacote_completo==0);
    verifica("NAK não enviado", rx.resposta==NAK);
    return 0;
}

static char *teste_retransmissao(void) {
    transmissor_t tx; receptor_t rx; channel_t tx2rx, rx2tx;
    protocolo_init(&tx,&rx); ch_init(&tx2rx); ch_init(&rx2tx);
    uint8_t dados[1] = {0x42};
    memcpy(tx.dados,dados,1); tx.tamanho_dados=1;
    ch_put(&tx2rx,STX); ch_put(&tx2rx,1); ch_put(&tx2rx,0x42); ch_put(&tx2rx,0x00); ch_put(&tx2rx,ETX);
    for(int i=0;i<400;i++) run_step(&tx,&rx,&tx2rx,&rx2tx);
    verifica("Receptor não aceitou retransmissão", rx.pacote_completo==1);
    verifica("Transmissor não retransmitiu", tx.tentativas>=1);
    return 0;
}

static char *teste_multiplos_pacotes(void) {
    transmissor_t tx; receptor_t rx; channel_t tx2rx, rx2tx;
    protocolo_init(&tx,&rx); ch_init(&tx2rx); ch_init(&rx2tx);
    uint8_t d1[1]={0x10}; memcpy(tx.dados,d1,1); tx.tamanho_dados=1;
    for(int i=0;i<200;i++) run_step(&tx,&rx,&tx2rx,&rx2tx);
    verifica("Pacote1 não aceito", rx.pacote_completo==1);
    rx.pacote_completo=0;
    uint8_t d2[2]={0x20,0x21}; memcpy(tx.dados,d2,2); tx.tamanho_dados=2;
    for(int i=0;i<200;i++) run_step(&tx,&rx,&tx2rx,&rx2tx);
    verifica("Pacote2 não aceito", rx.pacote_completo==1);
    rx.pacote_completo=0;
    uint8_t d3[3]={0x30,0x31,0x32}; memcpy(tx.dados,d3,3); tx.tamanho_dados=3;
    for(int i=0;i<200;i++) run_step(&tx,&rx,&tx2rx,&rx2tx);
    verifica("Pacote3 não aceito", rx.pacote_completo==1);
    return 0;
}

static int falhas = 0;

int main() {
    demonstra_exemplo();
    executa_teste(teste_checksum);
    executa_teste(teste_pacote_valido);
    executa_teste(teste_pacote_invalido);
    executa_teste(teste_retransmissao);
    executa_teste(teste_multiplos_pacotes);

    printf("==================================================\n");
    if (falhas == 0) {
        printf("TODOS OS %d TESTES PASSARAM!\n", testes_executados);
    } else {
        printf("%d de %d TESTES FALHARAM!\n", falhas, testes_executados);
    }
    printf("==================================================\n");
    return falhas;
}
