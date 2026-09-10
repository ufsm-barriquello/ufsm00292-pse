#include <stdio.h>
#include <stdlib.h>
#include "fsm_tbl_protocolo.h"

// Framework minimalista de assercoes de TDD recomendado nas aulas praticas da UFSM
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                  if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

// Teste 1: Verificar se a tabela de ponteiros de funcao e inicializada corretamente
static char * teste_fsm_tbl_inicializacao(void) {
    fsm_tbl_init();

    verifica("Erro: Estado inicial deveria ser ST_STX", sm.state == ST_STX);
    verifica("Erro: Indice de buffer deveria ser 0", sm.indBuffer == 0);
    verifica("Erro: Checksum acumulado deveria ser 0", sm.chkBuffer == 0);
    verifica("Erro: Flag pacote_processado deveria ser false", sm.pacote_processado == false);
    verifica("Erro: Ponteiro de funcao ST_STX nao foi mapeado", sm.action[ST_STX] != NULL);
    verifica("Erro: Ponteiro de funcao ST_QTD nao foi mapeado", sm.action[ST_QTD] != NULL);
    verifica("Erro: Ponteiro de funcao ST_DATA nao foi mapeado", sm.action[ST_DATA] != NULL);
    verifica("Erro: Ponteiro de funcao ST_CHK nao foi mapeado", sm.action[ST_CHK] != NULL);
    verifica("Erro: Ponteiro de funcao ST_ETX nao foi mapeado", sm.action[ST_ETX] != NULL);
    return 0;
}

// Teste 2: Processar um pacote valido via ponteiros de funcao
static char * teste_fsm_tbl_pacote_valido(void) {
    fsm_tbl_init();

    // Dados: [15, 30, 45]. Checksum = 15 ^ 30 ^ 45 = 32
    uint8_t chk_calculado = 15 ^ 30 ^ 45;
    uint8_t pacote[] = { STX, 3, 15, 30, 45, chk_calculado, ETX };

    fsm_tbl_process_bytes(pacote, sizeof(pacote));

    verifica("Erro: Pacote valido deveria ter sido marcado como processado", sm.pacote_processado == true);
    verifica("Erro: FSM deveria ter retornado ao estado ST_STX ao final", sm.state == ST_STX);
    verifica("Erro: Quantidade de dados no buffer deveria ser 3", sm.indBuffer == 3);
    verifica("Erro: Dado 0 incorreto (esperado: 15)", sm.buffer[0] == 15);
    verifica("Erro: Dado 1 incorreto (esperado: 30)", sm.buffer[1] == 30);
    verifica("Erro: Dado 2 incorreto (esperado: 45)", sm.buffer[2] == 45);
    return 0;
}

// Teste 3: Ignorar caracteres invalidos (lixo) antes do inicio do pacote (STX)
static char * teste_fsm_tbl_ignorar_lixo(void) {
    fsm_tbl_init();

    // Envia ruidos no canal serial antes do STX
    fsm_tbl_process_char(0xFF);
    fsm_tbl_process_char(0x00);
    fsm_tbl_process_char(0xAB);

    verifica("Erro: FSM deveria permanecer em ST_STX ao receber lixo", sm.state == ST_STX);
    verifica("Erro: Buffer nao deveria acumular lixo", sm.indBuffer == 0);

    // Envia pacote valido logo em seguida
    uint8_t chk = 99;
    uint8_t pacote[] = { STX, 1, 99, chk, ETX };
    fsm_tbl_process_bytes(pacote, sizeof(pacote));

    verifica("Erro: FSM deveria processar o pacote corretamente apos o lixo", sm.pacote_processado == true);
    verifica("Erro: Dado extraido invalido", sm.buffer[0] == 99);
    return 0;
}

// Teste 4: Detectar e rejeitar pacotes com Checksum corrompido
static char * teste_fsm_tbl_falha_checksum(void) {
    fsm_tbl_init();

    // Checksum correto seria 10^20 = 30, mas enviaremos 0x99 (errado)
    uint8_t pacote[] = { STX, 2, 10, 20, 0x99, ETX };

    fsm_tbl_process_bytes(pacote, sizeof(pacote));

    verifica("Erro: Pacote com checksum invalido NAO deveria ser processado", sm.pacote_processado == false);
    verifica("Erro: FSM deveria ter resetado para ST_STX", sm.state == ST_STX);
    return 0;
}

// Teste 5: Rejeitar pacotes com byte de fechamento ETX invalido
static char * teste_fsm_tbl_falha_etx(void) {
    fsm_tbl_init();

    uint8_t chk = 77;
    // Substitui ETX (0x03) por 0xEE
    uint8_t pacote[] = { STX, 1, 77, chk, 0xEE };

    fsm_tbl_process_bytes(pacote, sizeof(pacote));

    verifica("Erro: Pacote com ETX invalido NAO deveria ser processado", sm.pacote_processado == false);
    verifica("Erro: FSM deveria retornar ao estado ST_STX", sm.state == ST_STX);
    return 0;
}

// Teste 6: Processar multiplos pacotes sequenciais sem interferencia
static char * teste_fsm_tbl_multiplos_pacotes(void) {
    fsm_tbl_init();

    // Pacote 1
    uint8_t p1[] = { STX, 1, 0xAA, 0xAA, ETX };
    fsm_tbl_process_bytes(p1, sizeof(p1));
    verifica("Erro no Pacote 1: Deveria ser processado", sm.pacote_processado == true);
    verifica("Erro no Pacote 1: Dado incorreto", sm.buffer[0] == 0xAA);

    // Pacote 2 enviado em seguida
    uint8_t chk2 = 0x11 ^ 0x22;
    uint8_t p2[] = { STX, 2, 0x11, 0x22, chk2, ETX };
    fsm_tbl_process_bytes(p2, sizeof(p2));
    verifica("Erro no Pacote 2: Deveria ser processado", sm.pacote_processado == true);
    verifica("Erro no Pacote 2: Indice de buffer incorreto", sm.indBuffer == 2);
    verifica("Erro no Pacote 2: Dado 0 incorreto", sm.buffer[0] == 0x11);
    verifica("Erro no Pacote 2: Dado 1 incorreto", sm.buffer[1] == 0x22);

    return 0;
}

// Agrupador de execucao dos testes unitarios
static char * executa_todos_testes(void) {
    executa_teste(teste_fsm_tbl_inicializacao);
    executa_teste(teste_fsm_tbl_pacote_valido);
    executa_teste(teste_fsm_tbl_ignorar_lixo);
    executa_teste(teste_fsm_tbl_falha_checksum);
    executa_teste(teste_fsm_tbl_falha_etx);
    executa_teste(teste_fsm_tbl_multiplos_pacotes);
    return 0;
}

int main() {
    char *resultado = executa_todos_testes();
    printf("\n========================================================\n");
    printf("  RESULTADO DA SUITE DE TESTES - TAREFA-03 (FSM TABELA)\n");
    printf("========================================================\n");
    if (resultado != 0) {
        printf("FALHA DETECTADA: %s\n", resultado);
    } else {
        printf("SENSACIONAL! TODOS OS TESTES PASSARAM COM SUCESSO!\n");
        printf("Mensagem para Print da Entrega: [TDD FSM FUNCTION POINTERS SUCCESS]\n");
    }
    printf("Total de testes unitarios executados: %d\n", testes_executados);
    printf("========================================================\n\n");
    return resultado != 0;
}
