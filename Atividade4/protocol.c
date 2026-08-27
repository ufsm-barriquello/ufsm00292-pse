#include "protocol.h"

#include <string.h>

static bool data_channel_write(protocol_data_channel_t *channel, uint8_t byte)
{
    if (channel->count >= PROTOCOL_CHANNEL_CAPACITY) {
        return false;
    }

    channel->bytes[channel->tail] = byte;
    channel->tail = (channel->tail + 1U) % PROTOCOL_CHANNEL_CAPACITY;
    channel->count++;
    return true;
}

static bool data_channel_read(protocol_data_channel_t *channel, uint8_t *byte)
{
    if (channel->count == 0U) {
        return false;
    }

    *byte = channel->bytes[channel->head];
    channel->head = (channel->head + 1U) % PROTOCOL_CHANNEL_CAPACITY;
    channel->count--;
    return true;
}

static bool ack_channel_write(protocol_ack_channel_t *channel, uint8_t byte)
{
    if (channel->count >= PROTOCOL_ACK_CHANNEL_CAPACITY) {
        return false;
    }

    channel->bytes[channel->tail] = byte;
    channel->tail = (channel->tail + 1U) % PROTOCOL_ACK_CHANNEL_CAPACITY;
    channel->count++;
    return true;
}

static bool ack_channel_read(protocol_ack_channel_t *channel, uint8_t *byte)
{
    if (channel->count == 0U) {
        return false;
    }

    *byte = channel->bytes[channel->head];
    channel->head = (channel->head + 1U) % PROTOCOL_ACK_CHANNEL_CAPACITY;
    channel->count--;
    return true;
}

static bool timer_expired(uint32_t now, uint32_t deadline)
{
    return (int32_t)(now - deadline) >= 0;
}

static uint8_t payload_checksum(const uint8_t *payload, size_t length)
{
    uint8_t checksum = 0U;
    size_t i;

    for (i = 0U; i < length; ++i) {
        checksum ^= payload[i];
    }

    return checksum;
}

static void build_frame(protocol_transmitter_t *transmitter)
{
    transmitter->frame[0] = PROTOCOL_STX;
    transmitter->frame[1] = (uint8_t)transmitter->length;

    if (transmitter->length > 0U) {
        memcpy(&transmitter->frame[2], transmitter->payload, transmitter->length);
    }

    transmitter->frame[2U + transmitter->length] =
        payload_checksum(transmitter->payload, transmitter->length);
    transmitter->frame[3U + transmitter->length] = PROTOCOL_ETX;
    transmitter->frame_length = transmitter->length + PROTOCOL_FRAME_OVERHEAD;
}

static bool take_ack(protocol_system_t *system)
{
    uint8_t byte;

    while (ack_channel_read(&system->ack_channel, &byte)) {
        if (byte == PROTOCOL_ACK) {
            return true;
        }
    }

    return false;
}

void protocol_system_init(protocol_system_t *system, uint32_t timeout_ticks)
{
    if (system == NULL) {
        return;
    }

    memset(system, 0, sizeof(*system));
    system->timeout_ticks = (timeout_ticks == 0U) ? 1U : timeout_ticks;
    PT_INIT(&system->transmitter.pt);
    PT_INIT(&system->receiver.pt);
}

bool protocol_send(protocol_system_t *system, const uint8_t *payload, size_t length)
{
    protocol_transmitter_t *transmitter;

    if ((system == NULL) || (length > PROTOCOL_MAX_PAYLOAD) ||
        ((length > 0U) && (payload == NULL))) {
        return false;
    }

    transmitter = &system->transmitter;
    if (transmitter->pending) {
        return false;
    }

    if (length > 0U) {
        memcpy(transmitter->payload, payload, length);
    }

    transmitter->length = length;
    transmitter->frame_length = 0U;
    transmitter->frame_index = 0U;
    transmitter->attempts = 0U;
    transmitter->retransmissions = 0U;
    transmitter->acknowledged = false;
    transmitter->completed = false;
    transmitter->pending = true;

    memset(&system->ack_channel, 0, sizeof(system->ack_channel));
    return true;
}

bool protocol_receive(protocol_system_t *system,
                      uint8_t *payload,
                      size_t capacity,
                      size_t *length)
{
    protocol_receiver_t *receiver;

    if ((system == NULL) || (length == NULL)) {
        return false;
    }

    receiver = &system->receiver;
    if (!receiver->frame_ready || (capacity < receiver->length) ||
        ((receiver->length > 0U) && (payload == NULL))) {
        return false;
    }

    if (receiver->length > 0U) {
        memcpy(payload, receiver->payload, receiver->length);
    }
    *length = receiver->length;
    receiver->frame_ready = false;
    return true;
}

void protocol_system_step(protocol_system_t *system)
{
    if (system == NULL) {
        return;
    }

    (void)protocol_transmitter_thread(system);
    (void)protocol_receiver_thread(system);
    system->ticks++;
}

void protocol_drop_next_ack(protocol_system_t *system)
{
    if (system != NULL) {
        system->drop_next_ack = true;
    }
}

void protocol_corrupt_next_checksum(protocol_system_t *system)
{
    if (system != NULL) {
        system->corrupt_next_checksum = true;
    }
}

PT_THREAD(protocol_transmitter_thread(protocol_system_t *system))
{
    protocol_transmitter_t *transmitter = &system->transmitter;

    PT_BEGIN(&transmitter->pt);

    while (true) {
        PT_WAIT_UNTIL(&transmitter->pt, transmitter->pending);
        build_frame(transmitter);

        do {
            transmitter->frame_index = 0U;
            transmitter->attempts++;

            while (transmitter->frame_index < transmitter->frame_length) {
                transmitter->byte_to_send =
                    transmitter->frame[transmitter->frame_index];

                if (system->corrupt_next_checksum &&
                    (transmitter->frame_index == transmitter->frame_length - 2U)) {
                    transmitter->byte_to_send ^= UINT8_C(0x01);
                    system->corrupt_next_checksum = false;
                }

                PT_WAIT_UNTIL(
                    &transmitter->pt,
                    data_channel_write(&system->data_channel,
                                       transmitter->byte_to_send));
                transmitter->frame_index++;
            }

            transmitter->deadline = system->ticks + system->timeout_ticks;
            transmitter->acknowledged = false;
            PT_WAIT_UNTIL(
                &transmitter->pt,
                (transmitter->acknowledged = take_ack(system)) ||
                    timer_expired(system->ticks, transmitter->deadline));

            if (!transmitter->acknowledged) {
                transmitter->retransmissions++;
            }
        } while (!transmitter->acknowledged);

        transmitter->pending = false;
        transmitter->completed = true;
    }

    PT_END(&transmitter->pt);
}

PT_THREAD(protocol_receiver_thread(protocol_system_t *system))
{
    protocol_receiver_t *receiver = &system->receiver;

    PT_BEGIN(&receiver->pt);

    while (true) {
        do {
            PT_WAIT_UNTIL(
                &receiver->pt,
                data_channel_read(&system->data_channel, &receiver->current_byte));
        } while (receiver->current_byte != PROTOCOL_STX);

        PT_WAIT_UNTIL(
            &receiver->pt,
            data_channel_read(&system->data_channel, &receiver->current_byte));
        receiver->length = receiver->current_byte;
        receiver->index = 0U;
        receiver->checksum = 0U;

        while (receiver->index < receiver->length) {
            PT_WAIT_UNTIL(
                &receiver->pt,
                data_channel_read(&system->data_channel, &receiver->current_byte));
            receiver->payload[receiver->index] = receiver->current_byte;
            receiver->checksum ^= receiver->current_byte;
            receiver->index++;
        }

        PT_WAIT_UNTIL(
            &receiver->pt,
            data_channel_read(&system->data_channel, &receiver->current_byte));
        if (receiver->current_byte != receiver->checksum) {
            receiver->invalid_frames++;
            continue;
        }

        PT_WAIT_UNTIL(
            &receiver->pt,
            data_channel_read(&system->data_channel, &receiver->current_byte));
        if (receiver->current_byte != PROTOCOL_ETX) {
            receiver->invalid_frames++;
            continue;
        }

        receiver->valid_frames++;
        receiver->frame_ready = true;

        if (system->drop_next_ack) {
            system->drop_next_ack = false;
            system->dropped_acks++;
        } else {
            PT_WAIT_UNTIL(
                &receiver->pt,
                ack_channel_write(&system->ack_channel, PROTOCOL_ACK));
        }
    }

    PT_END(&receiver->pt);
}
