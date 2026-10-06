#include <stdio.h>
#include <stdlib.h>
#include "protocolo_fsm.h"

#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

static char * teste_ignora_ruido_antes_do_stx(void) {
    initSM();
    unsigned char ruido[] = { 0xFF, 0x1A, 0x00 };
    handleRx(ruido, 3);
    verifica("Erro: Maquina desviou do estado inicial", sm.state == ST_STX);
    return 0;
}

static char * teste_recebe_pacote_completo_valido(void) {
    initSM();
    unsigned char pacote[] = { 0x02, 0x02, 0x10, 0x20, 0x30, 0x03 };
    handleRx(pacote, 6);
    verifica("Erro: Pacote valido rejeitado", pacote_valido == 1);
    verifica("Erro: Dados corrompidos", sm.buffer[0] == 0x10 && sm.buffer[1] == 0x20);
    return 0;
}

static char * teste_rejeita_checksum_invalido(void) {
    initSM();
    unsigned char pacote_invalido[] = { 0x02, 0x02, 0x10, 0x20, 0x99, 0x03 };
    handleRx(pacote_invalido, 6);
    verifica("Erro: Pacote invalido foi aceito", pacote_valido == 0);
    return 0;
}

static char * executa_testes(void) {
    executa_teste(teste_ignora_ruido_antes_do_stx);
    executa_teste(teste_recebe_pacote_completo_valido);
    executa_teste(teste_rejeita_checksum_invalido);
    return 0;
}

int main() {
    char *resultado = executa_testes();
    if (resultado != 0) {
        printf("%s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);
    return resultado != 0;
}
