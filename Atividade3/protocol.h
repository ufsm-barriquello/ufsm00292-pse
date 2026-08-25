#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PROTOCOL_STX UINT8_C(0x02)
#define PROTOCOL_ETX UINT8_C(0x03)
#define PROTOCOL_MAX_PAYLOAD UINT8_MAX
#define PROTOCOL_FRAME_OVERHEAD 4U

typedef enum {
    PROTOCOL_TX_IDLE,
    PROTOCOL_TX_STX,
    PROTOCOL_TX_LENGTH,
    PROTOCOL_TX_DATA,
    PROTOCOL_TX_CHECKSUM,
    PROTOCOL_TX_ETX
} protocol_tx_state_t;

typedef struct {
    protocol_tx_state_t state;
    uint8_t payload[PROTOCOL_MAX_PAYLOAD];
    uint8_t length;
    uint8_t index;
    uint8_t checksum;
} protocol_tx_t;

typedef enum {
    PROTOCOL_RX_WAIT_STX,
    PROTOCOL_RX_LENGTH,
    PROTOCOL_RX_DATA,
    PROTOCOL_RX_CHECKSUM,
    PROTOCOL_RX_ETX
} protocol_rx_state_t;

typedef enum {
    PROTOCOL_RX_NONE,
    PROTOCOL_RX_FRAME_READY,
    PROTOCOL_RX_CHECKSUM_ERROR,
    PROTOCOL_RX_FORMAT_ERROR
} protocol_rx_event_t;

typedef struct {
    protocol_rx_state_t state;
    uint8_t payload[PROTOCOL_MAX_PAYLOAD];
    uint8_t length;
    uint8_t index;
    uint8_t checksum;
} protocol_rx_t;

void protocol_tx_init(protocol_tx_t *tx);
bool protocol_tx_start(protocol_tx_t *tx, const uint8_t *payload, size_t length);
bool protocol_tx_next(protocol_tx_t *tx, uint8_t *byte_to_send);
bool protocol_tx_busy(const protocol_tx_t *tx);

void protocol_rx_init(protocol_rx_t *rx);
protocol_rx_event_t protocol_rx_process(protocol_rx_t *rx, uint8_t byte_received);
const uint8_t *protocol_rx_payload(const protocol_rx_t *rx);
size_t protocol_rx_length(const protocol_rx_t *rx);

#endif
