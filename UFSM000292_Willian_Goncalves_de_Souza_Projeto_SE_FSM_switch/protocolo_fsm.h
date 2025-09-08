#ifndef PROTOCOLO_FSM_H
#define PROTOCOLO_FSM_H

#include <stdint.h>

/* Estados da FSM */
typedef enum {
    ESTADO_IDLE,
    ESTADO_AGUARDA_QTD,
    ESTADO_PROCESS_DADOS,
    ESTADO_AGUARDA_CHK,
    ESTADO_COMPLETA,
    ESTADO_ERRO
} EstadoProtocolo;

/* Estrutura do protocolo */
typedef struct {
    EstadoProtocolo estado_atual;
    uint8_t qtd_dados_esperada;
    uint8_t dados_recebidos[256];
    uint8_t indice_dados;
    uint8_t checksum_calculado;
    uint8_t checksum_recebido;
    uint8_t pacote_completo;
    uint8_t dados_processados[256];
    uint8_t tamanho_processado;
} MaquinaProtocolo;

/* Protótipos das funções */
void maquina_init(MaquinaProtocolo *maq);
EstadoProtocolo maquina_processa_evento(MaquinaProtocolo *maq, uint8_t evento);
uint8_t maquina_pacote_completo(MaquinaProtocolo *maq);
const uint8_t* maquina_obter_dados(MaquinaProtocolo *maq, uint8_t *tamanho);

/* Constantes do protocolo */
#define STX 0x02
#define ETX 0x03

#endif /* PROTOCOLO_FSM_H */
