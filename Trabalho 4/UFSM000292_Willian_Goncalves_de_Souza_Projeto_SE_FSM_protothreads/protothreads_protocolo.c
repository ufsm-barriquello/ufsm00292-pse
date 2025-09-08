// ============================================================
// Arquivo: protothreads_protocolo.c
// ============================================================
#include "protothreads_protocolo.h"

void ch_init(channel_t *ch) {
    ch->head = ch->tail = ch->count = 0;
}

int ch_put(channel_t *ch, uint8_t b) {
    if (ch->count == CH_SIZE) return -1;
    ch->buf[ch->head] = b;
    ch->head = (ch->head + 1) % CH_SIZE;
    ch->count++;
    return 0;
}

int ch_get(channel_t *ch, uint8_t *b) {
    if (ch->count == 0) return -1;
    *b = ch->buf[ch->tail];
    ch->tail = (ch->tail + 1) % CH_SIZE;
    ch->count--;
    return 0;
}

void protocolo_init(transmissor_t *tx, receptor_t *rx) {
    if (tx) {
        PT_INIT(&tx->pt);
        tx->tamanho_dados = 0;
        tx->tem_byte_saida = 0;
        tx->ack_ok = 0;
        tx->tentativas = 0;
        tx->deadline = 0;
    }
    if (rx) {
        PT_INIT(&rx->pt);
        rx->qtd_esperada = 0;
        rx->indice = 0;
        rx->checksum_calculado = 0;
        rx->checksum_recebido = 0;
        rx->pacote_completo = 0;
        rx->resposta = 0;
        rx->estado = 0;
    }
}

uint8_t calcular_checksum(const uint8_t *dados, uint8_t tamanho) {
    uint8_t chk = STX ^ tamanho;
    for (int i = 0; i < tamanho; i++) chk ^= dados[i];
    return chk;
}

int transmissor_thread(transmissor_t *tx, channel_t *canal_tx, channel_t *canal_rx, unsigned long ticks) {
    PT_BEGIN(&tx->pt);
    while (1) {
        if (tx->tamanho_dados == 0) {
            PT_YIELD(&tx->pt);
            continue;
        }
        tx->tentativas = 0;
        tx->ack_ok = 0;
        while (tx->tentativas < MAX_TENTATIVAS && !tx->ack_ok) {
            ch_put(canal_tx, STX);
            ch_put(canal_tx, tx->tamanho_dados);
            for (int i = 0; i < tx->tamanho_dados; i++) ch_put(canal_tx, tx->dados[i]);
            ch_put(canal_tx, calcular_checksum(tx->dados, tx->tamanho_dados));
            ch_put(canal_tx, ETX);
            tx->tentativas++;
            tx->deadline = ticks + TIMEOUT_TICKS;
            while (ticks < tx->deadline && !tx->ack_ok) {
                uint8_t b;
                if (ch_get(canal_rx, &b) == 0) {
                    if (b == ACK) { tx->ack_ok = 1; }
                    else if (b == NAK) { break; }
                }
                PT_YIELD(&tx->pt);
            }
        }
        tx->tamanho_dados = 0;
    }
    PT_END(&tx->pt);
}

int receptor_thread(receptor_t *rx, channel_t *canal_rx, channel_t *canal_tx) {
    PT_BEGIN(&rx->pt);
    while (1) {
        uint8_t b;
        if (ch_get(canal_rx, &b) == 0) {
            switch (rx->estado) {
                case 0:
                    if (b == STX) { rx->checksum_calculado = STX; rx->estado = 1; }
                    break;
                case 1:
                    rx->qtd_esperada = b; rx->checksum_calculado ^= b;
                    rx->indice = 0; rx->estado = (rx->qtd_esperada > 0) ? 2 : 3;
                    break;
                case 2:
                    rx->buffer[rx->indice++] = b; rx->checksum_calculado ^= b;
                    if (rx->indice == rx->qtd_esperada) rx->estado = 3;
                    break;
                case 3:
                    rx->checksum_recebido = b; rx->estado = 4;
                    break;
                case 4:
                    if (b == ETX) {
                        if (rx->checksum_recebido == rx->checksum_calculado) {
                            rx->pacote_completo = 1; rx->resposta = ACK; ch_put(canal_tx, ACK);
                        } else {
                            rx->resposta = NAK; ch_put(canal_tx, NAK);
                        }
                    }
                    rx->estado = 0;
                    break;
            }
        }
        PT_YIELD(&rx->pt);
    }
    PT_END(&rx->pt);
}
