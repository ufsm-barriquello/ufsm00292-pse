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
#define ACK_BYTE 0x06
#define NACK_BYTE 0x15
#define TIMEOUT_ESPERA 100
#define MAX_TENTATIVAS 3

// --- FSM Receptora (RX) ---
typedef enum {
  RX_ESPERA_STX,
  RX_ESPERA_QTD,
  RX_RECEBENDO_DADOS,
  RX_ESPERA_CHK,
  RX_ESPERA_ETX
} rx_estado_t;

typedef struct {
  rx_estado_t estado;
  uint8_t buffer_dados[MAX_DADOS];
  uint8_t qtd_dados, contador, chk_calculado, chk_recebido;
  bool pacote_pronto;
} maquina_estados_rx_t;

// --- FSM Transmissora (TX) ---
typedef enum {
  TX_IDLE,
  TX_ENVIANDO,
  TX_ESPERANDO_ACK,
  TX_SUCESSO,
  TX_FALHA
} tx_estado_t;

typedef struct {
  tx_estado_t estado;
  uint8_t tentativas;
  uint32_t tempo_limite;
  uint8_t pacote_a_enviar[MAX_DADOS + 5];
  int tamanho_pacote;
} maquina_estados_tx_t;

// --- Ambiente Simulado ---
uint8_t buffer_tx_para_rx[32], buffer_rx_para_tx[4];
int count_tx_para_rx = 0, count_rx_para_tx = 0;
uint32_t g_tempo_simulado = 0;
bool g_pacote_recebido_com_sucesso = false;
uint8_t g_payload_recebido[MAX_DADOS];
int g_tamanho_payload_recebido = 0;

maquina_estados_tx_t g_fsm_tx;
maquina_estados_rx_t g_fsm_rx;

// --- Funções da FSM Receptora ---
void maquina_estados_rx_init(maquina_estados_rx_t *fsm) {
  fsm->estado = RX_ESPERA_STX;
  fsm->pacote_pronto = false;
}

void maquina_estados_rx_processa_byte(maquina_estados_rx_t *fsm, uint8_t byte) {
  fsm->pacote_pronto = false;

  switch (fsm->estado) {
  case RX_ESPERA_STX:
    if (byte == STX) {
      fsm->chk_calculado = 0;
      fsm->contador = 0;
      fsm->estado = RX_ESPERA_QTD;
    }
    break;

  case RX_ESPERA_QTD:
    if (byte <= MAX_DADOS) {
      fsm->qtd_dados = byte;
      fsm->chk_calculado = byte; // Checksum inicia com a quantidade
      fsm->estado =
          (byte == 0) ? RX_ESPERA_CHK : RX_RECEBENDO_DADOS;
    } else {
      fsm->estado = RX_ESPERA_STX; // Erro, quantidade excede o buffer
    }
    break;

  case RX_RECEBENDO_DADOS:
    fsm->buffer_dados[fsm->contador] = byte;
    fsm->chk_calculado ^= byte;
    fsm->contador++;
    if (fsm->contador >= fsm->qtd_dados) {
      fsm->estado = RX_ESPERA_CHK;
    }
    break;

  case RX_ESPERA_CHK:
    fsm->chk_recebido = byte;
    fsm->estado = RX_ESPERA_ETX;
    break;

  case RX_ESPERA_ETX:
    if (byte == ETX && fsm->chk_calculado == fsm->chk_recebido) {
      fsm->pacote_pronto = true;
    }
    fsm->estado = RX_ESPERA_STX;
    break;
  }
}

// --- Funções da FSM Transmissora ---
void maquina_estados_tx_init(maquina_estados_tx_t *fsm) {
    fsm->estado = TX_IDLE;
    fsm->tentativas = 0;
}

void scheduler_transmissora(bool iniciar_envio, uint8_t *payload, int tamanho) {
  switch (g_fsm_tx.estado) {
  case TX_IDLE:
    if (iniciar_envio) {
      g_fsm_tx.pacote_a_enviar[0] = STX;
      g_fsm_tx.pacote_a_enviar[1] = tamanho;
      memcpy(&g_fsm_tx.pacote_a_enviar[2], payload, tamanho);
      uint8_t chk = tamanho; // Checksum inicia com a quantidade
      for (int i = 0; i < tamanho; i++) {
        chk ^= payload[i];
      }
      g_fsm_tx.pacote_a_enviar[2 + tamanho] = chk;
      g_fsm_tx.pacote_a_enviar[3 + tamanho] = ETX;
      g_fsm_tx.tamanho_pacote = 4 + tamanho;
      g_fsm_tx.tentativas = 0;
      g_fsm_tx.estado = TX_ENVIANDO;
    }
    break;

  case TX_ENVIANDO:
    memcpy(buffer_tx_para_rx, g_fsm_tx.pacote_a_enviar,
           g_fsm_tx.tamanho_pacote);
    count_tx_para_rx = g_fsm_tx.tamanho_pacote;
    g_fsm_tx.tentativas++;
    g_fsm_tx.tempo_limite = g_tempo_simulado + TIMEOUT_ESPERA;
    g_fsm_tx.estado = TX_ESPERANDO_ACK;
    break;

  case TX_ESPERANDO_ACK:
    if (count_rx_para_tx > 0 && buffer_rx_para_tx[0] == ACK_BYTE) {
      count_rx_para_tx = 0;
      g_fsm_tx.estado = TX_SUCESSO;
    } else if (g_tempo_simulado >= g_fsm_tx.tempo_limite) {
      if (g_fsm_tx.tentativas < MAX_TENTATIVAS) {
        g_fsm_tx.estado = TX_ENVIANDO;
      } else {
        g_fsm_tx.estado = TX_FALHA;
      }
    }
    break;

  case TX_SUCESSO:
  case TX_FALHA:
    // Permanece no estado final
    break;
  }
}

void scheduler_receptora() {
  if (count_tx_para_rx > 0) {
    uint8_t byte = buffer_tx_para_rx[0];
    memmove(&buffer_tx_para_rx[0], &buffer_tx_para_rx[1], --count_tx_para_rx);

    maquina_estados_rx_processa_byte(&g_fsm_rx, byte);

    if (g_fsm_rx.pacote_pronto) {
      g_pacote_recebido_com_sucesso = true;
      g_tamanho_payload_recebido = g_fsm_rx.qtd_dados;
      memcpy(g_payload_recebido, g_fsm_rx.buffer_dados, g_fsm_rx.qtd_dados);

      buffer_rx_para_tx[0] = ACK_BYTE;
      count_rx_para_tx = 1;
    }
  }
}

// --- Funções de Teste ---
void resetar_ambiente() {
  g_tempo_simulado = 0;
  g_pacote_recebido_com_sucesso = false;
  count_tx_para_rx = 0;
  count_rx_para_tx = 0;
  maquina_estados_rx_init(&g_fsm_rx);
  maquina_estados_tx_init(&g_fsm_tx);
}

char *teste_transmissao_com_sucesso() {
  resetar_ambiente();
  uint8_t payload[] = "OLA";

  while (g_fsm_tx.estado != TX_SUCESSO && g_fsm_tx.estado != TX_FALHA &&
         g_tempo_simulado < 200) {
    scheduler_transmissora(g_tempo_simulado == 10, payload, 3);
    scheduler_receptora();
    g_tempo_simulado++;
  }

  verifica("O transmissor deveria terminar com sucesso",
           g_fsm_tx.estado == TX_SUCESSO);
  verifica("O pacote deveria ter sido recebido", g_pacote_recebido_com_sucesso);
  verifica("O payload recebido deve ser 'OLA'",
           strncmp((char *)g_payload_recebido, "OLA", 3) == 0);
  verifica("Deveria ter sido necessária apenas 1 tentativa",
           g_fsm_tx.tentativas == 1);
  return 0;
}

char *teste_retransmissao_apos_timeout() {
  resetar_ambiente();
  uint8_t payload[] = "DADO";

  while (g_fsm_tx.estado != TX_SUCESSO && g_fsm_tx.estado != TX_FALHA &&
         g_tempo_simulado < 500) {

    // Apenas na primeira tentativa, "perdemos" o ACK para forçar o timeout
    if (g_fsm_tx.tentativas == 1 && g_fsm_tx.estado == TX_ESPERANDO_ACK && count_rx_para_tx > 0) {
      count_rx_para_tx = 0; // Simula a perda do ACK
    }

    scheduler_transmissora(g_tempo_simulado == 10, payload, 4);
    scheduler_receptora();

    g_tempo_simulado++;
  }

  verifica("O transmissor deveria terminar com sucesso",
           g_fsm_tx.estado == TX_SUCESSO);
  verifica("O pacote foi recebido eventualmente",
           g_pacote_recebido_com_sucesso);
  verifica("Foram necessárias 2 tentativas (1 retransmissão)",
           g_fsm_tx.tentativas == 2);
  return 0;
}

char *roda_testes() {
  executa_teste(teste_transmissao_com_sucesso);
  executa_teste(teste_retransmissao_apos_timeout);
  return 0;
}

int main() {
  printf("--- Rodando Testes ---\n");
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