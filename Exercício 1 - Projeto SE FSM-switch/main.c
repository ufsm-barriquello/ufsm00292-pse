#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define verifica(msg, teste)                                                   \
  do {                                                                         \
    if (!(teste))                                                              \
      return msg;                                                              \
  } while (0)
#define executa_teste(teste)                                                   \
  do {                                                                         \
    char *msg = teste();                                                       \
    testes_executados++;                                                       \
    if (msg) {                                                                 \
      printf("FALHOU: %s\n", #teste);                                          \
      return msg;                                                              \
    } else {                                                                   \
      printf("Passou: %s\n", #teste);                                          \
    }                                                                          \
  } while (0)

int testes_executados = 0;

#define MAX_DADOS 10
#define STX 0x02
#define ETX 0x03

typedef enum {
  ESPERA_STX,
  ESPERA_QTD,
  RECEBENDO_DADOS,
  ESPERA_CHK,
  ESPERA_ETX
} estado_rx_t;

typedef struct {
  estado_rx_t estado;
  uint8_t buffer_dados[MAX_DADOS];
  uint8_t qtd_dados;
  uint8_t contador;
  uint8_t chk_calculado;
  uint8_t chk_recebido;
} maquina_estados_rx_t;

bool processa_byte(maquina_estados_rx_t *rx, uint8_t byte) {
  switch (rx->estado) {
  case ESPERA_STX:
    if (byte == STX) {
      rx->estado = ESPERA_QTD;
      rx->qtd_dados = 0;
      rx->contador = 0;
      rx->chk_calculado = 0;
    }
    break;

  case ESPERA_QTD:
    rx->qtd_dados = byte;
    if (byte == 0) {
      rx->estado = ESPERA_CHK;
    } else {
      rx->estado = RECEBENDO_DADOS;
    }
    break;

  case RECEBENDO_DADOS:
    rx->buffer_dados[rx->contador] = byte;
    rx->chk_calculado ^= byte;
    rx->contador++;
    if (rx->contador == rx->qtd_dados) {
      rx->estado = ESPERA_CHK;
    }
    break;

  case ESPERA_CHK:
    rx->chk_recebido = byte;
    rx->estado = ESPERA_ETX;
    break;

  case ESPERA_ETX:
    if (byte == ETX && rx->chk_calculado == rx->chk_recebido) {
      rx->estado = ESPERA_STX;
      return true;
    }
    rx->estado = ESPERA_STX;
    break;
  }
  return false;
}

// --- Testes ---

char *teste_pacote_correto() {
  maquina_estados_rx_t maquina = {.estado = ESPERA_STX};

  uint8_t checksum = 'O' ^ 'L' ^ 'A';
  uint8_t pacote[] = {STX, 3, 'O', 'L', 'A', checksum, ETX};
  bool pacote_ok = false;

  for (int i = 0; i < sizeof(pacote); i++) {
    pacote_ok = processa_byte(&maquina, pacote[i]);
  }

  verifica("Pacote deveria ter sido aceito", pacote_ok);
  verifica("Dados deveriam ser 'OLA'",
           strncmp((char *)maquina.buffer_dados, "OLA", 3) == 0);
  return 0;
}

char *teste_ignora_lixo_antes() {
  maquina_estados_rx_t maquina = {.estado = ESPERA_STX};
  processa_byte(&maquina, 0xAA);
  processa_byte(&maquina, 0xBB);

  uint8_t pacote[] = {STX, 1, 'B', 'B', ETX};
  bool pacote_ok = false;
  for (int i = 0; i < sizeof(pacote); i++) {
    pacote_ok = processa_byte(&maquina, pacote[i]);
  }

  verifica("Deveria aceitar pacote depois do lixo", pacote_ok);
  return 0;
}

char *teste_checksum_errado() {
  maquina_estados_rx_t maquina = {.estado = ESPERA_STX};

  uint8_t checksum_correto = 'A';
  uint8_t pacote[] = {STX, 1, 'A', checksum_correto + 1, ETX};
  bool pacote_ok = false;

  for (int i = 0; i < sizeof(pacote); i++) {
    pacote_ok = processa_byte(&maquina, pacote[i]);
  }

  verifica("Nao deveria aceitar pacote com checksum errado", !pacote_ok);
  verifica("Maquina deveria voltar a esperar por STX",
           maquina.estado == ESPERA_STX);
  return 0;
}

char *roda_testes() {
  executa_teste(teste_pacote_correto);
  executa_teste(teste_ignora_lixo_antes);
  executa_teste(teste_checksum_errado);
  return 0;
}

int main() {
  printf("--- Rodando testes ---\n");
  char *resultado = roda_testes();

  if (resultado) {
    printf("--> Motivo da falha: %s\n", resultado);
    return 1;
  }

  printf("\n-----------------------------\n");
  printf("TODOS OS %d TESTES PASSARAM!\n", testes_executados);
  printf("-----------------------------\n");
  return 0;
}