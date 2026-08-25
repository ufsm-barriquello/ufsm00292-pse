#include "protocol.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MAX_FRAME_SIZE (PROTOCOL_MAX_PAYLOAD + PROTOCOL_FRAME_OVERHEAD)

static size_t build_frame(const uint8_t *payload, size_t length, uint8_t *frame)
{
    protocol_tx_t tx;
    size_t frame_length = 0U;

    protocol_tx_init(&tx);
    assert(protocol_tx_start(&tx, payload, length));

    while (protocol_tx_next(&tx, &frame[frame_length])) {
        frame_length++;
        assert(frame_length <= MAX_FRAME_SIZE);
    }

    assert(!protocol_tx_busy(&tx));
    return frame_length;
}

static protocol_rx_event_t feed_bytes(protocol_rx_t *rx,
                                      const uint8_t *bytes,
                                      size_t length)
{
    protocol_rx_event_t event = PROTOCOL_RX_NONE;
    size_t i;

    for (i = 0U; i < length; ++i) {
        protocol_rx_event_t current = protocol_rx_process(rx, bytes[i]);
        if (current != PROTOCOL_RX_NONE) {
            event = current;
        }
    }

    return event;
}

static void test_transmitter_builds_expected_frame(void)
{
    static const uint8_t payload[] = {0xA1U, 0xB2U, 0xC3U};
    static const uint8_t expected[] = {
        PROTOCOL_STX, 0x03U, 0xA1U, 0xB2U, 0xC3U, 0xD0U, PROTOCOL_ETX
    };
    uint8_t frame[MAX_FRAME_SIZE];
    size_t frame_length = build_frame(payload, sizeof(payload), frame);

    assert(frame_length == sizeof(expected));
    assert(memcmp(frame, expected, sizeof(expected)) == 0);
}

static void test_valid_frame_is_received(void)
{
    static const uint8_t payload[] = {0x10U, PROTOCOL_STX, PROTOCOL_ETX, 0x40U};
    uint8_t frame[MAX_FRAME_SIZE];
    protocol_rx_t rx;
    size_t frame_length = build_frame(payload, sizeof(payload), frame);

    protocol_rx_init(&rx);
    assert(feed_bytes(&rx, frame, frame_length) == PROTOCOL_RX_FRAME_READY);
    assert(protocol_rx_length(&rx) == sizeof(payload));
    assert(memcmp(protocol_rx_payload(&rx), payload, sizeof(payload)) == 0);
}

static void test_empty_frame(void)
{
    static const uint8_t expected[] = {
        PROTOCOL_STX, 0x00U, 0x00U, PROTOCOL_ETX
    };
    uint8_t frame[MAX_FRAME_SIZE];
    protocol_rx_t rx;
    size_t frame_length = build_frame(NULL, 0U, frame);

    assert(frame_length == sizeof(expected));
    assert(memcmp(frame, expected, sizeof(expected)) == 0);

    protocol_rx_init(&rx);
    assert(feed_bytes(&rx, frame, frame_length) == PROTOCOL_RX_FRAME_READY);
    assert(protocol_rx_length(&rx) == 0U);
}

static void test_checksum_error_and_recovery(void)
{
    static const uint8_t payload[] = {0x01U, 0x02U, 0x04U};
    uint8_t frame[MAX_FRAME_SIZE];
    protocol_rx_t rx;
    size_t frame_length = build_frame(payload, sizeof(payload), frame);

    protocol_rx_init(&rx);
    frame[2U + sizeof(payload)] ^= 0x01U;
    assert(feed_bytes(&rx, frame, frame_length) == PROTOCOL_RX_CHECKSUM_ERROR);
    assert(rx.state == PROTOCOL_RX_WAIT_STX);

    frame_length = build_frame(payload, sizeof(payload), frame);
    assert(feed_bytes(&rx, frame, frame_length) == PROTOCOL_RX_FRAME_READY);
    assert(memcmp(protocol_rx_payload(&rx), payload, sizeof(payload)) == 0);
}

static void test_invalid_etx_resynchronizes_on_stx(void)
{
    static const uint8_t first_payload[] = {0x55U};
    static const uint8_t next_payload[] = {0x77U, 0x88U};
    uint8_t first_frame[MAX_FRAME_SIZE];
    uint8_t next_frame[MAX_FRAME_SIZE];
    protocol_rx_t rx;
    size_t first_length = build_frame(first_payload, sizeof(first_payload), first_frame);
    size_t next_length = build_frame(next_payload, sizeof(next_payload), next_frame);

    protocol_rx_init(&rx);
    assert(feed_bytes(&rx, first_frame, first_length - 1U) == PROTOCOL_RX_NONE);
    assert(protocol_rx_process(&rx, PROTOCOL_STX) == PROTOCOL_RX_FORMAT_ERROR);
    assert(rx.state == PROTOCOL_RX_LENGTH);

    assert(feed_bytes(&rx, &next_frame[1], next_length - 1U) == PROTOCOL_RX_FRAME_READY);
    assert(protocol_rx_length(&rx) == sizeof(next_payload));
    assert(memcmp(protocol_rx_payload(&rx), next_payload, sizeof(next_payload)) == 0);
}

static void test_noise_and_invalid_transmitter_arguments(void)
{
    static const uint8_t noise[] = {0x00U, 0x10U, 0xFFU};
    static const uint8_t byte = 0x42U;
    protocol_rx_t rx;
    protocol_tx_t tx;

    protocol_rx_init(&rx);
    assert(feed_bytes(&rx, noise, sizeof(noise)) == PROTOCOL_RX_NONE);
    assert(rx.state == PROTOCOL_RX_WAIT_STX);

    protocol_tx_init(&tx);
    assert(!protocol_tx_start(&tx, NULL, 1U));
    assert(!protocol_tx_start(&tx, &byte, PROTOCOL_MAX_PAYLOAD + 1U));
    assert(protocol_tx_start(&tx, &byte, 1U));
    assert(!protocol_tx_start(&tx, &byte, 1U));
}

static void test_maximum_payload(void)
{
    uint8_t payload[PROTOCOL_MAX_PAYLOAD];
    uint8_t frame[MAX_FRAME_SIZE];
    protocol_rx_t rx;
    size_t i;
    size_t frame_length;

    for (i = 0U; i < sizeof(payload); ++i) {
        payload[i] = (uint8_t)i;
    }

    frame_length = build_frame(payload, sizeof(payload), frame);
    assert(frame_length == MAX_FRAME_SIZE);

    protocol_rx_init(&rx);
    assert(feed_bytes(&rx, frame, frame_length) == PROTOCOL_RX_FRAME_READY);
    assert(protocol_rx_length(&rx) == sizeof(payload));
    assert(memcmp(protocol_rx_payload(&rx), payload, sizeof(payload)) == 0);
}

int main(void)
{
    test_transmitter_builds_expected_frame();
    test_valid_frame_is_received();
    test_empty_frame();
    test_checksum_error_and_recovery();
    test_invalid_etx_resynchronizes_on_stx();
    test_noise_and_invalid_transmitter_arguments();
    test_maximum_payload();

    puts("Todos os testes passaram.");
    return 0;
}
