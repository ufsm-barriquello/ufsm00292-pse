#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "fsm_protocolo.h"

// Macros de asserção oficiais recomendadas nas aulas práticas da UFSM
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                  if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

// Teste 1: Garantir que a inicialização da FSM está com parâmetros corretos e no estado ST_STX
static char * teste_fsm_inicializacao(void) {
    FsmProtocolo fsm;
    fsm_init(&fsm);

    verifica("Erro: A FSM recem-inicializada deveria estar no estado ST_STX", fsm.state == ST_STX);
    verifica("Erro: O indice do buffer deveria ser zero", fsm.indBuffer == 0);
    verifica("Erro: O buffer de checksum acumulado deveria ser zero", fsm.chkBuffer == 0);
    verifica("Erro: Flag pacote_processado nao deveria estar ativo", fsm.pacote_processado == false);
    return 0;
}

// Teste 2: Alimenta a FSM com um pacote válido completo de 3 dados úteis e valida o resultado
static char * teste_fsm_pacote_valido(void) {
    FsmProtocolo fsm;
    fsm_init(&fsm);

    // Pacote de dados: STX, QTD=3, DADOS=[10, 20, 30], CHK=(10^20^30), ETX
    // XOR calculado: 10 (0x0A) ^ 20 (0x14) = 30 (0x1E) ^ 30 (0x1E) = 0 (0x00)
    uint8_t chk_esperado = (uint8_t)(10u ^ 20u ^ 30u); // XOR dos dados úteis: 0x0A ^ 0x14 ^ 0x1E = 0x00
    uint8_t pacote[] = { STX, 3, 10, 20, 30, chk_esperado, ETX };
    size_t tam_pacote = sizeof(pacote);

    for (size_t i = 0; i < tam_pacote; i++) {
        fsm_process_char(&fsm, pacote[i]);
    }

    verifica("Erro: FSM deveria retornar ao estado ST_STX ao final", fsm.state == ST_STX);
    verifica("Erro: O pacote deveria ter sido processado com sucesso", fsm.pacote_processado == true);
    verifica("Erro: Indice do buffer final deveria ser 3", fsm.indBuffer == 3);
    verifica("Erro: Primeiro dado lido incorreto (esperado: 10)", fsm.buffer[0] == 10);
    verifica("Erro: Segundo dado lido incorreto (esperado: 20)", fsm.buffer[1] == 20);
    verifica("Erro: Terceiro dado lido incorreto (esperado: 30)", fsm.buffer[2] == 30);
    return 0;
}

// Teste 3: Garante que a FSM ignora caracteres aleatórios (ruído) antes do STX começar
static char * teste_fsm_ignorar_lixo_antes_stx(void) {
    FsmProtocolo fsm;
    fsm_init(&fsm);

    // Alimenta lixo antes do STX
    fsm_process_char(&fsm, 0xAA);
    fsm_process_char(&fsm, 0x55);
    fsm_process_char(&fsm, 0xFF);

    verifica("Erro: FSM deve permanecer no estado ST_STX apos ruidos", fsm.state == ST_STX);
    verifica("Erro: Nao deve computar nada no buffer", fsm.indBuffer == 0);

    // Agora insere um pacote válido simples
    uint8_t chk_esperado = 42;
    uint8_t pacote[] = { STX, 1, 42, chk_esperado, ETX };
    for (size_t i = 0; i < sizeof(pacote); i++) {
        fsm_process_char(&fsm, pacote[i]);
    }

    verifica("Erro: FSM deve processar pacote mesmo se houver lixo antes", fsm.pacote_processado == true);
    verifica("Erro: Dado util extraido incorretamente", fsm.buffer[0] == 42);
    return 0;
}

// Teste 4: Testa a proteção contra corrupção no campo Checksum (CHK)
static char * teste_fsm_falha_checksum(void) {
    FsmProtocolo fsm;
    fsm_init(&fsm);

    // Pacote com Checksum inválido de propósito
    // Dados corretos: STX, QTD=2, DADOS=[15, 25], CHK_CORRETO=(15^25 = 22), ETX
    // Passaremos CHK_INVALIDO = 99
    uint8_t pacote[] = { STX, 2, 15, 25, 99, ETX };

    for (size_t i = 0; i < sizeof(pacote); i++) {
        fsm_process_char(&fsm, pacote[i]);
    }

    verifica("Erro: O pacote com checksum incorreto NAO deveria ser processado", fsm.pacote_processado == false);
    verifica("Erro: FSM deveria ter resetado para o estado de busca ST_STX", fsm.state == ST_STX);
    return 0;
}

// Teste 5: Testa a proteção contra a ausência de caractere ETX válido no final
static char * teste_fsm_falha_etx(void) {
    FsmProtocolo fsm;
    fsm_init(&fsm);

    // Pacote com ETX corrompido (ex: 0x04 em vez de 0x03)
    uint8_t chk_esperado = 88;
    uint8_t pacote[] = { STX, 1, 88, chk_esperado, 0x04 };

    for (size_t i = 0; i < sizeof(pacote); i++) {
        fsm_process_char(&fsm, pacote[i]);
    }

    verifica("Erro: O pacote com ETX invalido NAO deveria ser marcado como processado", fsm.pacote_processado == false);
    verifica("Erro: FSM deveria retornar a ST_STX ao final da analise", fsm.state == ST_STX);
    return 0;
}

// Teste 6: Valida se a FSM processa múltiplos pacotes consecutivos reinicializando as variáveis internas
static char * teste_fsm_multiplos_pacotes(void) {
    FsmProtocolo fsm;
    fsm_init(&fsm);

    // Pacote 1 válido: payload [100] -> CHK = 100
    uint8_t pacote1[] = { STX, 1, 100, 100, ETX };
    for (size_t i = 0; i < sizeof(pacote1); i++) {
        fsm_process_char(&fsm, pacote1[i]);
    }
    verifica("Erro no pacote 1: Deveria estar processado", fsm.pacote_processado == true);
    verifica("Erro no pacote 1: Dado incorreto", fsm.buffer[0] == 100);

    // Pacote 2 válido consecutivo: payload [50, 60] -> CHK = (50^60 = 30)
    uint8_t chk_p2 = 50 ^ 60;
    uint8_t pacote2[] = { STX, 2, 50, 60, chk_p2, ETX };
    for (size_t i = 0; i < sizeof(pacote2); i++) {
        fsm_process_char(&fsm, pacote2[i]);
    }
    verifica("Erro no pacote 2: Deveria estar processado", fsm.pacote_processado == true);
    verifica("Erro no pacote 2: Buffer nao foi limpo corretamente ou dados incorretos", fsm.indBuffer == 2);
    verifica("Erro no pacote 2: Dado 1 incorreto", fsm.buffer[0] == 50);
    verifica("Erro no pacote 2: Dado 2 incorreto", fsm.buffer[1] == 60);

    return 0;
}

// Agrupador para execução dos casos de teste
static char * executa_todos_testes(void) {
    executa_teste(teste_fsm_inicializacao);
    executa_teste(teste_fsm_pacote_valido);
    executa_teste(teste_fsm_ignorar_lixo_antes_stx);
    executa_teste(teste_fsm_falha_checksum);
    executa_teste(teste_fsm_falha_etx);
    executa_teste(teste_fsm_multiplos_pacotes);
    return 0;
}

int main() {
    char *resultado = executa_todos_testes();
    printf("\n==================================================\n");
    printf("     RESULTADO DA SUITE DE TESTES DA TAREFA-02\n");
    printf("==================================================\n");
    if (resultado != 0) {
        printf("FALHA DETECTADA: %s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM COM SUCESSO!\n");
        printf("Testes validados:\n");
        printf("- inicializacao\n");
        printf("- pacote valido\n");
        printf("- ruido antes do STX\n");
        printf("- checksum invalido\n");
        printf("- ETX invalido\n");
        printf("- multiplos pacotes\n");
        printf("Mensagem para Print da Entrega: [TDD FSM COMPLETED SUCCESS]\n");
    }
    printf("Total de testes unitarios da FSM executados: %d\n", testes_executados);
    printf("==================================================\n\n");
    return resultado != 0;
}
