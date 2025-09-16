#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "protocolo_pt.h"

// --- Framework de Testes ---
int testes_executados = 0;
#define VERIFICA(mensagem, teste) \
    do { \
        if (!(teste)) { \
            printf("\nFALHOU: %s\n", mensagem); \
            assert(teste); \
            return "Falha no teste"; \
        } \
    } while (0)

#define EXECUTA_TESTE(teste) \
    do { \
        printf("Executando: %s...\n", #teste); \
        char *mensagem = teste(); \
        testes_executados++; \
        if (mensagem) return mensagem; \
    } while (0)

// Estruturas globais para os testes
static shared_channel_t canal_teste;
static struct pt pt_tx, pt_rx;

// Agendadores isolados para garantir que os testes não interfiram uns nos outros
void agendar_apenas_rx(int vezes) {
    for(int i = 0; i < vezes; i++) {
        protothread_rx(&pt_rx, &canal_teste);
    }
}

void agendar_apenas_tx(int vezes) {
     for(int i = 0; i < vezes; i++) {
        protothread_tx(&pt_tx, &canal_teste);
    }
}

// --- Suíte de Testes (11 Casos) ---

// Testes de TX
static char *teste_tx_pt_deve_iniciar_em_idle() {
    PT_INIT(&pt_tx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    VERIFICA("TX PT: deveria iniciar com a flag de transmissão inativa", !canal_teste.iniciar_transmissao);
    return 0;
}

static char *teste_tx_pt_deve_gerar_pacote_completo() {
    PT_INIT(&pt_tx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t dados_originais[] = {0xAA, 0xBB, 0xCC};
    uint8_t checksum_esperado = 0xAA ^ 0xBB ^ 0xCC;
    uint8_t pacote_esperado[] = {0x02, 0x03, 0xAA, 0xBB, 0xCC, checksum_esperado, 0x03};
    uint8_t pacote_gerado[sizeof(pacote_esperado)];
    size_t bytes_recebidos = 0;
    memcpy(canal_teste.dados_para_enviar, dados_originais, sizeof(dados_originais));
    canal_teste.qtd_a_enviar = sizeof(dados_originais);
    canal_teste.iniciar_transmissao = true;
    for(int i = 0; i < 100 && bytes_recebidos < sizeof(pacote_gerado); i++) {
        agendar_apenas_tx(1);
        uint8_t byte;
        while(buffer_read(&canal_teste.tx_to_rx_buffer, &byte)) { pacote_gerado[bytes_recebidos++] = byte; }
    }
    VERIFICA("TX PT: deveria gerar o número correto de bytes", bytes_recebidos == sizeof(pacote_esperado));
    VERIFICA("TX PT: pacote gerado deve ser idêntico ao esperado", memcmp(pacote_gerado, pacote_esperado, sizeof(pacote_esperado)) == 0);
    return 0;
}

static char *teste_tx_pt_deve_gerar_pacote_sem_dados() {
    PT_INIT(&pt_tx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t pacote_esperado[] = {0x02, 0x00, 0x00, 0x03};
    uint8_t pacote_gerado[sizeof(pacote_esperado)];
    size_t bytes_recebidos = 0;
    canal_teste.qtd_a_enviar = 0;
    canal_teste.iniciar_transmissao = true;
    for(int i = 0; i < 100 && bytes_recebidos < sizeof(pacote_gerado); i++) {
        agendar_apenas_tx(1);
        uint8_t byte;
        while(buffer_read(&canal_teste.tx_to_rx_buffer, &byte)) { pacote_gerado[bytes_recebidos++] = byte; }
    }
    VERIFICA("TX PT (0 dados): deveria gerar o número correto de bytes", bytes_recebidos == sizeof(pacote_esperado));
    VERIFICA("TX PT (0 dados): pacote gerado deve ser o esperado", memcmp(pacote_gerado, pacote_esperado, sizeof(pacote_esperado)) == 0);
    return 0;
}

// Testes de RX
static char *teste_rx_pt_deve_iniciar_em_idle() {
    PT_INIT(&pt_rx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    VERIFICA("RX PT: flag 'pacote_pronto' deveria ser falsa no início", !canal_teste.pacote_recebido_com_sucesso);
    return 0;
}

static char* teste_rx_pt_pacote_valido() {
    // SETUP: Limpa o estado para um teste limpo
    PT_INIT(&pt_rx); 
    memset(&canal_teste, 0, sizeof(shared_channel_t));

    // PREPARAÇÃO: Coloca um pacote 100% válido no buffer de entrada
    uint8_t pacote_a_enviar[] = {0x02, 0x02, 0xAA, 0xBB, 0x11, 0x03};
    for(size_t i = 0; i < sizeof(pacote_a_enviar); i++) { 
        buffer_write(&canal_teste.tx_to_rx_buffer, pacote_a_enviar[i]); 
    }

    // EXECUÇÃO: Roda o protothread do receptor para processar os bytes no buffer
    agendar_apenas_rx(100); 

    /*
    DIAGNÓSTICO: Imprime o estado final do canal antes de verificar
    printf("\n--- Bloco de Diagnostico do Teste ---\n");
    printf("Estado final da flag 'pacote_recebido_com_sucesso': %s\n", canal_teste.pacote_recebido_com_sucesso ? "true" : "false");
    printf("Quantidade de dados recebida: %u\n", canal_teste.qtd_recebida);
    printf("Dados recebidos[0]: 0x%02X\n", canal_teste.dados_recebidos[0]);
    printf("Dados recebidos[1]: 0x%02X\n", canal_teste.dados_recebidos[1]);
    printf("Checksum final calculado pelo RX: 0x%02X\n", canal_teste.rx_chk_calculado);
    printf("--------------------------------------\n");
    */ 

    // VERIFICAÇÃO: Checa se o resultado final está correto
    VERIFICA("RX PT: Pacote válido deveria ser recebido com sucesso", canal_teste.pacote_recebido_com_sucesso);
    VERIFICA("RX PT: Quantidade de dados deve ser 2", canal_teste.qtd_recebida == 2);
    VERIFICA("RX PT: Dados recebidos devem ser os esperados", canal_teste.dados_recebidos[0] == 0xAA && canal_teste.dados_recebidos[1] == 0xBB);
    
    return 0; // Se chegar aqui, o teste passou
}

static char *teste_rx_pt_deve_ignorar_lixo_antes_do_stx() {
    PT_INIT(&pt_rx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t pacote_com_lixo[] = {0xFF, 0xEE, 0xDD, 0x02, 0x01, 0xAA, 0xAA, 0x03};
    for(size_t i = 0; i < sizeof(pacote_com_lixo); i++) { buffer_write(&canal_teste.tx_to_rx_buffer, pacote_com_lixo[i]); }
    agendar_apenas_rx(100);
    VERIFICA("RX PT: deveria ter processado pacote mesmo com lixo antes", canal_teste.pacote_recebido_com_sucesso);
    VERIFICA("RX PT: dados recebidos deveriam ser 0xAA", canal_teste.dados_recebidos[0] == 0xAA);
    return 0;
}

static char *teste_rx_pt_deve_rejeitar_pacote_com_checksum_invalido() {
    PT_INIT(&pt_rx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t pacote_invalido[] = {0x02, 0x02, 0x11, 0x22, 0x00, 0x03};
    for (size_t i = 0; i < sizeof(pacote_invalido); i++) { buffer_write(&canal_teste.tx_to_rx_buffer, pacote_invalido[i]); }
    agendar_apenas_rx(100);
    VERIFICA("RX PT: não deveria sinalizar pacote completo com checksum inválido", !canal_teste.pacote_recebido_com_sucesso);
    return 0;
}

static char *teste_rx_pt_deve_processar_pacote_sem_dados() {
    PT_INIT(&pt_rx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t pacote_valido[] = {0x02, 0x00, 0x00, 0x03};
    for (size_t i = 0; i < sizeof(pacote_valido); i++) { buffer_write(&canal_teste.tx_to_rx_buffer, pacote_valido[i]); }
    agendar_apenas_rx(100);
    VERIFICA("RX PT (0 dados): deveria sinalizar pacote completo", canal_teste.pacote_recebido_com_sucesso);
    VERIFICA("RX PT (0 dados): quantidade de dados deve ser 0", canal_teste.qtd_recebida == 0);
    return 0;
}

static char *teste_rx_pt_deve_rejeitar_pacote_com_qtd_maior_que_o_buffer() {
    PT_INIT(&pt_rx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t qtd_invalida = MAX_DADOS_SIZE + 1;
    buffer_write(&canal_teste.tx_to_rx_buffer, 0x02);
    buffer_write(&canal_teste.tx_to_rx_buffer, qtd_invalida);
    buffer_write(&canal_teste.tx_to_rx_buffer, 0xAA);
    agendar_apenas_rx(100);
    VERIFICA("RX PT: não deveria sinalizar pacote completo com QTD inválida", !canal_teste.pacote_recebido_com_sucesso);
    return 0;
}

static char *teste_rx_pt_deve_resetar_se_novo_stx_chegar_no_meio() {
    PT_INIT(&pt_rx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t fluxo_dados[] = {0x02, 0x05, 0xAA, 0xBB, 0x02, 0x01, 0xCC, 0xCC, 0x03};
    for (size_t i = 0; i < sizeof(fluxo_dados); i++) { buffer_write(&canal_teste.tx_to_rx_buffer, fluxo_dados[i]); }
    agendar_apenas_rx(200);
    VERIFICA("RX PT (interrompido): deveria ter processado o segundo pacote", canal_teste.pacote_recebido_com_sucesso);
    VERIFICA("RX PT (interrompido): QTD deveria ser do segundo pacote (1)", canal_teste.qtd_recebida == 1);
    VERIFICA("RX PT (interrompido): dado deveria ser do segundo pacote (0xCC)", canal_teste.dados_recebidos[0] == 0xCC);
    return 0;
}

static char *teste_rx_pt_deve_rejeitar_pacote_sem_etx() {
    PT_INIT(&pt_rx); memset(&canal_teste, 0, sizeof(shared_channel_t));
    uint8_t checksum = 0x11 ^ 0x22;
    uint8_t pacote_incompleto[] = {0x02, 0x02, 0x11, 0x22, checksum, 0xFF};
    for (size_t i = 0; i < sizeof(pacote_incompleto); i++) { buffer_write(&canal_teste.tx_to_rx_buffer, pacote_incompleto[i]); }
    agendar_apenas_rx(100);
    VERIFICA("RX PT (sem ETX): não deveria sinalizar pacote completo", !canal_teste.pacote_recebido_com_sucesso);
    return 0;
}

static char *executa_todos_os_testes(void) {
    EXECUTA_TESTE(teste_tx_pt_deve_iniciar_em_idle);
    EXECUTA_TESTE(teste_tx_pt_deve_gerar_pacote_completo);
    EXECUTA_TESTE(teste_tx_pt_deve_gerar_pacote_sem_dados);
    EXECUTA_TESTE(teste_rx_pt_deve_iniciar_em_idle);
    EXECUTA_TESTE(teste_rx_pt_pacote_valido);
    EXECUTA_TESTE(teste_rx_pt_deve_ignorar_lixo_antes_do_stx);
    EXECUTA_TESTE(teste_rx_pt_deve_rejeitar_pacote_com_checksum_invalido);
    EXECUTA_TESTE(teste_rx_pt_deve_processar_pacote_sem_dados);
    EXECUTA_TESTE(teste_rx_pt_deve_rejeitar_pacote_com_qtd_maior_que_o_buffer);
    EXECUTA_TESTE(teste_rx_pt_deve_resetar_se_novo_stx_chegar_no_meio);
    EXECUTA_TESTE(teste_rx_pt_deve_rejeitar_pacote_sem_etx);
    return 0;
}

int main() {
    char *resultado_testes = executa_todos_os_testes();
    if (resultado_testes != 0) {
        printf("\nUM OU MAIS TESTES FALHARAM.\n");
        return 1;
    }
    printf("\nTODOS OS %d TESTES PASSARAM COM SUCESSO!\n\n", testes_executados);

    printf("--- Iniciando Demonstracao da Comunicacao Final ---\n");
    struct pt pt_tx_main, pt_rx_main;
    shared_channel_t canal_main;
    PT_INIT(&pt_tx_main); PT_INIT(&pt_rx_main);
    memset(&canal_main, 0, sizeof(shared_channel_t));

    // Configura o pacote a ser enviado
    uint8_t dados[] = {0xDE, 0xAD, 0xBE, 0xEF};
    memcpy(canal_main.dados_para_enviar, dados, sizeof(dados));
    canal_main.qtd_a_enviar = sizeof(dados);
    
    // Inicia a transmissão
    printf("Driver: Solicitando envio do pacote [DE AD BE EF]\n");
    canal_main.iniciar_transmissao = true;
    
    // FSM para decodificar o ACK
    enum { ACK_IDLE, ACK_STX, ACK_QTD, ACK_DADO, ACK_CHK, ACK_ETX } ack_state = ACK_IDLE;
    uint8_t ack_byte;
    uint8_t ack_checksum_calc = 0;

    // Loop de agendamento da demonstração
    for(int i = 0; i < 5000; i++) {
        // Roda ambos os protothreads
        protothread_tx(&pt_tx_main, &canal_main);
        protothread_rx(&pt_rx_main, &canal_main);

        // O Driver verifica se o RX recebeu o pacote
        if (canal_main.pacote_recebido_com_sucesso) {
            printf("Driver: Pacote de dados consumido.\n");
            // Sinaliza ao RX que o pacote foi "consumido" e ele pode continuar
            canal_main.pacote_recebido_com_sucesso = false; 
        }
        
        // O Driver verifica se o ciclo terminou para encerrar a demonstração
        if (canal_main.ack_recebido && !canal_main.iniciar_transmissao) {
            printf("Driver: Ciclo de comunicacao completo.\n");
            break;
        }
    }
    printf("\n--- Fim da Demonstracao ---\n");
}