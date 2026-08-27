#include <stdio.h>
#include "fsm_protocolo.h"

#define verifica(msg, teste) do { if (!(teste)) return msg; } while (0)
#define executa_teste(teste) do { char *m = teste(); testes_executados++; if (m) return m; } while (0)

int testes_executados = 0;

static char * teste_tx_geracao_pacote(void) {
    uint8_t dados[] = {0x11, 0x22};
    uint8_t pacote[10];
    size_t tam = tx_monta_pacote(dados, 2, pacote);

    verifica("Erro TX: Tamanho incorreto", tam == 6);
    verifica("Erro TX: STX incorreto", pacote[0] == 0x02);
    verifica("Erro TX: CHK incorreto", pacote[4] == 0x33);
    verifica("Erro TX: ETX incorreto", pacote[5] == 0x03);

    return 0;
}

static char * teste_fsm_tabela_recepcao_sucesso(void) {
    FSMDecoder fsm;
    fsm_init(&fsm);

    uint8_t pacote[] = {0x02, 0x02, 0x11, 0x22, 0x33, 0x03};
    bool ok = false;

    for (size_t i = 0; i < sizeof(pacote); i++) {
        ok = fsm_process_byte(&fsm, pacote[i]);
    }

    verifica("Erro RX: Pacote valido rejeitado pela Tabela/Ponteiro", ok == true);
    verifica("Erro RX: Estado final deve ser STATE_VALIDO", fsm.current_state == STATE_VALIDO);
    return 0;
}

static char * teste_fsm_tabela_checksum_errado(void) {
    FSMDecoder fsm;
    fsm_init(&fsm);

    uint8_t pacote_bad_chk[] = {0x02, 0x02, 0x11, 0x22, 0xFF, 0x03};
    bool ok = false;

    for (size_t i = 0; i < sizeof(pacote_bad_chk); i++) {
        ok = fsm_process_byte(&fsm, pacote_bad_chk[i]);
    }

    verifica("Erro RX: Pacote com CHK invalido foi aceito", ok == false);
    verifica("Erro RX: FSM nao voltou para STATE_STX", fsm.current_state == STATE_STX);
    return 0;
}

static char * executa_todos_testes(void) {
    executa_teste(teste_tx_geracao_pacote);
    executa_teste(teste_fsm_tabela_recepcao_sucesso);
    executa_teste(teste_fsm_tabela_checksum_errado);
    return 0;
}

int main(void) {
    printf("===================================================\n");
    printf("  EXECUTANDO TESTES PROJETO 3 (FSM TABELA/PONTEIROS)\n");
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