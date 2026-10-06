#include <stdio.h>
#include <stdlib.h>
#include "protocolo_fsm.h"

/* --- Infraestrutura TDD --- */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

/* --- Cenários de Teste --- */
static char * teste_ignora_lixo_antes_do_stx(void) {
    reset_fsm();
    unsigned char dados_lixo[] = { 0xFF, 0x00, 0x15 };
    handleRx(dados_lixo, 3);
    verifica("Erro: FSM nao deveria ter saido do estado ST_STX", state == ST_STX);
    return 0;
}

static char * teste_recebe_pacote_completo_valido(void) {
    reset_fsm();
    unsigned char pacote_valido_array[] = { 0x02, 0x02, 0x10, 0x20, 0x30, 0x03 };
    handleRx(pacote_valido_array, 6);
    verifica("Erro: Pacote valido nao foi reconhecido", pacote_valido == 1);
    verifica("Erro: Dados nao foram salvos corretamente", buffer[0] == 0x10 && buffer[1] == 0x20);
    return 0;
}

static char * teste_rejeita_checksum_invalido(void) {
    reset_fsm();
    unsigned char pacote_invalido[] = { 0x02, 0x02, 0x10, 0x20, 0x99, 0x03 };
    handleRx(pacote_invalido, 6);
    verifica("Erro: Pacote com checksum invalido foi aceito", pacote_valido == 0);
    return 0;
}

static char * executa_testes(void) {
    executa_teste(teste_ignora_lixo_antes_do_stx);
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
