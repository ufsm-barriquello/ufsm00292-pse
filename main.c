#include <stdio.h>
#include <stdint.h>

#define STX 0x02
#define ETX 0x03

/* ------------------------- RECEPTOR ------------------------- */
typedef enum {
    RX_IDLE,
    RX_WAIT_QTD,
    RX_READ_DATA,
    RX_WAIT_CHK,
    RX_WAIT_ETX
} RxState;

static RxState rx_state = RX_IDLE;

static uint8_t rx_buf[256];
static uint8_t rx_expected = 0;  
static uint8_t rx_len = 0;        
static uint8_t rx_chk = 0;        

static void rx_reset(void) {
    rx_state = RX_IDLE;
    rx_expected = 0;
    rx_len = 0;
    rx_chk = 0;
}

static void rx_byte(uint8_t b) {
    switch (rx_state) {
    case RX_IDLE:
        if (b == STX) {
            rx_state = RX_WAIT_QTD;
        }
        break;

    case RX_WAIT_QTD:
        rx_expected = b;
        rx_len = 0;
        rx_chk = 0;
        rx_state = (rx_expected > 0) ? RX_READ_DATA : RX_WAIT_CHK;
        break;

    case RX_READ_DATA:
        if (rx_len < sizeof(rx_buf)) {
            rx_buf[rx_len++] = b;
            rx_chk ^= b;
        }
        if (rx_len >= rx_expected) {
            rx_state = RX_WAIT_CHK;
        }
        break;

    case RX_WAIT_CHK:
        if (b == rx_chk) {
            rx_state = RX_WAIT_ETX;
        } else {
            printf("ERRO: checksum invalido\n");
            rx_reset();
        }
        break;

    case RX_WAIT_ETX:
        if (b == ETX) {
            printf("OK: pacote recebido. Dados: ");
            for (uint8_t i = 0; i < rx_expected; i++) {
                printf("%02X ", rx_buf[i]);
            }
            printf("\n");
        } else {
            printf("ERRO: ETX invalido\n");
        }
        rx_reset();
        break;
    }
}

/* ------------------------- TRANSMISSOR ------------------------- */
static uint8_t tx_calc_chk(const uint8_t *data, uint8_t n) {
    uint8_t c = 0;
    for (uint8_t i = 0; i < n; i++) {
        c ^= data[i];
    }
    return c;
}

static void tx_send(const uint8_t *data, uint8_t n) {
    uint8_t chk = tx_calc_chk(data, n);

    rx_byte(STX);
    rx_byte(n);
    for (uint8_t i = 0; i < n; i++) {
        rx_byte(data[i]);
    }
    rx_byte(chk);
    rx_byte(ETX);
}

int main(void) {
    const uint8_t p1[] = {0x10, 0x20, 0x30};
    const uint8_t p2[] = {0xAB, 0xCD};

    tx_send(p1, (uint8_t)sizeof p1);
    tx_send(p2, (uint8_t)sizeof p2);

    const uint8_t bad[] = {0x01, 0x02, 0x03};
    uint8_t chk_bad = tx_calc_chk(bad, (uint8_t)sizeof bad) ^ 0xFF; 
    rx_byte(STX);
    rx_byte((uint8_t)sizeof bad);
    for (uint8_t i = 0; i < (uint8_t)sizeof bad; i++) rx_byte(bad[i]);
    rx_byte(chk_bad); 
    rx_byte(ETX);

    return 0;
}

