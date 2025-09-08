#include "protocolo_fsm.h"
#include <string.h>

void maquina_init(MaquinaProtocolo *maq) {
    maq->estado_atual = ESTADO_IDLE;
    maq->qtd_dados_esperada = 0;
    maq->indice_dados = 0;
    maq->checksum_calculado = 0;
    maq->checksum_recebido = 0;
    maq->pacote_completo = 0;
    maq->tamanho_processado = 0;
    memset(maq->dados_recebidos, 0, sizeof(maq->dados_recebidos));
    memset(maq->dados_processados, 0, sizeof(maq->dados_processados));
}

EstadoProtocolo maquina_processa_evento(MaquinaProtocolo *maq, uint8_t evento) {
    switch (maq->estado_atual) {
        case ESTADO_IDLE:
            if (evento == STX) {
                maq->estado_atual = ESTADO_AGUARDA_QTD;
                maq->checksum_calculado = STX;
                maq->pacote_completo = 0;
            }
            break;

        case ESTADO_AGUARDA_QTD:
            maq->qtd_dados_esperada = evento;
            maq->checksum_calculado ^= evento;
            maq->estado_atual = ESTADO_PROCESS_DADOS;
            maq->indice_dados = 0;
            break;

        case ESTADO_PROCESS_DADOS:
            if (maq->indice_dados < maq->qtd_dados_esperada) {
                maq->dados_recebidos[maq->indice_dados] = evento;
                maq->checksum_calculado ^= evento;
                maq->indice_dados++;

                if (maq->indice_dados == maq->qtd_dados_esperada) {
                    maq->estado_atual = ESTADO_AGUARDA_CHK;
                }
            } else {
                maq->estado_atual = ESTADO_ERRO;
            }
            break;

        case ESTADO_AGUARDA_CHK:
            maq->checksum_recebido = evento;
            maq->estado_atual = ESTADO_COMPLETA;
            break;

        case ESTADO_COMPLETA:
            if (evento == ETX) {
                if (maq->checksum_calculado == maq->checksum_recebido) {
                    // Pacote válido - copia dados para processamento
                    memcpy(maq->dados_processados, maq->dados_recebidos, maq->qtd_dados_esperada);
                    maq->tamanho_processado = maq->qtd_dados_esperada;
                    maq->pacote_completo = 1;
                } else {
                    maq->estado_atual = ESTADO_ERRO;
                }
            } else {
                maq->estado_atual = ESTADO_ERRO;
            }
            // Volta para IDLE independente do resultado
            maq->estado_atual = ESTADO_IDLE;
            break;

        case ESTADO_ERRO:
            // Permanece em erro até receber STX
            if (evento == STX) {
                maq->estado_atual = ESTADO_AGUARDA_QTD;
                maq->checksum_calculado = STX;
                maq->pacote_completo = 0;
            }
            break;

        default:
            maq->estado_atual = ESTADO_ERRO;
            break;
    }

    return maq->estado_atual;
}

uint8_t maquina_pacote_completo(MaquinaProtocolo *maq) {
    return maq->pacote_completo;
}

const uint8_t* maquina_obter_dados(MaquinaProtocolo *maq, uint8_t *tamanho) {
    if (tamanho) {
        *tamanho = maq->tamanho_processado;
    }
    return maq->dados_processados;
}
