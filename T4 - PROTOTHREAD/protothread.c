#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* macros de testes - baseado em minUnit: www.jera.com/techinfo/jtns/jtn002.html */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                if (mensagem) return mensagem; } while (0)
int testes_executados = 0;

// ===========================================
// == PROTOTHREADS - Implementação Simples ===
// ===========================================
typedef struct {
    int lc;  // local continuation
} pt_t;

#define PT_INIT(pt) do { (pt)->lc = 0; } while(0)
#define PT_BEGIN(pt) switch((pt)->lc) { case 0:
#define PT_END(pt) } (pt)->lc = 0; return 0
#define PT_WAIT_UNTIL(pt, cond) do { (pt)->lc = __LINE__; case __LINE__: if(!(cond)) return 1; } while(0)
#define PT_YIELD(pt) do { (pt)->lc = __LINE__; return 1; case __LINE__:; } while(0)
#define PT_RESTART(pt) do { (pt)->lc = 0; return 1; } while(0)

// ========================================
// ====== PROTOCOLO DE COMUNICAÇÃO ========
// ========================================
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define MAX_DATA_SIZE 255
#define MAX_PACKET_SIZE (MAX_DATA_SIZE + 4) // STX + QTD + DATA + CHK + ETX

// Estados do protocolo
typedef enum {
    STATE_IDLE,
    STATE_WAIT_STX,
    STATE_WAIT_QTD,
    STATE_WAIT_DATA,
    STATE_WAIT_CHK,
    STATE_WAIT_ETX,
    STATE_PACKET_READY,
    STATE_ERROR
} protocol_state_t;

// Estruturas para transmissor e receptor
typedef struct {
    pt_t pt;
    uint8_t *data;
    uint8_t data_size;
    uint8_t packet[MAX_PACKET_SIZE];
    uint8_t packet_size;
    bool packet_sent;
    bool ack_received;
    bool timeout;
} transmitter_t;

typedef struct {
    pt_t pt;
    protocol_state_t state;
    uint8_t buffer[MAX_PACKET_SIZE];
    uint8_t expected_size;
    uint8_t received_size;
    uint8_t checksum;
    bool packet_ready;
    bool error;
    bool send_ack;
} receiver_t;

// Canal de comunicação simulado
typedef struct {
    uint8_t tx_buffer[MAX_PACKET_SIZE];
    uint8_t tx_size;
    bool tx_ready;
    uint8_t rx_buffer[MAX_PACKET_SIZE];
    uint8_t rx_size;
    bool rx_ready;
    bool simulate_error;
} communication_channel_t;

// Variáveis globais para simulação
static communication_channel_t channel;
static transmitter_t tx;
static receiver_t rx;

// ========================================
// ========= FUNÇÕES AUXILIARES ===========
// ========================================

uint8_t calculate_checksum(uint8_t *data, uint8_t size) {
    uint8_t checksum = 0;
    for (int i = 0; i < size; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void init_transmitter(transmitter_t *tx) {
    PT_INIT(&tx->pt);
    tx->packet_sent = false;
    tx->ack_received = false;
    tx->timeout = false;
}

void init_receiver(receiver_t *rx) {
    PT_INIT(&rx->pt);
    rx->state = STATE_WAIT_STX;
    rx->received_size = 0;
    rx->packet_ready = false;
    rx->error = false;
    rx->send_ack = false;
}

void init_channel(communication_channel_t *ch) {
    ch->tx_ready = false;
    ch->rx_ready = false;
    ch->simulate_error = false;
    ch->tx_size = 0;
    ch->rx_size = 0;
}

// ========================================
// ===== PROTOTHREAD DO TRANSMISSOR =======
// ========================================

int transmitter_thread(transmitter_t *tx) {
    PT_BEGIN(&tx->pt);
    
    // Monta o pacote
    tx->packet[0] = STX;
    tx->packet[1] = tx->data_size;
    memcpy(&tx->packet[2], tx->data, tx->data_size);
    tx->packet[2 + tx->data_size] = calculate_checksum(tx->data, tx->data_size);
    tx->packet[3 + tx->data_size] = ETX;
    tx->packet_size = 4 + tx->data_size;
    
    // Envia o pacote
    memcpy(channel.tx_buffer, tx->packet, tx->packet_size);
    channel.tx_size = tx->packet_size;
    channel.tx_ready = true;
    tx->packet_sent = true;
    
    // Espera ACK ou timeout
    PT_WAIT_UNTIL(&tx->pt, tx->ack_received || tx->timeout);
    
    if (tx->ack_received) {
        // Sucesso - pode enviar próximo pacote
        tx->packet_sent = false;
        tx->ack_received = false;
    } else {
        // Timeout - reenvia
        PT_RESTART(&tx->pt);
    }
    
    PT_END(&tx->pt);
}

// ========================================
// ======= PROTOTHREAD DO RECEPTOR ========
// ========================================

int receiver_thread(receiver_t *rx) {
    PT_BEGIN(&rx->pt);
    
    while (1) {
        // Espera dados chegarem
        PT_WAIT_UNTIL(&rx->pt, channel.tx_ready);
        
        // Processa os dados recebidos
        for (int i = 0; i < channel.tx_size; i++) {
            uint8_t byte = channel.tx_buffer[i];
            
            switch (rx->state) {
                case STATE_WAIT_STX:
                    if (byte == STX) {
                        rx->buffer[0] = byte;
                        rx->received_size = 1;
                        rx->state = STATE_WAIT_QTD;
                    }
                    break;
                    
                case STATE_WAIT_QTD:
                    rx->buffer[rx->received_size++] = byte;
                    rx->expected_size = byte;
                    if (rx->expected_size == 0 || rx->expected_size > MAX_DATA_SIZE) {
                        rx->state = STATE_ERROR;
                    } else {
                        rx->state = STATE_WAIT_DATA;
                    }
                    break;
                    
                case STATE_WAIT_DATA:
                    rx->buffer[rx->received_size++] = byte;
                    if (rx->received_size >= 2 + rx->expected_size) {
                        rx->state = STATE_WAIT_CHK;
                    }
                    break;
                    
                case STATE_WAIT_CHK:
                    rx->buffer[rx->received_size++] = byte;
                    rx->checksum = byte;
                    rx->state = STATE_WAIT_ETX;
                    break;
                    
                case STATE_WAIT_ETX:
                    rx->buffer[rx->received_size++] = byte;
                    if (byte == ETX) {
                        // Verifica checksum
                        uint8_t calculated_checksum = calculate_checksum(&rx->buffer[2], rx->expected_size);
                        if (calculated_checksum == rx->checksum) {
                            rx->packet_ready = true;
                            rx->send_ack = true;
                        } else {
                            rx->error = true;
                        }
                    } else {
                        rx->error = true;
                    }
                    rx->state = STATE_WAIT_STX;
                    break;
                    
                case STATE_ERROR:
                    rx->state = STATE_WAIT_STX;
                    rx->received_size = 0;
                    break;
            }
        }
        
        channel.tx_ready = false;
        
        // Envia ACK se necessário
        if (rx->send_ack) {
            channel.rx_buffer[0] = ACK;
            channel.rx_size = 1;
            channel.rx_ready = true;
            rx->send_ack = false;
        }
        
        PT_YIELD(&rx->pt);
    }
    
    PT_END(&rx->pt);
}

// ========================================
// ========== FUNÇÕES DE TESTE ============
// ========================================

void setup_test_environment(void) {
    init_channel(&channel);
    init_transmitter(&tx);
    init_receiver(&rx);
}

void simulate_ack(void) {
    tx.ack_received = true;
}

void simulate_timeout(void) {
    tx.timeout = true;
}

static char * executa_testes(void);

int main()
{
    char *resultado = executa_testes();
    if (resultado != 0)
    {
        printf("%s\n", resultado);
    }
    else
    {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    return resultado != 0;
}

// =========================================
// =============== TESTES ==================
// =========================================

static char * test_checksum_calculation(void) {
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint8_t expected = 0x01 ^ 0x02 ^ 0x03;
    verifica("erro: checksum incorreto", calculate_checksum(data, 3) == expected);
    return 0;
}

static char * test_transmitter_packet_creation(void) {
    setup_test_environment();
    
    uint8_t test_data[] = {0x11, 0x22, 0x33};
    tx.data = test_data;
    tx.data_size = 3;
    
    // Executa uma iteração do transmissor
    transmitter_thread(&tx);
    
    verifica("erro: pacote não foi enviado", tx.packet_sent);
    verifica("erro: primeiro byte deve ser STX", tx.packet[0] == STX);
    verifica("erro: segundo byte deve ser o tamanho", tx.packet[1] == 3);
    verifica("erro: dados incorretos", memcmp(&tx.packet[2], test_data, 3) == 0);
    verifica("erro: penúltimo byte deve ser checksum", tx.packet[5] == calculate_checksum(test_data, 3));
    verifica("erro: último byte deve ser ETX", tx.packet[6] == ETX);
    
    return 0;
}

static char * test_receiver_valid_packet(void) {
    setup_test_environment();
    
    // Simula recepção de pacote válido
    uint8_t test_packet[] = {STX, 0x02, 0x41, 0x42, 0x41^0x42, ETX};
    memcpy(channel.tx_buffer, test_packet, 6);
    channel.tx_size = 6;
    channel.tx_ready = true;
    
    // Executa uma iteração do receptor
    receiver_thread(&rx);
    
    verifica("erro: pacote deveria estar pronto", rx.packet_ready);
    verifica("erro: não deveria haver erro", !rx.error);
    verifica("erro: deveria enviar ACK", channel.rx_ready);
    verifica("erro: ACK incorreto", channel.rx_buffer[0] == ACK);
    
    return 0;
}

static char * test_receiver_invalid_checksum(void) {
    setup_test_environment();
    
    // Simula recepção de pacote com checksum inválido
    uint8_t test_packet[] = {STX, 0x02, 0x41, 0x42, 0xFF, ETX}; // checksum errado
    memcpy(channel.tx_buffer, test_packet, 6);
    channel.tx_size = 6;
    channel.tx_ready = true;
    
    // Executa uma iteração do receptor
    receiver_thread(&rx);
    
    verifica("erro: pacote não deveria estar pronto", !rx.packet_ready);
    verifica("erro: deveria haver erro", rx.error);
    verifica("erro: não deveria enviar ACK", !channel.rx_ready);
    
    return 0;
}

static char * test_receiver_missing_stx(void) {
    setup_test_environment();
    
    // Simula recepção de pacote sem STX
    uint8_t test_packet[] = {0xFF, 0x02, 0x41, 0x42, 0x41^0x42, ETX};
    memcpy(channel.tx_buffer, test_packet, 6);
    channel.tx_size = 6;
    channel.tx_ready = true;
    
    // Executa uma iteração do receptor
    receiver_thread(&rx);
    
    verifica("erro: pacote não deveria estar pronto", !rx.packet_ready);
    verifica("erro: não deveria enviar ACK", !channel.rx_ready);
    
    return 0;
}

static char * test_receiver_missing_etx(void) {
    setup_test_environment();
    
    // Simula recepção de pacote sem ETX
    uint8_t test_packet[] = {STX, 0x02, 0x41, 0x42, 0x41^0x42, 0xFF};
    memcpy(channel.tx_buffer, test_packet, 6);
    channel.tx_size = 6;
    channel.tx_ready = true;
    
    // Executa uma iteração do receptor
    receiver_thread(&rx);
    
    verifica("erro: pacote não deveria estar pronto", !rx.packet_ready);
    verifica("erro: deveria haver erro", rx.error);
    verifica("erro: não deveria enviar ACK", !channel.rx_ready);
    
    return 0;
}

static char * test_transmitter_with_ack(void) {
    setup_test_environment();
    
    uint8_t test_data[] = {0x55};
    tx.data = test_data;
    tx.data_size = 1;
    
    // Primeira execução - envia pacote
    int result = transmitter_thread(&tx);
    verifica("erro: transmissor deveria estar aguardando", result == 1);
    verifica("erro: pacote deveria estar enviado", tx.packet_sent);
    
    // Simula recebimento de ACK
    simulate_ack();
    
    // Segunda execução - processa ACK
    result = transmitter_thread(&tx);
    verifica("erro: transmissor deveria ter terminado", result == 0);
    verifica("erro: flags deveriam estar resetadas", !tx.packet_sent && !tx.ack_received);
    
    return 0;
}

static char * test_transmitter_with_timeout(void) {
    setup_test_environment();
    
    uint8_t test_data[] = {0x99};
    tx.data = test_data;
    tx.data_size = 1;
    
    // Primeira execução - envia pacote
    int result = transmitter_thread(&tx);
    verifica("erro: transmissor deveria estar aguardando", result == 1);
    
    // Simula timeout
    simulate_timeout();
    
    // Segunda execução - processa timeout e reinicia
    result = transmitter_thread(&tx);
    verifica("erro: transmissor deveria estar aguardando novamente", result == 1);
    verifica("erro: pacote deveria estar enviado novamente", tx.packet_sent);
    
    return 0;
}

static char * test_communication_complete_cycle(void) {
    setup_test_environment();
    
    // Configura transmissor
    uint8_t test_data[] = {0xAA, 0xBB, 0xCC};
    tx.data = test_data;
    tx.data_size = 3;
    
    // Transmissor envia
    transmitter_thread(&tx);
    
    // Receptor processa
    receiver_thread(&rx);
    
    // Verifica se o receptor recebeu corretamente
    verifica("erro: receptor deveria ter pacote pronto", rx.packet_ready);
    verifica("erro: dados recebidos incorretos", 
             memcmp(&rx.buffer[2], test_data, 3) == 0);
    
    // Simula ACK chegando ao transmissor
    if (channel.rx_ready && channel.rx_buffer[0] == ACK) {
        tx.ack_received = true;
    }
    
    // Transmissor processa ACK
    int result = transmitter_thread(&tx);
    verifica("erro: transmissor deveria ter terminado", result == 0);
    
    return 0;
}

/***********************************************/

static char * executa_testes(void) {
    executa_teste(test_checksum_calculation);
    executa_teste(test_transmitter_packet_creation);
    executa_teste(test_receiver_valid_packet);
    executa_teste(test_receiver_invalid_checksum);
    executa_teste(test_receiver_missing_stx);
    executa_teste(test_receiver_missing_etx);
    executa_teste(test_transmitter_with_ack);
    executa_teste(test_transmitter_with_timeout);
    executa_teste(test_communication_complete_cycle);
    
    return 0;
}