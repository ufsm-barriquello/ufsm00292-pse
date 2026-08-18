#include <stdio.h>
#include <string.h>
#include "protocolo.h"

#define verifica(msg, teste) do { if (!(teste)) return msg; } while (0)
#define executa_teste(teste) do { char *m = teste(); testes_executados++; if (m) return m; } while (0)

int testes_executados = 0;

static char * teste_tx_montagem_pacote(void) {
    uint8_t payload[] = {0x10, 0x20, 0x30};
    uint8_t pacote[10];
    size_t tam = tx_monta_pacote(payload, 3, pacote);

    verifica("Erro TX: Tamanho do pacote incorreto", tam == 7);
    verifica("Erro TX: STX incorreto", pacote[0] == 0x02);
    verifica("Erro TX: QTD incorreta", pacote[1] == 3);
    verifica("Erro TX: Checksum incorreto (deve ser 0x60)", pacote[5] == 0x60);
    verifica("Erro TX: ETX incorreto", pacote[6] == 0x03);

    return 0;
}

static char * teste_rx_recepcao_sucesso(void) {
    ProtocoloRX rx;
    rx_init(&rx);

    // Pacote: STX, QTD=2, D1=0x05, D2=0x05, CHK=0x0A, ETX
    uint8_t pacote_valido[] = {0x02, 0x02, 0x05, 0x05, 0x0A, 0x03};

    bool concluido = false;
    for (size_t i = 0; i < sizeof(pacote_valido); i++) {
        concluido = rx_processa_byte(&rx, pacote_valido[i]);
    }

    verifica("Erro RX: Pacote valido nao foi aceito", concluido == true);
    verifica("Erro RX: Estado final deve ser RX_PACOTE_VALIDO", rx.estado == RX_PACOTE_VALIDO);
    verifica("Erro RX: Dado 0 corrompido", rx.buffer_dados[0] == 0x05);
    verifica("Erro RX: Dado 1 corrompido", rx.buffer_dados[1] == 0x05);

    return 0;
}

static char * teste_rx_checksum_incorreto(void) {
    ProtocoloRX rx;
    rx_init(&rx);

    // Pacote com CHK errado (esperado 0x0A, enviado 0xFF)
    uint8_t pacote_corrompido[] = {0x02, 0x02, 0x05, 0x05, 0xFF, 0x03};

    bool concluido = false;
    for (size_t i = 0; i < sizeof(pacote_corrompido); i++) {
        concluido = rx_processa_byte(&rx, pacote_corrompido[i]);
    }

    verifica("Erro RX: Pacote com CHK invalido foi aceito incorretamente", concluido == false);
    verifica("Erro RX: FSM nao resetou apos erro de CHK", rx.estado == RX_AGUARDA_STX);

    return 0;
}

static char * executa_todos_testes(void) {
    executa_teste(teste_tx_montagem_pacote);
    executa_teste(teste_rx_recepcao_sucesso);
    executa_teste(teste_rx_checksum_incorreto);
    return 0;
}

int main(void) {
    printf("===========================================\n");
    printf("  EXECUTANDO TESTES FSM PROTOCOLO \n");
    printf("===========================================\n");

    char *resultado = executa_todos_testes();

    if (resultado != 0) {
        printf("[FALHA] %s\n", resultado);
    } else {
        printf("[SUCESSO] TODOS OS TESTES DA FSM PASSARAM!\n");
    }
    printf("Total de testes executados: %d\n", testes_executados);
    printf("===========================================\n");

    return (resultado != 0);
}