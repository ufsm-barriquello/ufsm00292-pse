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

typedef struct maquina_estados_rx_s maquina_estados_rx_t;
typedef void (*estado_handler_t)(maquina_estados_rx_t *fsm, uint8_t byte);

struct maquina_estados_rx_s {
  estado_handler_t estado_atual;
  uint8_t buffer_dados[MAX_DADOS];
  uint8_t qtd_dados;
  uint8_t contador;
  uint8_t chk_calculado;
  uint8_t chk_recebido;
  bool pacote_pronto;
};

void handle_espera_stx(maquina_estados_rx_t *fsm, uint8_t byte);
void handle_espera_qtd(maquina_estados_rx_t *fsm, uint8_t byte);
void handle_recebendo_dados(maquina_estados_rx_t *fsm, uint8_t byte);
void handle_espera_chk(maquina_estados_rx_t *fsm, uint8_t byte);
void handle_espera_etx(maquina_estados_rx_t *fsm, uint8_t byte);

void handle_espera_stx(maquina_estados_rx_t *fsm, uint8_t byte) {
  if (byte == STX) {
    fsm->qtd_dados = 0;
    fsm->contador = 0;
    fsm->chk_calculado = 0;
    fsm->estado_atual = handle_espera_qtd;
  }
}

void handle_espera_qtd(maquina_estados_rx_t *fsm, uint8_t byte) {
  if (byte <= MAX_DADOS) {
    fsm->qtd_dados = byte;
    fsm->estado_atual =
        (byte == 0) ? handle_espera_chk : handle_recebendo_dados;
  } else {
    fsm->estado_atual = handle_espera_stx;
  }
}

void handle_recebendo_dados(maquina_estados_rx_t *fsm, uint8_t byte) {
  fsm->buffer_dados[fsm->contador] = byte;
  fsm->chk_calculado ^= byte;
  fsm->contador++;
  if (fsm->contador == fsm->qtd_dados) {
    fsm->estado_atual = handle_espera_chk;
  }
}

void handle_espera_chk(maquina_estados_rx_t *fsm, uint8_t byte) {
  fsm->chk_recebido = byte;
  fsm->estado_atual = handle_espera_etx;
}

void handle_espera_etx(maquina_estados_rx_t *fsm, uint8_t byte) {
  if (byte == ETX && fsm->chk_calculado == fsm->chk_recebido) {
    fsm->pacote_pronto = true;
  }
  fsm->estado_atual = handle_espera_stx;
}

void maquina_estados_init(maquina_estados_rx_t *fsm) {
  fsm->estado_atual = handle_espera_stx;
  fsm->pacote_pronto = false;
}

bool processa_byte(maquina_estados_rx_t *fsm, uint8_t byte) {
  fsm->pacote_pronto = false;
  fsm->estado_atual(fsm, byte);
  return fsm->pacote_pronto;
}

// --- Testes ---

char *teste_pacote_correto() {
  maquina_estados_rx_t maquina;
  maquina_estados_init(&maquina);

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
  maquina_estados_rx_t maquina;
  maquina_estados_init(&maquina);

  processa_byte(&maquina, 0xAA);
  processa_byte(&maquina, 0xBB);
  verifica("Estado deveria continuar em 'handle_espera_stx'",
           maquina.estado_atual == handle_espera_stx);

  uint8_t pacote[] = {STX, 1, 'B', 'B', ETX};
  bool pacote_ok = false;
  for (int i = 0; i < sizeof(pacote); i++) {
    pacote_ok = processa_byte(&maquina, pacote[i]);
  }

  verifica("Deveria aceitar pacote depois do lixo", pacote_ok);
  return 0;
}

char *teste_checksum_errado() {
  maquina_estados_rx_t maquina;
  maquina_estados_init(&maquina);

  uint8_t checksum_correto = 'A';
  uint8_t pacote[] = {STX, 1, 'A', checksum_correto + 1, ETX};
  bool pacote_ok = false;

  for (int i = 0; i < sizeof(pacote); i++) {
    pacote_ok = processa_byte(&maquina, pacote[i]);
  }

  verifica("Nao deveria aceitar pacote com checksum errado", !pacote_ok);
  verifica("Maquina deveria voltar ao estado inicial",
           maquina.estado_atual == handle_espera_stx);
  return 0;
}

char *roda_testes() {
  executa_teste(teste_pacote_correto);
  executa_teste(teste_ignora_lixo_antes);
  executa_teste(teste_checksum_errado);
  return 0;
}

// --- Main ---
int main() {
  printf("--- Rodando testes (FSM com Ponteiros de Função) ---\n");
  char *resultado = roda_testes();

  if (resultado) {
    printf("--> Motivo da falha: %s\n", resultado);
    return 1;
  }

  printf("\n---------------------------------\n");
  printf("TODOS OS %d TESTES PASSARAM!\n", testes_executados);
  printf("---------------------------------\n");
  return 0;
}