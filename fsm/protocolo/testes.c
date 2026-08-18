/*
 * Testes das FSMs do protocolo (desenvolvimento guiado por testes).
 *
 * Cada teste segue as 3 fases vistas em aula:
 *   1. Configuracao: estado atual da maquina + evento + resultado esperado
 *   2. Exercicio:    estimula a maquina com o evento
 *   3. Verificacao:  confere estado de destino e saidas produzidas
 */

#include <stdio.h>
#include <string.h>
#include "protocolo.h"

static int total_verificacoes = 0;
static int falhas = 0;
static const char *teste_corrente = "";

#define VERIFICA(cond, descricao)                                          \
    do {                                                                   \
        total_verificacoes++;                                              \
        if (!(cond)) {                                                     \
            falhas++;                                                      \
            printf("  [FALHOU] %s: %s (linha %d)\n",                       \
                   teste_corrente, (descricao), __LINE__);                 \
        }                                                                  \
    } while (0)

#define TESTE(nome)                                                        \
    do {                                                                   \
        teste_corrente = #nome;                                            \
        nome();                                                            \
    } while (0)

/* ================================================================== */
/* Receptor                                                           */
/* ================================================================== */

static void rx_estado_inicial_e_espera_stx(void)
{
    rx_t rx;                                    /* 1. configuracao      */

    rx_init(&rx);                               /* 2. exercicio         */

    VERIFICA(rx.estado == RX_ESPERA_STX,        /* 3. verificacao       */
             "apos rx_init a FSM deve esperar STX");
}

static void rx_ignora_lixo_fora_de_quadro(void)
{
    rx_t rx;
    rx_result_t r;

    rx_init(&rx);                               /* estado: ESPERA_STX   */

    r = rx_processa(&rx, 0xAA);                 /* evento: byte != STX  */

    VERIFICA(rx.estado == RX_ESPERA_STX, "deve permanecer em ESPERA_STX");
    VERIFICA(r == RX_NADA, "byte de ruido nao gera resultado");
}

static void rx_stx_leva_para_espera_qtd(void)
{
    rx_t rx;
    rx_result_t r;

    rx_init(&rx);

    r = rx_processa(&rx, STX);

    VERIFICA(rx.estado == RX_ESPERA_QTD, "STX deve levar a ESPERA_QTD");
    VERIFICA(r == RX_NADA, "STX sozinho nao completa quadro");
}

static void rx_qtd_valida_leva_para_recebe_dados(void)
{
    rx_t rx;
    rx_result_t r;

    rx_init(&rx);
    rx_processa(&rx, STX);                      /* estado: ESPERA_QTD   */

    r = rx_processa(&rx, 3);                    /* evento: QTD = 3      */

    VERIFICA(rx.estado == RX_RECEBE_DADOS, "QTD > 0 deve levar a RECEBE_DADOS");
    VERIFICA(rx.qtd == 3, "QTD recebido deve ser armazenado");
    VERIFICA(rx.chk == 3, "checksum parcial deve iniciar com o QTD");
    VERIFICA(r == RX_NADA, "quadro ainda incompleto");
}

static void rx_qtd_zero_pula_para_checksum(void)
{
    rx_t rx;
    rx_result_t r;

    rx_init(&rx);
    rx_processa(&rx, STX);

    r = rx_processa(&rx, 0);                    /* evento: QTD = 0      */

    VERIFICA(rx.estado == RX_ESPERA_CHK, "QTD = 0 deve levar direto a ESPERA_CHK");
    VERIFICA(r == RX_NADA, "quadro ainda incompleto");
}

static void rx_qtd_acima_do_limite_gera_erro(void)
{
    rx_t rx;
    rx_result_t r;

    rx_init(&rx);
    rx_processa(&rx, STX);

    r = rx_processa(&rx, PROTO_MAX_DADOS + 1);

    VERIFICA(r == RX_ERRO_QTD, "QTD acima do limite deve gerar RX_ERRO_QTD");
    VERIFICA(rx.estado == RX_ESPERA_STX, "apos erro a FSM ressincroniza");
}

static void rx_dados_sao_armazenados_em_ordem(void)
{
    rx_t rx;
    rx_result_t r;

    rx_init(&rx);
    rx_processa(&rx, STX);
    rx_processa(&rx, 2);                        /* estado: RECEBE_DADOS */

    rx_processa(&rx, 0x10);
    r = rx_processa(&rx, 0x20);

    VERIFICA(rx.dados[0] == 0x10 && rx.dados[1] == 0x20,
             "dados devem ser gravados na ordem de chegada");
    VERIFICA(rx.estado == RX_ESPERA_CHK, "ultimo dado deve levar a ESPERA_CHK");
    VERIFICA(rx.chk == (uint8_t)(2 + 0x10 + 0x20), "checksum parcial acumulado");
    VERIFICA(r == RX_NADA, "quadro ainda incompleto");
}

static void rx_checksum_correto_leva_para_espera_etx(void)
{
    rx_t rx;
    rx_result_t r;
    uint8_t dados[1];

    dados[0] = 0x55;
    rx_init(&rx);
    rx_processa(&rx, STX);
    rx_processa(&rx, 1);
    rx_processa(&rx, dados[0]);                 /* estado: ESPERA_CHK   */

    r = rx_processa(&rx, protocolo_checksum(1, dados));

    VERIFICA(rx.estado == RX_ESPERA_ETX, "checksum correto deve levar a ESPERA_ETX");
    VERIFICA(r == RX_NADA, "quadro so termina no ETX");
}

static void rx_checksum_errado_gera_erro(void)
{
    rx_t rx;
    rx_result_t r;
    uint8_t dados[1];

    dados[0] = 0x55;
    rx_init(&rx);
    rx_processa(&rx, STX);
    rx_processa(&rx, 1);
    rx_processa(&rx, dados[0]);

    r = rx_processa(&rx, (uint8_t)(protocolo_checksum(1, dados) + 1));

    VERIFICA(r == RX_ERRO_CHK, "checksum invalido deve gerar RX_ERRO_CHK");
    VERIFICA(rx.estado == RX_ESPERA_STX, "apos erro a FSM ressincroniza");
}

static void rx_etx_completa_o_quadro(void)
{
    rx_t rx;
    rx_result_t r;
    uint8_t dados[1];

    dados[0] = 0x55;
    rx_init(&rx);
    rx_processa(&rx, STX);
    rx_processa(&rx, 1);
    rx_processa(&rx, dados[0]);
    rx_processa(&rx, protocolo_checksum(1, dados));  /* estado: ESPERA_ETX */

    r = rx_processa(&rx, ETX);

    VERIFICA(r == RX_QUADRO_OK, "ETX deve completar o quadro");
    VERIFICA(rx.estado == RX_ESPERA_STX, "FSM volta a esperar o proximo quadro");
    VERIFICA(rx.qtd == 1 && rx.dados[0] == 0x55, "quadro recebido fica disponivel");
}

static void rx_sem_etx_gera_erro(void)
{
    rx_t rx;
    rx_result_t r;
    uint8_t dados[1];

    dados[0] = 0x55;
    rx_init(&rx);
    rx_processa(&rx, STX);
    rx_processa(&rx, 1);
    rx_processa(&rx, dados[0]);
    rx_processa(&rx, protocolo_checksum(1, dados));

    r = rx_processa(&rx, 0x00);                 /* evento: byte != ETX  */

    VERIFICA(r == RX_ERRO_ETX, "byte final invalido deve gerar RX_ERRO_ETX");
    VERIFICA(rx.estado == RX_ESPERA_STX, "apos erro a FSM ressincroniza");
}

static void rx_recebe_quadro_completo(void)
{
    rx_t rx;
    rx_result_t r = RX_NADA;
    uint8_t quadro[] = { STX, 3, 'A', 'B', 'C', (uint8_t)(3 + 'A' + 'B' + 'C'), ETX };
    unsigned i;

    rx_init(&rx);

    for (i = 0; i < sizeof(quadro); i++) {
        r = rx_processa(&rx, quadro[i]);
    }

    VERIFICA(r == RX_QUADRO_OK, "quadro completo deve ser aceito");
    VERIFICA(rx.qtd == 3, "QTD do quadro recebido");
    VERIFICA(memcmp(rx.dados, "ABC", 3) == 0, "conteudo do quadro recebido");
}

static void rx_recebe_quadro_sem_dados(void)
{
    rx_t rx;
    rx_result_t r = RX_NADA;
    uint8_t quadro[] = { STX, 0, 0, ETX };      /* checksum = QTD = 0   */
    unsigned i;

    rx_init(&rx);

    for (i = 0; i < sizeof(quadro); i++) {
        r = rx_processa(&rx, quadro[i]);
    }

    VERIFICA(r == RX_QUADRO_OK, "quadro sem dados deve ser aceito");
    VERIFICA(rx.qtd == 0, "quadro sem dados tem QTD = 0");
}

static void rx_aceita_dados_iguais_a_stx_e_etx(void)
{
    rx_t rx;
    rx_result_t r = RX_NADA;
    uint8_t dados[2];
    uint8_t quadro[6];
    unsigned i;

    dados[0] = STX;                             /* dados "parecidos" com */
    dados[1] = ETX;                             /* os delimitadores      */
    quadro[0] = STX;
    quadro[1] = 2;
    quadro[2] = dados[0];
    quadro[3] = dados[1];
    quadro[4] = protocolo_checksum(2, dados);
    quadro[5] = ETX;

    rx_init(&rx);

    for (i = 0; i < sizeof(quadro); i++) {
        r = rx_processa(&rx, quadro[i]);
    }

    VERIFICA(r == RX_QUADRO_OK, "campo DADOS pode conter 0x02 e 0x03");
    VERIFICA(rx.dados[0] == STX && rx.dados[1] == ETX, "dados preservados");
}

static void rx_ressincroniza_apos_quadro_corrompido(void)
{
    rx_t rx;
    rx_result_t r = RX_NADA;
    uint8_t corrompido[] = { STX, 1, 'X', 0x00, ETX };  /* checksum errado */
    uint8_t valido[]     = { STX, 1, 'Y', (uint8_t)(1 + 'Y'), ETX };
    unsigned i;

    rx_init(&rx);
    for (i = 0; i < sizeof(corrompido); i++) {
        rx_processa(&rx, corrompido[i]);
    }

    for (i = 0; i < sizeof(valido); i++) {
        r = rx_processa(&rx, valido[i]);
    }

    VERIFICA(r == RX_QUADRO_OK, "quadro seguinte deve ser recebido normalmente");
    VERIFICA(rx.dados[0] == 'Y', "conteudo do quadro seguinte");
}

/* ================================================================== */
/* Transmissor                                                        */
/* ================================================================== */

static void tx_estado_inicial_e_ocioso(void)
{
    tx_t tx;
    uint8_t byte = 0xFF;

    tx_init(&tx);

    VERIFICA(tx.estado == TX_OCIOSO, "apos tx_init a FSM fica ociosa");
    VERIFICA(tx_ocupado(&tx) == 0, "transmissor ocioso nao esta ocupado");
    VERIFICA(tx_processa(&tx, &byte) == 0, "FSM ociosa nao produz bytes");
}

static void tx_carrega_prepara_envio_do_stx(void)
{
    tx_t tx;
    uint8_t dados[2];
    int ok;

    dados[0] = 0x11;
    dados[1] = 0x22;
    tx_init(&tx);

    ok = tx_carrega(&tx, dados, 2);

    VERIFICA(ok == 1, "carga de quadro valido deve ter sucesso");
    VERIFICA(tx.estado == TX_ENVIA_STX, "quadro carregado comeca pelo STX");
    VERIFICA(tx.qtd == 2, "QTD do quadro carregado");
    VERIFICA(tx.chk == (uint8_t)(2 + 0x11 + 0x22), "checksum calculado na carga");
    VERIFICA(tx_ocupado(&tx) == 1, "transmissor com quadro esta ocupado");
}

static void tx_recusa_quadro_maior_que_o_limite(void)
{
    tx_t tx;
    uint8_t dados[PROTO_MAX_DADOS];
    int ok;

    memset(dados, 0, sizeof(dados));
    tx_init(&tx);

    ok = tx_carrega(&tx, dados, PROTO_MAX_DADOS + 1);

    VERIFICA(ok == 0, "quadro acima do limite deve ser recusado");
    VERIFICA(tx.estado == TX_OCIOSO, "FSM continua ociosa apos recusa");
}

static void tx_emite_stx_e_vai_para_qtd(void)
{
    tx_t tx;
    uint8_t dados[1];
    uint8_t byte = 0;
    int enviou;

    dados[0] = 0x77;
    tx_init(&tx);
    tx_carrega(&tx, dados, 1);                  /* estado: ENVIA_STX    */

    enviou = tx_processa(&tx, &byte);

    VERIFICA(enviou == 1, "FSM deve produzir um byte");
    VERIFICA(byte == STX, "primeiro byte do quadro e o STX");
    VERIFICA(tx.estado == TX_ENVIA_QTD, "apos STX vem o QTD");
}

static void tx_emite_qtd_e_vai_para_dados(void)
{
    tx_t tx;
    uint8_t dados[1];
    uint8_t byte = 0;

    dados[0] = 0x77;
    tx_init(&tx);
    tx_carrega(&tx, dados, 1);
    tx_processa(&tx, &byte);                    /* estado: ENVIA_QTD    */

    tx_processa(&tx, &byte);

    VERIFICA(byte == 1, "segundo byte do quadro e o QTD_DADOS");
    VERIFICA(tx.estado == TX_ENVIA_DADOS, "com QTD > 0 vem o campo DADOS");
}

static void tx_qtd_zero_pula_para_checksum(void)
{
    tx_t tx;
    uint8_t byte = 0;

    tx_init(&tx);
    tx_carrega(&tx, 0, 0);
    tx_processa(&tx, &byte);                    /* STX, estado ENVIA_QTD */

    tx_processa(&tx, &byte);

    VERIFICA(byte == 0, "QTD_DADOS = 0");
    VERIFICA(tx.estado == TX_ENVIA_CHK, "quadro sem dados vai direto ao checksum");
}

static void tx_emite_dados_em_ordem(void)
{
    tx_t tx;
    uint8_t dados[2];
    uint8_t byte = 0;

    dados[0] = 0xAA;
    dados[1] = 0xBB;
    tx_init(&tx);
    tx_carrega(&tx, dados, 2);
    tx_processa(&tx, &byte);                    /* STX                   */
    tx_processa(&tx, &byte);                    /* QTD, estado ENVIA_DADOS */

    tx_processa(&tx, &byte);
    VERIFICA(byte == 0xAA, "primeiro dado");
    VERIFICA(tx.estado == TX_ENVIA_DADOS, "ainda ha dados a enviar");

    tx_processa(&tx, &byte);
    VERIFICA(byte == 0xBB, "segundo dado");
    VERIFICA(tx.estado == TX_ENVIA_CHK, "apos o ultimo dado vem o checksum");
}

static void tx_emite_checksum_e_depois_etx(void)
{
    tx_t tx;
    uint8_t dados[1];
    uint8_t byte = 0;

    dados[0] = 0x77;
    tx_init(&tx);
    tx_carrega(&tx, dados, 1);
    tx_processa(&tx, &byte);                    /* STX   */
    tx_processa(&tx, &byte);                    /* QTD   */
    tx_processa(&tx, &byte);                    /* DADOS */

    tx_processa(&tx, &byte);
    VERIFICA(byte == protocolo_checksum(1, dados), "checksum transmitido");
    VERIFICA(tx.estado == TX_ENVIA_ETX, "apos o checksum vem o ETX");

    tx_processa(&tx, &byte);
    VERIFICA(byte == ETX, "ultimo byte do quadro e o ETX");
    VERIFICA(tx.estado == TX_OCIOSO, "fim do quadro deixa a FSM ociosa");
    VERIFICA(tx_ocupado(&tx) == 0, "transmissor livre para o proximo quadro");
}

static void tx_gera_a_sequencia_completa_do_quadro(void)
{
    tx_t tx;
    uint8_t dados[3];
    uint8_t esperado[7];
    uint8_t obtido[16];
    uint8_t byte = 0;
    unsigned n = 0;

    dados[0] = 'A';
    dados[1] = 'B';
    dados[2] = 'C';
    esperado[0] = STX;
    esperado[1] = 3;
    esperado[2] = 'A';
    esperado[3] = 'B';
    esperado[4] = 'C';
    esperado[5] = protocolo_checksum(3, dados);
    esperado[6] = ETX;

    tx_init(&tx);
    tx_carrega(&tx, dados, 3);

    while (tx_processa(&tx, &byte) && n < sizeof(obtido)) {
        obtido[n++] = byte;
    }

    VERIFICA(n == sizeof(esperado), "quadro de 3 dados tem 7 bytes");
    VERIFICA(memcmp(obtido, esperado, sizeof(esperado)) == 0,
             "sequencia | STX | QTD | DADOS | CHK | ETX |");
}

/* ================================================================== */
/* Integracao transmissor -> receptor                                 */
/* ================================================================== */

static void tx_e_rx_trocam_um_quadro(void)
{
    tx_t tx;
    rx_t rx;
    uint8_t dados[] = { 'S', 'E', 'R', 'I', 'A', 'L' };
    uint8_t byte = 0;
    rx_result_t r = RX_NADA;

    tx_init(&tx);
    rx_init(&rx);
    tx_carrega(&tx, dados, sizeof(dados));

    while (tx_processa(&tx, &byte)) {
        r = rx_processa(&rx, byte);
    }

    VERIFICA(r == RX_QUADRO_OK, "receptor deve aceitar o quadro do transmissor");
    VERIFICA(rx.qtd == sizeof(dados), "QTD recebido igual ao transmitido");
    VERIFICA(memcmp(rx.dados, dados, sizeof(dados)) == 0, "dados intactos");
}

static void tx_e_rx_trocam_varios_quadros(void)
{
    tx_t tx;
    rx_t rx;
    uint8_t dados[PROTO_MAX_DADOS];
    uint8_t byte = 0;
    rx_result_t r;
    int quadros_ok = 0;
    unsigned tamanhos[] = { 0, 1, PROTO_MAX_DADOS };
    unsigned i, j;

    for (i = 0; i < sizeof(dados); i++) {
        dados[i] = (uint8_t)(i * 7);
    }

    tx_init(&tx);
    rx_init(&rx);

    for (i = 0; i < sizeof(tamanhos) / sizeof(tamanhos[0]); i++) {
        tx_carrega(&tx, dados, (uint8_t)tamanhos[i]);
        while (tx_processa(&tx, &byte)) {
            r = rx_processa(&rx, byte);
            if (r == RX_QUADRO_OK) {
                quadros_ok++;
                VERIFICA(rx.qtd == tamanhos[i], "QTD do quadro recebido");
                for (j = 0; j < tamanhos[i]; j++) {
                    VERIFICA(rx.dados[j] == dados[j], "dado recebido");
                }
            }
        }
    }

    VERIFICA(quadros_ok == 3, "os tres quadros devem ser recebidos em sequencia");
}

/* ================================================================== */

static void checksum_soma_qtd_e_dados_modulo_256(void)
{
    uint8_t dados[3];

    dados[0] = 0xFF;
    dados[1] = 0xFF;
    dados[2] = 0x02;

    VERIFICA(protocolo_checksum(0, 0) == 0, "quadro sem dados tem checksum 0");
    VERIFICA(protocolo_checksum(3, dados) == (uint8_t)(3 + 0xFF + 0xFF + 0x02),
             "checksum soma QTD e DADOS com estouro em 8 bits");
}

int main(void)
{
    printf("Testes das FSMs do protocolo |STX|QTD_DADOS|DADOS|CHK|ETX|\n\n");

    TESTE(checksum_soma_qtd_e_dados_modulo_256);

    TESTE(rx_estado_inicial_e_espera_stx);
    TESTE(rx_ignora_lixo_fora_de_quadro);
    TESTE(rx_stx_leva_para_espera_qtd);
    TESTE(rx_qtd_valida_leva_para_recebe_dados);
    TESTE(rx_qtd_zero_pula_para_checksum);
    TESTE(rx_qtd_acima_do_limite_gera_erro);
    TESTE(rx_dados_sao_armazenados_em_ordem);
    TESTE(rx_checksum_correto_leva_para_espera_etx);
    TESTE(rx_checksum_errado_gera_erro);
    TESTE(rx_etx_completa_o_quadro);
    TESTE(rx_sem_etx_gera_erro);
    TESTE(rx_recebe_quadro_completo);
    TESTE(rx_recebe_quadro_sem_dados);
    TESTE(rx_aceita_dados_iguais_a_stx_e_etx);
    TESTE(rx_ressincroniza_apos_quadro_corrompido);

    TESTE(tx_estado_inicial_e_ocioso);
    TESTE(tx_carrega_prepara_envio_do_stx);
    TESTE(tx_recusa_quadro_maior_que_o_limite);
    TESTE(tx_emite_stx_e_vai_para_qtd);
    TESTE(tx_emite_qtd_e_vai_para_dados);
    TESTE(tx_qtd_zero_pula_para_checksum);
    TESTE(tx_emite_dados_em_ordem);
    TESTE(tx_emite_checksum_e_depois_etx);
    TESTE(tx_gera_a_sequencia_completa_do_quadro);

    TESTE(tx_e_rx_trocam_um_quadro);
    TESTE(tx_e_rx_trocam_varios_quadros);

    printf("\n%d verificacoes, %d falha(s).\n", total_verificacoes, falhas);
    printf("%s\n", (falhas == 0) ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS");

    return (falhas == 0) ? 0 : 1;
}
