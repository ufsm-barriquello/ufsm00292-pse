#include "protocol.h"

#include <string.h>

static void protocol_rx_begin_frame(protocol_rx_t *rx)
{
    rx->state = PROTOCOL_RX_LENGTH;
    rx->length = 0U;
    rx->index = 0U;
    rx->checksum = 0U;
}

void protocol_tx_init(protocol_tx_t *tx)
{
    if (tx == NULL) {
        return;
    }

    memset(tx, 0, sizeof(*tx));
    tx->state = PROTOCOL_TX_IDLE;
}

bool protocol_tx_start(protocol_tx_t *tx, const uint8_t *payload, size_t length)
{
    if ((tx == NULL) || (tx->state != PROTOCOL_TX_IDLE) ||
        (length > PROTOCOL_MAX_PAYLOAD) || ((length > 0U) && (payload == NULL))) {
        return false;
    }

    if (length > 0U) {
        memcpy(tx->payload, payload, length);
    }

    tx->length = (uint8_t)length;
    tx->index = 0U;
    tx->checksum = 0U;
    tx->state = PROTOCOL_TX_STX;
    return true;
}

bool protocol_tx_next(protocol_tx_t *tx, uint8_t *byte_to_send)
{
    if ((tx == NULL) || (byte_to_send == NULL)) {
        return false;
    }

    switch (tx->state) {
    case PROTOCOL_TX_IDLE:
        return false;

    case PROTOCOL_TX_STX:
        *byte_to_send = PROTOCOL_STX;
        tx->state = PROTOCOL_TX_LENGTH;
        break;

    case PROTOCOL_TX_LENGTH:
        *byte_to_send = tx->length;
        tx->index = 0U;
        tx->checksum = 0U;
        tx->state = (tx->length == 0U) ? PROTOCOL_TX_CHECKSUM : PROTOCOL_TX_DATA;
        break;

    case PROTOCOL_TX_DATA:
        *byte_to_send = tx->payload[tx->index];
        tx->checksum ^= *byte_to_send;
        tx->index++;
        if (tx->index >= tx->length) {
            tx->state = PROTOCOL_TX_CHECKSUM;
        }
        break;

    case PROTOCOL_TX_CHECKSUM:
        *byte_to_send = tx->checksum;
        tx->state = PROTOCOL_TX_ETX;
        break;

    case PROTOCOL_TX_ETX:
        *byte_to_send = PROTOCOL_ETX;
        tx->state = PROTOCOL_TX_IDLE;
        break;

    default:
        protocol_tx_init(tx);
        return false;
    }

    return true;
}

bool protocol_tx_busy(const protocol_tx_t *tx)
{
    return (tx != NULL) && (tx->state != PROTOCOL_TX_IDLE);
}

void protocol_rx_init(protocol_rx_t *rx)
{
    if (rx == NULL) {
        return;
    }

    memset(rx, 0, sizeof(*rx));
    rx->state = PROTOCOL_RX_WAIT_STX;
}

protocol_rx_event_t protocol_rx_process(protocol_rx_t *rx, uint8_t byte_received)
{
    if (rx == NULL) {
        return PROTOCOL_RX_FORMAT_ERROR;
    }

    switch (rx->state) {
    case PROTOCOL_RX_WAIT_STX:
        if (byte_received == PROTOCOL_STX) {
            protocol_rx_begin_frame(rx);
        }
        break;

    case PROTOCOL_RX_LENGTH:
        rx->length = byte_received;
        rx->index = 0U;
        rx->checksum = 0U;
        rx->state = (rx->length == 0U) ? PROTOCOL_RX_CHECKSUM : PROTOCOL_RX_DATA;
        break;

    case PROTOCOL_RX_DATA:
        rx->payload[rx->index] = byte_received;
        rx->checksum ^= byte_received;
        rx->index++;
        if (rx->index >= rx->length) {
            rx->state = PROTOCOL_RX_CHECKSUM;
        }
        break;

    case PROTOCOL_RX_CHECKSUM:
        if (byte_received != rx->checksum) {
            rx->state = PROTOCOL_RX_WAIT_STX;
            return PROTOCOL_RX_CHECKSUM_ERROR;
        }
        rx->state = PROTOCOL_RX_ETX;
        break;

    case PROTOCOL_RX_ETX:
        if (byte_received == PROTOCOL_ETX) {
            rx->state = PROTOCOL_RX_WAIT_STX;
            return PROTOCOL_RX_FRAME_READY;
        }

        if (byte_received == PROTOCOL_STX) {
            protocol_rx_begin_frame(rx);
        } else {
            rx->state = PROTOCOL_RX_WAIT_STX;
        }
        return PROTOCOL_RX_FORMAT_ERROR;

    default:
        protocol_rx_init(rx);
        return PROTOCOL_RX_FORMAT_ERROR;
    }

    return PROTOCOL_RX_NONE;
}

const uint8_t *protocol_rx_payload(const protocol_rx_t *rx)
{
    return (rx == NULL) ? NULL : rx->payload;
}

size_t protocol_rx_length(const protocol_rx_t *rx)
{
    return (rx == NULL) ? 0U : rx->length;
}
