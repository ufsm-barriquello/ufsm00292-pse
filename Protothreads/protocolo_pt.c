#include "protocolo_pt.h"
#include <string.h>
#include <stdio.h>

// --- Funções Auxiliares ---
bool buffer_write(comm_buffer_t* comm, uint8_t byte) {
    uint16_t next_head = (comm->head + 1) % 256;
    if (next_head == comm->tail) return false;
    comm->buffer[comm->head] = byte;
    comm->head = next_head;
    return true;
}

bool buffer_read(comm_buffer_t* comm, uint8_t* byte) {
    if (comm->head == comm->tail) return false;
    *byte = comm->buffer[comm->tail];
    comm->tail = (comm->tail + 1) % 256;
    return true;
}

static uint8_t calcular_checksum(const uint8_t *dados, uint8_t qtd) {
    uint8_t chk = 0;
    for (uint8_t i = 0; i < qtd; i++) {
        chk ^= dados[i];
    }
    return chk;
}

// --- Protothread do Transmissor ---
PT_THREAD(protothread_tx(struct pt* pt, shared_channel_t* channel)) {
    static uint8_t checksum;
    static uint8_t i;
    static unsigned long timestamp;

    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, channel->iniciar_transmissao);
        channel->ack_recebido = false;
        checksum = calcular_checksum(channel->dados_para_enviar, channel->qtd_a_enviar);

        do {
            printf("TX: Enviando pacote...\n");
            buffer_write(&channel->tx_to_rx_buffer, STX_VALUE);       PT_YIELD(pt);
            buffer_write(&channel->tx_to_rx_buffer, channel->qtd_a_enviar); PT_YIELD(pt);
            for (i = 0; i < channel->qtd_a_enviar; ++i) {
                buffer_write(&channel->tx_to_rx_buffer, channel->dados_para_enviar[i]);
                PT_YIELD(pt);
            }
            buffer_write(&channel->tx_to_rx_buffer, checksum);       PT_YIELD(pt);
            buffer_write(&channel->tx_to_rx_buffer, ETX_VALUE);         PT_YIELD(pt);

            printf("TX: Pacote enviado. Aguardando ACK...\n");
            timestamp = 0;
            PT_WAIT_UNTIL(pt, channel->ack_recebido || ++timestamp > TIMEOUT_MS);
        } while(!channel->ack_recebido);

        channel->iniciar_transmissao = false;
    }
    PT_END(pt);
}

// --- Protothread do Receptor ---
PT_THREAD(protothread_rx(struct pt* pt, shared_channel_t* channel)) {
    uint8_t byte_recebido;
    uint8_t i;

    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, buffer_read(&channel->tx_to_rx_buffer, &byte_recebido));

        if (byte_recebido == STX_VALUE) {

        PROCESSAR_CORPO_PACOTE:

            channel->pacote_recebido_com_sucesso = false;
            channel->rx_chk_calculado = 0;

            PT_WAIT_UNTIL(pt, buffer_read(&channel->tx_to_rx_buffer, &byte_recebido));
            channel->qtd_recebida = byte_recebido;
            if (channel->qtd_recebida > MAX_DADOS_SIZE) { continue; }

            for (i = 0; i < channel->qtd_recebida; ++i) {
                PT_WAIT_UNTIL(pt, buffer_read(&channel->tx_to_rx_buffer, &byte_recebido));
                if (byte_recebido == STX_VALUE) { goto PROCESSAR_CORPO_PACOTE; }
                channel->rx_chk_calculado ^= byte_recebido;
                channel->dados_recebidos[i] = byte_recebido;
            }

            PT_WAIT_UNTIL(pt, buffer_read(&channel->tx_to_rx_buffer, &byte_recebido));
            if (byte_recebido == STX_VALUE) { goto PROCESSAR_CORPO_PACOTE; }
            if (byte_recebido != channel->rx_chk_calculado) { continue; }

            PT_WAIT_UNTIL(pt, buffer_read(&channel->tx_to_rx_buffer, &byte_recebido));
            if (byte_recebido != ETX_VALUE) { continue; }

            // --- SUCESSO ---
            channel->pacote_recebido_com_sucesso = true;

            // Envia ACK de volta
            uint8_t ack_data[1] = {ACK_VALUE};
            uint8_t ack_checksum = calcular_checksum(ack_data, 1);
            buffer_write(&channel->rx_to_tx_buffer, STX_VALUE);
            buffer_write(&channel->rx_to_tx_buffer, 1);
            buffer_write(&channel->rx_to_tx_buffer, ACK_VALUE);
            buffer_write(&channel->rx_to_tx_buffer, ack_checksum);
            buffer_write(&channel->rx_to_tx_buffer, ETX_VALUE);

            // Agora o receptor confirma diretamente
            channel->ack_recebido = true;

            PT_WAIT_UNTIL(pt, channel->pacote_recebido_com_sucesso == false);
        }
    }

    PT_END(pt);
}