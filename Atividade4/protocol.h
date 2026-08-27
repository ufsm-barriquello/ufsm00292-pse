#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "pt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PROTOCOL_STX UINT8_C(0x02)
#define PROTOCOL_ETX UINT8_C(0x03)
#define PROTOCOL_ACK UINT8_C(0x06)
#define PROTOCOL_MAX_PAYLOAD UINT8_MAX
#define PROTOCOL_FRAME_OVERHEAD 4U
#define PROTOCOL_MAX_FRAME_SIZE (PROTOCOL_MAX_PAYLOAD + PROTOCOL_FRAME_OVERHEAD)
#define PROTOCOL_CHANNEL_CAPACITY 16U
#define PROTOCOL_ACK_CHANNEL_CAPACITY 4U

typedef struct {
    uint8_t bytes[PROTOCOL_CHANNEL_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
} protocol_data_channel_t;

typedef struct {
    uint8_t bytes[PROTOCOL_ACK_CHANNEL_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
} protocol_ack_channel_t;

typedef struct {
    struct pt pt;
    uint8_t payload[PROTOCOL_MAX_PAYLOAD];
    uint8_t frame[PROTOCOL_MAX_FRAME_SIZE];
    size_t length;
    size_t frame_length;
    size_t frame_index;
    uint8_t byte_to_send;
    uint32_t deadline;
    unsigned int attempts;
    unsigned int retransmissions;
    bool pending;
    bool acknowledged;
    bool completed;
} protocol_transmitter_t;

typedef struct {
    struct pt pt;
    uint8_t payload[PROTOCOL_MAX_PAYLOAD];
    size_t length;
    size_t index;
    uint8_t current_byte;
    uint8_t checksum;
    unsigned int valid_frames;
    unsigned int invalid_frames;
    bool frame_ready;
} protocol_receiver_t;

typedef struct {
    protocol_transmitter_t transmitter;
    protocol_receiver_t receiver;
    protocol_data_channel_t data_channel;
    protocol_ack_channel_t ack_channel;
    uint32_t ticks;
    uint32_t timeout_ticks;
    unsigned int dropped_acks;
    bool drop_next_ack;
    bool corrupt_next_checksum;
} protocol_system_t;

void protocol_system_init(protocol_system_t *system, uint32_t timeout_ticks);
bool protocol_send(protocol_system_t *system, const uint8_t *payload, size_t length);
bool protocol_receive(protocol_system_t *system,
                      uint8_t *payload,
                      size_t capacity,
                      size_t *length);
void protocol_system_step(protocol_system_t *system);
void protocol_drop_next_ack(protocol_system_t *system);
void protocol_corrupt_next_checksum(protocol_system_t *system);

PT_THREAD(protocol_transmitter_thread(protocol_system_t *system));
PT_THREAD(protocol_receiver_thread(protocol_system_t *system));

#endif
