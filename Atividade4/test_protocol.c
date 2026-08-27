#include "protocol.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define TEST_MAX_STEPS 2000U

static void run_until_complete(protocol_system_t *system)
{
    unsigned int step;

    for (step = 0U; step < TEST_MAX_STEPS && !system->transmitter.completed; ++step) {
        protocol_system_step(system);
    }

    assert(system->transmitter.completed);
    assert(system->transmitter.acknowledged);
}

static void assert_received_payload(protocol_system_t *system,
                                    const uint8_t *expected,
                                    size_t expected_length)
{
    uint8_t received[PROTOCOL_MAX_PAYLOAD];
    size_t received_length = 0U;

    assert(protocol_receive(system, received, sizeof(received), &received_length));
    assert(received_length == expected_length);
    assert(memcmp(received, expected, expected_length) == 0);
}

static void test_frame_format_and_first_attempt_ack(void)
{
    static const uint8_t payload[] = {0xA1U, 0xB2U, 0xC3U};
    static const uint8_t expected_frame[] = {
        PROTOCOL_STX, 0x03U, 0xA1U, 0xB2U, 0xC3U, 0xD0U, PROTOCOL_ETX
    };
    protocol_system_t system;

    protocol_system_init(&system, 5U);
    assert(protocol_send(&system, payload, sizeof(payload)));
    protocol_system_step(&system);

    assert(system.transmitter.frame_length == sizeof(expected_frame));
    assert(memcmp(system.transmitter.frame, expected_frame, sizeof(expected_frame)) == 0);

    run_until_complete(&system);
    assert(system.transmitter.attempts == 1U);
    assert(system.transmitter.retransmissions == 0U);
    assert(system.receiver.valid_frames == 1U);
    assert(system.receiver.invalid_frames == 0U);
    assert_received_payload(&system, payload, sizeof(payload));
}

static void test_lost_ack_causes_retransmission(void)
{
    static const uint8_t payload[] = {0x10U, 0x20U};
    protocol_system_t system;

    protocol_system_init(&system, 4U);
    protocol_drop_next_ack(&system);
    assert(protocol_send(&system, payload, sizeof(payload)));
    run_until_complete(&system);

    assert(system.transmitter.attempts == 2U);
    assert(system.transmitter.retransmissions == 1U);
    assert(system.receiver.valid_frames == 2U);
    assert(system.dropped_acks == 1U);
    assert_received_payload(&system, payload, sizeof(payload));
}

static void test_corrupted_checksum_causes_retransmission(void)
{
    static const uint8_t payload[] = {0x01U, 0x02U, 0x04U};
    protocol_system_t system;

    protocol_system_init(&system, 4U);
    protocol_corrupt_next_checksum(&system);
    assert(protocol_send(&system, payload, sizeof(payload)));
    run_until_complete(&system);

    assert(system.transmitter.attempts == 2U);
    assert(system.transmitter.retransmissions == 1U);
    assert(system.receiver.invalid_frames == 1U);
    assert(system.receiver.valid_frames == 1U);
    assert_received_payload(&system, payload, sizeof(payload));
}

static void test_empty_payload(void)
{
    static const uint8_t expected_frame[] = {
        PROTOCOL_STX, 0x00U, 0x00U, PROTOCOL_ETX
    };
    protocol_system_t system;
    size_t received_length = 1U;

    protocol_system_init(&system, 3U);
    assert(protocol_send(&system, NULL, 0U));
    run_until_complete(&system);

    assert(system.transmitter.frame_length == sizeof(expected_frame));
    assert(memcmp(system.transmitter.frame, expected_frame, sizeof(expected_frame)) == 0);
    assert(protocol_receive(&system, NULL, 0U, &received_length));
    assert(received_length == 0U);
}

static void test_maximum_payload(void)
{
    uint8_t payload[PROTOCOL_MAX_PAYLOAD];
    protocol_system_t system;
    size_t i;

    for (i = 0U; i < sizeof(payload); ++i) {
        payload[i] = (uint8_t)i;
    }

    protocol_system_init(&system, 5U);
    assert(protocol_send(&system, payload, sizeof(payload)));
    run_until_complete(&system);

    assert(system.transmitter.frame_length == PROTOCOL_MAX_FRAME_SIZE);
    assert(system.transmitter.attempts == 1U);
    assert_received_payload(&system, payload, sizeof(payload));
}

static void test_rejects_invalid_or_concurrent_send(void)
{
    static const uint8_t payload = 0x42U;
    protocol_system_t system;

    protocol_system_init(&system, 5U);
    assert(!protocol_send(&system, NULL, 1U));
    assert(!protocol_send(&system, &payload, PROTOCOL_MAX_PAYLOAD + 1U));
    assert(protocol_send(&system, &payload, 1U));
    assert(!protocol_send(&system, &payload, 1U));
}

int main(void)
{
    test_frame_format_and_first_attempt_ack();
    test_lost_ack_causes_retransmission();
    test_corrupted_checksum_causes_retransmission();
    test_empty_payload();
    test_maximum_payload();
    test_rejects_invalid_or_concurrent_send();

    puts("Todos os testes passaram.");
    return 0;
}
