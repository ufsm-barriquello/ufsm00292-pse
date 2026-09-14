#include "pt_protocolo.h"
#include <stddef.h>
#include <string.h>

void canal_init(CanalComunicacao *c) {
    if (c == NULL) return;
    c->head = 0;
    c->tail = 0;
    c->count = 0;
}

bool canal_write(CanalComunicacao *c, uint8_t byte) {
    if (c == NULL || c->count >= (int)(sizeof(c->buffer))) return false;
    c->buffer[c->head] = byte;
    c->head = (c->head + 1) % (sizeof(c->buffer));
    c->count++;
    return true;
}

bool canal_read(CanalComunicacao *c, uint8_t *byte) {
    if (c == NULL || byte == NULL || c->count == 0) return false;
    *byte = c->buffer[c->tail];
    c->tail = (c->tail + 1) % (sizeof(c->buffer));
    c->count--;
    return true;
}

bool canal_is_empty(const CanalComunicacao *c) {
    if (c == NULL) return true;
    return (c->count == 0);
}

int codificar_pacote(const uint8_t *dados, uint8_t qtd, uint8_t *frame_out) {
    if (frame_out == NULL || (qtd > 0 && dados == NULL)) return 0;

    frame_out[0] = STX;
    frame_out[1] = qtd;

    uint8_t chk = 0;
    for (int i = 0; i < qtd; i++) {
        frame_out[2 + i] = dados[i];
        chk ^= dados[i];
    }

    frame_out[2 + qtd] = chk;
    frame_out[3 + qtd] = ETX;

    return 4 + qtd; // Tamanho total do quadro
}

void tx_init(TransmissorPT *tx, const uint8_t *dados, uint8_t qtd, int timeout_limit) {
    if (tx == NULL) return;
    PT_INIT(&tx->pt);
    tx->qtd = qtd;
    if (qtd > 0 && dados != NULL) {
        memcpy(tx->payload, dados, qtd);
    }
    tx->frame_len = codificar_pacote(tx->payload, tx->qtd, tx->frame);
    tx->tx_index = 0;
    tx->retentativas = 0;
    tx->timer = 0;
    tx->timeout_limit = timeout_limit;
    tx->ack_recebido = false;
    tx->erro_timeout = false;
    tx->concluido = false;
}

void rx_init(ReceptorPT *rx) {
    if (rx == NULL) return;
    PT_INIT(&rx->pt);
    rx->qtd_esperada = 0;
    rx->chk_calculado = 0;
    rx->chk_recebido = 0;
    rx->ind_rx = 0;
    rx->byte_temp = 0;
    rx->pacote_valido = false;
    rx->pacotes_recebidos_count = 0;
}

int pt_transmissora_run(TransmissorPT *tx, CanalComunicacao *tx_to_rx, CanalComunicacao *rx_to_tx) {
    PT_BEGIN(&tx->pt);

    while (!tx->concluido && !tx->erro_timeout) {
        // Step 1: Transmite o quadro byte a byte para o canal
        tx->tx_index = 0;
        while (tx->tx_index < tx->frame_len) {
            PT_WAIT_UNTIL(&tx->pt, canal_write(tx_to_rx, tx->frame[tx->tx_index]));
            tx->tx_index++;
        }

        // Step 2: Aguarda confirmacao ACK do receptor ou estouro de tempo (Timeout)
        tx->timer = 0;
        tx->ack_recebido = false;

        while (tx->timer < tx->timeout_limit && !tx->ack_recebido) {
            if (!canal_is_empty(rx_to_tx)) {
                uint8_t resp;
                if (canal_read(rx_to_tx, &resp)) {
                    if (resp == ACK) {
                        tx->ack_recebido = true;
                        tx->concluido = true;
                        break;
                    }
                }
            }
            tx->timer++;
            PT_YIELD(&tx->pt);
        }

        // Step 3: Se nao recebeu ACK dentro do tempo limite, incrementa retentativas
        if (!tx->ack_recebido) {
            tx->retentativas++;
            if (tx->retentativas >= MAX_RETENTATIVAS) {
                tx->erro_timeout = true;
                PT_EXIT(&tx->pt);
            }
        }
    }

    PT_END(&tx->pt);
}

int pt_receptora_run(ReceptorPT *rx, CanalComunicacao *tx_to_rx, CanalComunicacao *rx_to_tx) {
    PT_BEGIN(&rx->pt);

    while (1) {
        // 1. Aguarda caractere delimitador STX
        do {
            PT_WAIT_UNTIL(&rx->pt, canal_read(tx_to_rx, &rx->byte_temp));
        } while (rx->byte_temp != STX);

        rx->ind_rx = 0;
        rx->chk_calculado = 0;
        rx->pacote_valido = false;

        // 2. Aguarda byte QTD
        PT_WAIT_UNTIL(&rx->pt, canal_read(tx_to_rx, &rx->byte_temp));
        rx->qtd_esperada = rx->byte_temp;

        // 3. Recebe N bytes de DADOS
        while (rx->ind_rx < rx->qtd_esperada) {
            PT_WAIT_UNTIL(&rx->pt, canal_read(tx_to_rx, &rx->byte_temp));
            if (rx->ind_rx < MAX_PAYLOAD) {
                rx->buffer_rx[rx->ind_rx++] = rx->byte_temp;
            }
            rx->chk_calculado ^= rx->byte_temp;
        }

        // 4. Aguarda byte CHK
        PT_WAIT_UNTIL(&rx->pt, canal_read(tx_to_rx, &rx->byte_temp));
        rx->chk_recebido = rx->byte_temp;

        // 5. Aguarda byte ETX
        PT_WAIT_UNTIL(&rx->pt, canal_read(tx_to_rx, &rx->byte_temp));

        // Valida se CHK e ETX conferem
        if (rx->chk_recebido == rx->chk_calculado && rx->byte_temp == ETX) {
            rx->pacote_valido = true;
            rx->pacotes_recebidos_count++;
            // Envia confirmacao ACK
            canal_write(rx_to_tx, ACK);
        } else {
            // Envia NACK em caso de erro
            canal_write(rx_to_tx, NACK);
        }
    }

    PT_END(&rx->pt);
}
