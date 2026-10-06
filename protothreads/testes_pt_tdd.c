#include <stdio.h>
#include "protothread_protocolo.h"

#define verifica(msg, teste) do { if (!(teste)) return msg; } while (0)
int testes_executados = 0;

/* Teste 1: Caminho Feliz */
static char * teste_transmissao_cooperativa_sucesso(void) {
    reset_sistema();
    payload_tx[0] = 0xAA; payload_tx[1] = 0xBB;
    qtd_tx = 2;

    while(!tx_concluido || tem_dado_no_canal) {
        thread_transmissora(&pt_tx);
        thread_receptora(&pt_rx);
    }
    
    verifica("Erro: O pacote nao foi recebido com sucesso", pacote_recebido_com_sucesso == 1);
    verifica("Erro: Os dados decodificados estao incorretos", buffer_rx[0] == 0xAA && buffer_rx[1] == 0xBB);
    return 0;
}

/* Teste 2: Falha de Segurança (Checksum) */
static char * teste_rejeita_checksum_invalido(void) {
    reset_sistema();
    unsigned char pacote_corrompido[] = { STX, 0x01, 0xAA, 0xFF /* chk errado */, ETX };
    
    for(int i = 0; i < 5; i++) {
        canal_comunicacao = pacote_corrompido[i];
        tem_dado_no_canal = 1;
        thread_receptora(&pt_rx);
    }
    verifica("Erro: Pacote com Checksum corrompido foi aceito", pacote_recebido_com_sucesso == 0);
    return 0;
}

/* Teste 3: Lixo na linha antes do STX */
static char * teste_ignora_ruido_antes_do_stx(void) {
    reset_sistema();
    unsigned char ruido[] = { 0xFF, 0x1A, 0x00 };
    
    for(int i = 0; i < 3; i++) {
        canal_comunicacao = ruido[i];
        tem_dado_no_canal = 1;
        thread_receptora(&pt_rx);
    }
    verifica("Erro: Decodificador validou ruido como pacote", pacote_recebido_com_sucesso == 0);
    return 0;
}

int main() {
    char *resultado;

    resultado = teste_transmissao_cooperativa_sucesso();
    if(resultado) { printf("%s\n", resultado); return 1; }
    testes_executados++;

    resultado = teste_rejeita_checksum_invalido();
    if(resultado) { printf("%s\n", resultado); return 1; }
    testes_executados++;

    resultado = teste_ignora_ruido_antes_do_stx();
    if(resultado) { printf("%s\n", resultado); return 1; }
    testes_executados++;

    printf("TODOS OS TESTES PASSARAM\n");
    printf("Testes executados: %d\n", testes_executados);
    return 0;
}