#include <stdio.h>
#include <string.h> // Para memcmp
#include "protocolo.h"

/* Framework de testes simples */
int testes_executados = 0;
#define verifica(mensagem, teste) \
    do                            \
    {                             \
        if (!(teste))             \
            return mensagem;      \
    } while (0)
#define EXECUTA_TESTE(teste)      \
    do                            \
    {                             \
        char *mensagem = teste(); \
        testes_executados++;      \
        if (mensagem)             \
            return mensagem;      \
    } while (0)

// --- Testes do Transmissor (TX) ---

static char *teste_tx_deve_iniciar_em_idle(void)
{
    protocolo_tx_t tx;
    protocolo_tx_iniciar(&tx, NULL, 5);
    verifica("TX FSM deveria iniciar no estado IDLE", tx.estado == TX_IDLE);
    return 0;
}

static char *teste_tx_deve_gerar_pacote_completo_na_ordem(void)
{
    protocolo_tx_t tx;
    uint8_t dados_originais[] = {0xAA, 0xBB, 0xCC};
    uint8_t qtd = sizeof(dados_originais);
    uint8_t checksum_esperado = 0xAA ^ 0xBB ^ 0xCC;
    uint8_t pacote_esperado[] = {0x02, 0x03, 0xAA, 0xBB, 0xCC, checksum_esperado, 0x03};
    uint8_t pacote_gerado[sizeof(pacote_esperado)];

    uint8_t byte_gerado;
    size_t i = 0;

    protocolo_tx_iniciar(&tx, dados_originais, qtd);

    while (protocolo_tx_gerar_byte(&tx, &byte_gerado))
    {
        if (i < sizeof(pacote_gerado))
        {
            pacote_gerado[i] = byte_gerado;
        }
        i++;
    }

    verifica("TX deveria gerar o numero correto de bytes", i == sizeof(pacote_esperado));
    verifica("TX pacote gerado deve ser identico ao esperado", memcmp(pacote_gerado, pacote_esperado, sizeof(pacote_esperado)) == 0);
    verifica("TX FSM deveria voltar para IDLE apos transmissao", tx.estado == TX_IDLE);

    return 0;
}

static char *teste_tx_deve_gerar_pacote_sem_dados(void)
{
    protocolo_tx_t tx;
    uint8_t checksum_esperado = 0;
    uint8_t pacote_esperado[] = {0x02, 0x00, checksum_esperado, 0x03};
    uint8_t pacote_gerado[sizeof(pacote_esperado)];
    uint8_t byte_gerado;
    size_t i = 0;
    protocolo_tx_iniciar(&tx, NULL, 0);
    while (protocolo_tx_gerar_byte(&tx, &byte_gerado))
    {
        if (i < sizeof(pacote_gerado))
        {
            pacote_gerado[i] = byte_gerado;
        }
        i++;
    }
    verifica("TX (0 dados) deveria gerar o numero correto de bytes", i == sizeof(pacote_esperado));
    verifica("TX (0 dados) pacote gerado deve ser o esperado", memcmp(pacote_gerado, pacote_esperado, sizeof(pacote_esperado)) == 0);
    return 0;
}

// --- Testes do Receptor (RX) ---

static char *teste_rx_deve_iniciar_em_idle(void)
{
    protocolo_rx_t rx;
    protocolo_rx_iniciar(&rx);
    verifica("RX FSM deveria iniciar no estado IDLE", rx.estado == RX_IDLE);
    verifica("RX flag 'pacote_pronto' deveria ser falsa", rx.pacote_pronto == false);
    return 0;
}

static char *teste_rx_deve_processar_pacote_valido(void)
{
    protocolo_rx_t rx;
    uint8_t dados_esperados[] = {0x11, 0x22};
    uint8_t checksum = 0x11 ^ 0x22;
    uint8_t pacote_valido[] = {0x02, 0x02, 0x11, 0x22, checksum, 0x03};
    protocolo_rx_iniciar(&rx);
    for (size_t i = 0; i < sizeof(pacote_valido); i++)
    {
        protocolo_rx_processar_byte(&rx, pacote_valido[i]);
    }
    verifica("RX deveria sinalizar pacote completo e valido", protocolo_rx_pacote_completo(&rx) == true);
    verifica("RX FSM deveria estar no estado PACOTE_PRONTO", rx.estado == RX_PACOTE_PRONTO);
    verifica("RX quantidade de dados recebida deve ser 2", rx.qtd_dados == 2);
    verifica("RX dados recebidos devem ser os esperados", memcmp(rx.dados, dados_esperados, 2) == 0);
    return 0;
}

static char *teste_rx_deve_ignorar_lixo_antes_do_stx(void)
{
    protocolo_rx_t rx;
    uint8_t pacote_com_lixo[] = {0xFF, 0xEE, 0xDD, 0x02, 0x01, 0xAA, 0xAA, 0x03};
    protocolo_rx_iniciar(&rx);
    for (size_t i = 0; i < sizeof(pacote_com_lixo); i++)
    {
        protocolo_rx_processar_byte(&rx, pacote_com_lixo[i]);
    }
    verifica("RX deveria ter processado o pacote mesmo com lixo antes", protocolo_rx_pacote_completo(&rx) == true);
    verifica("RX dados recebidos deveriam ser 0xAA", rx.dados[0] == 0xAA);
    return 0;
}

static char *teste_rx_deve_rejeitar_pacote_com_checksum_invalido(void)
{
    protocolo_rx_t rx;
    uint8_t pacote_invalido[] = {0x02, 0x02, 0x11, 0x22, 0x00, 0x03};
    protocolo_rx_iniciar(&rx);
    for (size_t i = 0; i < sizeof(pacote_invalido); i++)
    {
        protocolo_rx_processar_byte(&rx, pacote_invalido[i]);
    }
    verifica("RX nao deveria sinalizar pacote completo com checksum invalido", protocolo_rx_pacote_completo(&rx) == false);
    return 0;
}

static char *teste_rx_deve_processar_pacote_sem_dados(void)
{
    protocolo_rx_t rx;
    uint8_t pacote_valido[] = {0x02, 0x00, 0x00, 0x03};
    protocolo_rx_iniciar(&rx);
    for (size_t i = 0; i < sizeof(pacote_valido); i++)
    {
        protocolo_rx_processar_byte(&rx, pacote_valido[i]);
    }
    verifica("RX (0 dados) deveria sinalizar pacote completo", protocolo_rx_pacote_completo(&rx) == true);
    verifica("RX (0 dados) quantidade de dados deve ser 0", rx.qtd_dados == 0);
    return 0;
}

static char *teste_rx_deve_rejeitar_pacote_com_qtd_maior_que_o_buffer(void)
{
    protocolo_rx_t rx;
    uint8_t qtd_invalida = MAX_DADOS_SIZE + 1;
    uint8_t pacote[] = {0x02, qtd_invalida, 0xAA};
    protocolo_rx_iniciar(&rx);
    for (size_t i = 0; i < sizeof(pacote); i++)
    {
        protocolo_rx_processar_byte(&rx, pacote[i]);
    }
    verifica("RX deveria estar em IDLE apos QTD invalida", rx.estado == RX_IDLE);
    verifica("RX nao deveria sinalizar pacote completo com QTD invalida", protocolo_rx_pacote_completo(&rx) == false);
    return 0;
}

static char *teste_rx_deve_resetar_se_novo_stx_chegar_no_meio(void)
{
    protocolo_rx_t rx;
    uint8_t fluxo_dados[] = {0x02, 0x05, 0xAA, 0xBB, /* Interrupcao! */ 0x02, 0x01, 0xCC, 0xCC, 0x03};
    protocolo_rx_iniciar(&rx);
    for (size_t i = 0; i < sizeof(fluxo_dados); i++)
    {
        protocolo_rx_processar_byte(&rx, fluxo_dados[i]);
    }
    verifica("RX (interrompido) deveria ter processado o segundo pacote", protocolo_rx_pacote_completo(&rx) == true);
    verifica("RX (interrompido) QTD deveria ser do segundo pacote (1)", rx.qtd_dados == 1);
    verifica("RX (interrompido) dado deveria ser do segundo pacote (0xCC)", rx.dados[0] == 0xCC);
    return 0;
}

static char *teste_rx_deve_rejeitar_pacote_sem_etx(void)
{
    protocolo_rx_t rx;
    uint8_t checksum = 0x11 ^ 0x22;
    uint8_t pacote_incompleto[] = {0x02, 0x02, 0x11, 0x22, checksum, 0xFF};
    protocolo_rx_iniciar(&rx);
    for (size_t i = 0; i < sizeof(pacote_incompleto); i++)
    {
        protocolo_rx_processar_byte(&rx, pacote_incompleto[i]);
    }
    verifica("RX (sem ETX) nao deveria sinalizar pacote completo", protocolo_rx_pacote_completo(&rx) == false);
    verifica("RX (sem ETX) deveria voltar para IDLE", rx.estado == RX_IDLE);
    return 0;
}

// --- Execucao de Todos os Testes ---

static char *executa_todos_os_testes(void)
{
    EXECUTA_TESTE(teste_tx_deve_iniciar_em_idle);
    EXECUTA_TESTE(teste_tx_deve_gerar_pacote_completo_na_ordem);
    EXECUTA_TESTE(teste_tx_deve_gerar_pacote_sem_dados);

    EXECUTA_TESTE(teste_rx_deve_iniciar_em_idle);
    EXECUTA_TESTE(teste_rx_deve_processar_pacote_valido);
    EXECUTA_TESTE(teste_rx_deve_ignorar_lixo_antes_do_stx);
    EXECUTA_TESTE(teste_rx_deve_rejeitar_pacote_com_checksum_invalido);
    EXECUTA_TESTE(teste_rx_deve_processar_pacote_sem_dados);
    EXECUTA_TESTE(teste_rx_deve_rejeitar_pacote_com_qtd_maior_que_o_buffer);
    EXECUTA_TESTE(teste_rx_deve_resetar_se_novo_stx_chegar_no_meio);
    EXECUTA_TESTE(teste_rx_deve_rejeitar_pacote_sem_etx);
    return 0;
}

int main()
{
    char *resultado = executa_todos_os_testes();
    if (resultado != 0)
    {
        printf("ERRO NO TESTE: %s\n", resultado);
    }
    else
    {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    printf("\nPressione Enter para sair...");
    getchar(); // O programa vai esperar aqui ate que pressione Enter.

    return resultado != 0;
}
