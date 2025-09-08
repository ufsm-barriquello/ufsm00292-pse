#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocolo_fsm.h"

/* Macros de testes TDD */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                if (mensagem) return mensagem; else printf(" Teste %d PASSOU: %s\n", testes_executados, #teste); } while (0)

int testes_executados = 0;

/* Função para demonstrar um pacote completo */
static void demonstrar_pacote_exemplo(void) {
    printf("\nEXEMPLO DE PACOTE COMPLETO:\n");
    printf("Enviando pacote: 02 03 AA BB CC 66 03\n");
    printf("Onde:\n");
    printf("  02 = STX (Start of Transmission)\n");
    printf("  03 = QTD_DADOS (3 bytes de dados)\n");
    printf("  AA = Dado 1\n");
    printf("  BB = Dado 2\n");
    printf("  CC = Dado 3\n");

    // Cálculo CORRETO do checksum: STX ^ QTD ^ DADO1 ^ DADO2 ^ DADO3
    uint8_t checksum_calculado = 0x02 ^ 0x03 ^ 0xAA ^ 0xBB ^ 0xCC;
    printf("  66 = CHK (Checksum: 02 ^ 03 ^ AA ^ BB ^ CC = %02X)\n", checksum_calculado);
    printf("  03 = ETX (End of Transmission)\n");

    // Demonstração prática do pacote
    MaquinaProtocolo maq;
    maquina_init(&maq);

    printf("\nProcessando pacote exemplo...\n");

    // Processa cada byte do pacote
    uint8_t pacote[] = {0x02, 0x03, 0xAA, 0xBB, 0xCC, checksum_calculado, 0x03};
    const char *nomes[] = {"STX", "QTD", "DADO1", "DADO2", "DADO3", "CHK", "ETX"};

    for (int i = 0; i < 7; i++) {
        EstadoProtocolo estado_anterior = maq.estado_atual;
        maquina_processa_evento(&maq, pacote[i]);

        printf("  Byte %02X (%s): Estado %d -> %d\n",
               pacote[i], nomes[i], estado_anterior, maq.estado_atual);
    }

    if (maquina_pacote_completo(&maq)) {
        uint8_t tamanho;
        const uint8_t *dados = maquina_obter_dados(&maq, &tamanho);
        printf("\n Pacote processado com SUCESSO!\n");
        printf("  Dados recebidos: ");
        for (int i = 0; i < tamanho; i++) {
            printf("%02X ", dados[i]);
        }
        printf("(%d bytes)\n", tamanho);
    } else {
        printf("\n Pacote com ERRO!\n");
    }
    printf("==================================================\n");
}

/* Testes unitários (mantidos exatamente iguais) */
static char *teste_idle_recebe_stx_transita_aguarda_qtd(void) {
    MaquinaProtocolo maq;
    maquina_init(&maq);

    EstadoProtocolo novo_estado = maquina_processa_evento(&maq, STX);

    verifica("IDLE + STX deve ir para AGUARDA_QTD",
             novo_estado == ESTADO_AGUARDA_QTD);
    verifica("Estado atual deve ser AGUARDA_QTD",
             maq.estado_atual == ESTADO_AGUARDA_QTD);

    return 0;
}

static char *teste_idle_recebe_outro_byte_permanece_idle(void) {
    MaquinaProtocolo maq;
    maquina_init(&maq);

    EstadoProtocolo novo_estado = maquina_processa_evento(&maq, 0x55);

    verifica("IDLE + outro byte deve permanecer IDLE",
             novo_estado == ESTADO_IDLE);
    verifica("Estado atual deve permanecer IDLE",
             maq.estado_atual == ESTADO_IDLE);

    return 0;
}

static char *teste_aguarda_qtd_armazena_quantidade(void) {
    MaquinaProtocolo maq;
    maquina_init(&maq);

    maquina_processa_evento(&maq, STX);
    EstadoProtocolo novo_estado = maquina_processa_evento(&maq, 0x03);

    verifica("AGUARDA_QTD + quantidade deve ir para PROCESS_DADOS",
             novo_estado == ESTADO_PROCESS_DADOS);
    verifica("Quantidade deve ser armazenada",
             maq.qtd_dados_esperada == 0x03);

    return 0;
}

static char *teste_processa_dados_corretamente(void) {
    MaquinaProtocolo maq;
    maquina_init(&maq);

    // STX -> QTD=2 -> Dado1 -> Dado2 -> CHK -> ETX
    maquina_processa_evento(&maq, STX);
    maquina_processa_evento(&maq, 0x02);
    maquina_processa_evento(&maq, 0xAA);
    maquina_processa_evento(&maq, 0xBB);

    verifica("Deve estar em AGUARDA_CHK apos 2 dados",
             maq.estado_atual == ESTADO_AGUARDA_CHK);
    verifica("Dado1 deve ser armazenado",
             maq.dados_recebidos[0] == 0xAA);
    verifica("Dado2 deve ser armazenado",
             maq.dados_recebidos[1] == 0xBB);

    return 0;
}

static char *teste_pacote_completo_valido(void) {
    MaquinaProtocolo maq;
    maquina_init(&maq);

    // Pacote completo: STX, QTD=2, 0x01, 0x02, CHK, ETX
    // Checksum calculado: STX ^ QTD ^ Dado1 ^ Dado2
    uint8_t checksum = STX ^ 0x02 ^ 0x01 ^ 0x02;

    maquina_processa_evento(&maq, STX);
    maquina_processa_evento(&maq, 0x02);
    maquina_processa_evento(&maq, 0x01);
    maquina_processa_evento(&maq, 0x02);
    maquina_processa_evento(&maq, checksum);
    maquina_processa_evento(&maq, ETX);

    verifica("Pacote valido deve ser completado",
             maquina_pacote_completo(&maq) == 1);
    verifica("Deve voltar para estado IDLE",
             maq.estado_atual == ESTADO_IDLE);

    uint8_t tamanho;
    const uint8_t *dados = maquina_obter_dados(&maq, &tamanho);
    verifica("Tamanho dos dados deve ser 2",
             tamanho == 2);
    verifica("Dado1 deve ser 0x01",
             dados[0] == 0x01);
    verifica("Dado2 deve ser 0x02",
             dados[1] == 0x02);

    return 0;
}

static char *teste_checksum_invalido(void) {
    MaquinaProtocolo maq;
    maquina_init(&maq);

    maquina_processa_evento(&maq, STX);
    maquina_processa_evento(&maq, 0x02);
    maquina_processa_evento(&maq, 0x01);
    maquina_processa_evento(&maq, 0x02);
    maquina_processa_evento(&maq, 0xFF); // Checksum inválido
    maquina_processa_evento(&maq, ETX);

    verifica("Pacote com checksum invalido nao deve ser completado",
             maquina_pacote_completo(&maq) == 0);
    verifica("Deve voltar para estado IDLE",
             maq.estado_atual == ESTADO_IDLE);

    return 0;
}

static char *teste_erro_recuperacao(void) {
    MaquinaProtocolo maq;
    maquina_init(&maq);

    // Força erro
    maquina_processa_evento(&maq, 0x55); // Byte inválido
    verifica("Byte invalido deve manter em IDLE",
             maq.estado_atual == ESTADO_IDLE);

    // Agora envia pacote válido
    uint8_t checksum = STX ^ 0x01 ^ 0xAB;
    maquina_processa_evento(&maq, STX);
    maquina_processa_evento(&maq, 0x01);
    maquina_processa_evento(&maq, 0xAB);
    maquina_processa_evento(&maq, checksum);
    maquina_processa_evento(&maq, ETX);

    verifica("Deve processar pacote valido apos erro",
             maquina_pacote_completo(&maq) == 1);

    return 0;
}

/* Função principal de testes */
static char *executa_testes(void) {
    printf("==================================================\n");
    printf("PROTOCOLO DE COMUNICAÇÃO - MÁQUINA DE ESTADOS FINITOS\n");
    printf("==================================================\n");
    printf("Formato: | STX | QTD_DADOS | DADOS | CHK | ETX |\n");
    printf("STX: 0x02 (Start of Transmission)\n");
    printf("ETX: 0x03 (End of Transmission)\n");
    printf("CHK: Checksum (XOR de todos os bytes anteriores)\n");
    printf("==================================================\n");

    demonstrar_pacote_exemplo();

    printf("EXECUTANDO TESTES TDD:\n");
    printf("==================================================\n");

    executa_teste(teste_idle_recebe_stx_transita_aguarda_qtd);
    executa_teste(teste_idle_recebe_outro_byte_permanece_idle);
    executa_teste(teste_aguarda_qtd_armazena_quantidade);
    executa_teste(teste_processa_dados_corretamente);
    executa_teste(teste_pacote_completo_valido);
    executa_teste(teste_checksum_invalido);
    executa_teste(teste_erro_recuperacao);

    return 0;
}

int main() {
    char *resultado = executa_testes();

    printf("==================================================\n");
    if (resultado != 0) {
        printf("FALHA: %s\n", resultado);
    } else {
        printf("TODOS OS 7 TESTES PASSARAM!\n");
    }

    printf("Testes executados: %d\n", testes_executados);
    printf("==================================================\n");

    return resultado != 0;
}
