#include "proto_comm.h"
#include <stdio.h>

void canal_init(CanalComunicacao *canal) {
    if (!canal) return;
    canal->barramento = 0;
    canal->novo_byte_disponivel = false;
    canal->canal_ack = 0;
    canal->novo_ack_disponivel = false;
}

void pt_tx_init(PT_Transmissor *tx, const uint8_t *dados, uint8_t qtd) {
    if (!tx || !dados) return;
    PT_INIT(&tx->pt);
    tx->transmissao_concluida = false;
    tx->ack_recebido = false;
    tx->retransmissoes = 0;
    tx->timer_ticks = 0;

    size_t idx = 0;
    uint8_t chk = 0;
    tx->buffer_envio[idx++] = STX_BYTE;
    tx->buffer_envio[idx++] = qtd;

    for (uint8_t i = 0; i < qtd; i++) {
        tx->buffer_envio[idx++] = dados[i];
        chk += dados[i];
    }
    tx->buffer_envio[idx++] = chk;
    tx->buffer_envio[idx++] = ETX_BYTE;
    tx->tamanho_pacote = idx;
}

void pt_rx_init(PT_Receptor *rx) {
    if (!rx) return;
    PT_INIT(&rx->pt);
    rx->qtd_esperada = 0;
    rx->idx_dados = 0;
    rx->chk_calculado = 0;
    rx->pacote_valido = false;
}

/* Protothread Transmissora (TX) */
char pt_transmissor_thread(PT_Transmissor *tx, CanalComunicacao *canal) {
    static size_t idx_tx;

    PT_BEGIN(&tx->pt);

    while (!tx->transmissao_concluida) {
        /* Transmite o pacote byte por byte */
        for (idx_tx = 0; idx_tx < tx->tamanho_pacote; idx_tx++) {
            PT_WAIT_UNTIL(&tx->pt, !canal->novo_byte_disponivel);
            canal->barramento = tx->buffer_envio[idx_tx];
            canal->novo_byte_disponivel = true;
        }

        /* Aguarda Resposta de ACK no canal */
        tx->timer_ticks = 0;
        while (tx->timer_ticks < TIMEOUT_TICKS && !tx->transmissao_concluida) {
            PT_WAIT_UNTIL(&tx->pt, canal->novo_ack_disponivel || (tx->timer_ticks >= TIMEOUT_TICKS));

            if (canal->novo_ack_disponivel) {
                if (canal->canal_ack == ACK_BYTE) {
                    tx->ack_recebido = true;
                    tx->transmissao_concluida = true;
                }
                canal->novo_ack_disponivel = false;
                break;
            }
            tx->timer_ticks++;
        }

        /* Se estourou o tempo e não foi concluído, marca retransmissão */
        if (!tx->transmissao_concluida) {
            tx->retransmissoes++;
        }
    }

    PT_END(&tx->pt);
}

/* Protothread Receptora (RX) */
char pt_receptor_thread(PT_Receptor *rx, CanalComunicacao *canal) {
    static uint8_t chk_rx;
    static uint8_t etx_rx;

    PT_BEGIN(&rx->pt);

    while (1) {
        /* 1. Aguarda STX */
        PT_WAIT_UNTIL(&rx->pt, canal->novo_byte_disponivel);
        if (canal->barramento != STX_BYTE) {
            canal->novo_byte_disponivel = false;
            continue;
        }
        canal->novo_byte_disponivel = false;

        /* 2. Aguarda QTD */
        PT_WAIT_UNTIL(&rx->pt, canal->novo_byte_disponivel);
        rx->qtd_esperada = canal->barramento;
        canal->novo_byte_disponivel = false;
        rx->idx_dados = 0;
        rx->chk_calculado = 0;

        /* 3. Recebe os Dados */
        while (rx->idx_dados < rx->qtd_esperada) {
            PT_WAIT_UNTIL(&rx->pt, canal->novo_byte_disponivel);
            rx->buffer_recebido[rx->idx_dados] = canal->barramento;
            rx->chk_calculado += canal->barramento;
            rx->idx_dados++;
            canal->novo_byte_disponivel = false;
        }

        /* 4. Aguarda CHK */
        PT_WAIT_UNTIL(&rx->pt, canal->novo_byte_disponivel);
        chk_rx = canal->barramento;
        canal->novo_byte_disponivel = false;

        /* 5. Aguarda ETX */
        PT_WAIT_UNTIL(&rx->pt, canal->novo_byte_disponivel);
        etx_rx = canal->barramento;
        canal->novo_byte_disponivel = false;

        /* Valida e Responde ACK / NACK */
        if (chk_rx == rx->chk_calculado && etx_rx == ETX_BYTE) {
            rx->pacote_valido = true;
            canal->canal_ack = ACK_BYTE;
        } else {
            rx->pacote_valido = false;
            canal->canal_ack = NACK_BYTE;
        }
        canal->novo_ack_disponivel = true;
    }

    PT_END(&rx->pt);
}