#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef PT_H_
#define PT_H_

struct pt {
  unsigned short lc;
};

#define PT_INIT(pt)   ((pt)->lc = 0)
#define PT_THREAD(name_args) char name_args
#define PT_WAITING 0
#define PT_YIELDED 1
#define PT_EXITED  2
#define PT_ENDED   3

#define PT_BEGIN(pt) { char PT_YIELD_FLAG = 1; switch((pt)->lc) { case 0:
#define PT_END(pt)   } PT_YIELD_FLAG = 0; (pt)->lc = 0; return PT_ENDED; }
#define PT_WAIT_UNTIL(pt, condition) \
  do { (pt)->lc = __LINE__; case __LINE__: \
  if(!(condition)) { return PT_WAITING; } } while(0)
#define PT_WAIT_WHILE(pt, cond)  PT_WAIT_UNTIL((pt), !(cond))
#define PT_YIELD(pt) \
  do { PT_YIELD_FLAG = 0; (pt)->lc = __LINE__; case __LINE__: \
  if(PT_YIELD_FLAG == 0) { return PT_YIELDED; } } while(0)
#define PT_EXIT(pt) \
  do { PT_INIT(pt); return PT_EXITED; } while(0)
#define PT_SCHEDULE(f) ((f) < PT_EXITED)

#endif /* PT_H_ */

#define ASSERT_TRUE(message, condition) do { if (!(condition)) return message; } while (0)
#define RUN_TEST_CASE(test_function) do { char *msg = test_function(); g_testsRun++; \
                                          if (msg) return msg; } while (0)
int g_testsRun = 0;

// Constantes do Protocolo e da Simulação
#define FRAME_START 0x02
#define FRAME_END   0x03
#define ACK_BYTE    0x06
#define PAYLOAD_MAX_SIZE 32
#define TIMEOUT_TICKS 100

// Ambiente de Comunicação Simulado
uint8_t g_comm_buffer[PAYLOAD_MAX_SIZE + 5];
int g_comm_buffer_len = 0;
bool g_ack_received = false;
unsigned int g_system_ticks = 0;

// Variáveis de estado para verificação nos testes
int g_retransmissions = 0;
uint8_t g_received_payload[PAYLOAD_MAX_SIZE];
int g_received_payload_len = 0;

void setup_test() {
    g_comm_buffer_len = 0;
    g_ack_received = false;
    g_system_ticks = 0;
    g_retransmissions = 0;
    g_received_payload_len = 0;
    memset(g_comm_buffer, 0, sizeof(g_comm_buffer));
    memset(g_received_payload, 0, sizeof(g_received_payload));
}


// Contextos das Protothreads
struct pt pt_transmitter, pt_receptor;

// Protótipos
PT_THREAD(protothread_receptor(struct pt *pt));
PT_THREAD(protothread_transmitter(struct pt *pt, const uint8_t* data, uint8_t len));

uint8_t calculate_checksum(const uint8_t* data, uint8_t len) {
    uint8_t chk = 0;
    for (uint8_t i = 0; i < len; ++i) {
        chk ^= data[i];
    }
    return chk;
}

PT_THREAD(protothread_receptor(struct pt *pt)) {
    static uint8_t expected_len;
    static uint8_t calculated_chk;
    static uint8_t received_chk;
    static uint8_t i;
    static uint8_t byte;

    PT_BEGIN(pt);

    while(1) {
        // Fase 1: Aguardar STX
        PT_WAIT_UNTIL(pt, g_comm_buffer_len > 0 && g_comm_buffer[0] == FRAME_START);
        g_comm_buffer_len = 0; // Consome o byte

        // Fase 2: Aguardar QTD
        PT_WAIT_UNTIL(pt, g_comm_buffer_len > 0);
        expected_len = g_comm_buffer[0];
        g_received_payload_len = 0;
        calculated_chk = 0;
        g_comm_buffer_len = 0;

        // Fase 3: Aguardar Dados (Payload)
        for (i = 0; i < expected_len; ++i) {
            PT_WAIT_UNTIL(pt, g_comm_buffer_len > 0);
            byte = g_comm_buffer[0];
            g_received_payload[i] = byte;
            calculated_chk ^= byte;
            g_received_payload_len++;
            g_comm_buffer_len = 0;
        }

        // Fase 4: Aguardar CHK
        PT_WAIT_UNTIL(pt, g_comm_buffer_len > 0);
        received_chk = g_comm_buffer[0];
        g_comm_buffer_len = 0;

        // Fase 5: Aguardar ETX e Validar
        PT_WAIT_UNTIL(pt, g_comm_buffer_len > 0);
        byte = g_comm_buffer[0];
        g_comm_buffer_len = 0;

        if (byte == FRAME_END && received_chk == calculated_chk) {
            // Sucesso! Envia ACK.
            g_comm_buffer[0] = ACK_BYTE;
            g_comm_buffer_len = 1;
        }
        // Se falhar, não faz nada e volta ao início para aguardar um novo STX.
    }

    PT_END(pt);
}

PT_THREAD(protothread_transmitter(struct pt *pt, const uint8_t* data, uint8_t len)) {
    static unsigned int ack_timeout_tick;
    static uint8_t checksum;
    static uint8_t i;
    
    PT_BEGIN(pt);

    checksum = calculate_checksum(data, len);

    while(1) { // Loop para permitir retransmissões
        // Fase 1: Enviar STX
        g_comm_buffer[0] = FRAME_START;
        g_comm_buffer_len = 1;
        PT_YIELD(pt); // Cede controle para o receptor processar

        // Fase 2: Enviar QTD 
        g_comm_buffer[0] = len;
        g_comm_buffer_len = 1;
        PT_YIELD(pt);

        // Fase 3: Enviar Dados (Payload)
        for (i = 0; i < len; ++i) {
            g_comm_buffer[0] = data[i];
            g_comm_buffer_len = 1;
            PT_YIELD(pt);
        }

        // Fase 4: Enviar CHK
        g_comm_buffer[0] = checksum;
        g_comm_buffer_len = 1;
        PT_YIELD(pt);

        // Fase 5: Enviar ETX
        g_comm_buffer[0] = FRAME_END;
        g_comm_buffer_len = 1;
        PT_YIELD(pt);

        //Fase 6: Aguardar ACK com Timeout
        ack_timeout_tick = g_system_ticks + TIMEOUT_TICKS;
        PT_WAIT_UNTIL(pt, g_ack_received || g_system_ticks >= ack_timeout_tick);

        if (g_ack_received) {
            // ACK recebido, transmissão concluída com sucesso.
            break; 
        } else {
            g_retransmissions++;
        }
    }

    PT_END(pt);
}


static char* test_receptor_decodes_valid_packet_and_sends_ack() {
    setup_test();
    uint8_t payload[] = {'O', 'L', 'A'};
    uint8_t chk = calculate_checksum(payload, 3);
    uint8_t packet[] = {FRAME_START, 3, 'O', 'L', 'A', chk, FRAME_END};

    PT_INIT(&pt_receptor);

    for (int i = 0; i < sizeof(packet); ++i) {
        g_comm_buffer[0] = packet[i];
        g_comm_buffer_len = 1;
        protothread_receptor(&pt_receptor);
    }
    
    ASSERT_TRUE("Receptor falhou: Carga útil não foi decodificada.", g_received_payload_len == 3 && memcmp(g_received_payload, payload, 3) == 0);
    ASSERT_TRUE("Receptor falhou: ACK não foi enviado após pacote válido.", g_comm_buffer_len == 1 && g_comm_buffer[0] == ACK_BYTE);
    return 0;
}

static char* test_transmitter_sends_packet_and_receives_ack() {
    setup_test();
    uint8_t payload_to_send[] = {0xDE, 0xAD};
    
    PT_INIT(&pt_transmitter);
    PT_INIT(&pt_receptor);

    while (PT_SCHEDULE(protothread_transmitter(&pt_transmitter, payload_to_send, 2))) {
        g_system_ticks++;
        protothread_receptor(&pt_receptor); // Receptor processa o que o transmissor envia
        
        // Simula a camada inferior recebendo o ACK e setando a flag
        if(g_comm_buffer_len == 1 && g_comm_buffer[0] == ACK_BYTE) {
            g_ack_received = true;
            g_comm_buffer_len = 0;
        }
    }

    ASSERT_TRUE("Transmissor falhou: Nenhuma retransmissão deveria ocorrer.", g_retransmissions == 0);
    ASSERT_TRUE("Lógica do teste falhou: ACK não foi recebido.", g_ack_received == true);
    return 0;
}

static char* test_transmitter_retransmits_on_timeout() {
    setup_test();
    uint8_t payload_to_send[] = {0xBE, 0xEF};
    
    PT_INIT(&pt_transmitter);
    PT_INIT(&pt_receptor);

    g_ack_received = false; // Garante que o ACK nunca será recebido

    // Simula um scheduler por um tempo maior que o timeout
    while (g_system_ticks < TIMEOUT_TICKS + 50) {
        protothread_transmitter(&pt_transmitter, payload_to_send, 2);
        g_system_ticks++;
    }
    
    ASSERT_TRUE("Transmissor falhou: Deveria ter ocorrido ao menos uma retransmissão.", g_retransmissions > 0);
    return 0;
}


static char* runAllTests() {
    RUN_TEST_CASE(test_receptor_decodes_valid_packet_and_sends_ack);
    RUN_TEST_CASE(test_transmitter_sends_packet_and_receives_ack);
    RUN_TEST_CASE(test_transmitter_retransmits_on_timeout);
    return 0;
}

int main() {
    char *result = runAllTests();
    
    printf("\n--- Resultado Final  ---\n");
    if (result != 0) {
        printf("UM TESTE FALHOU: %s\n", result);
    } else {
        printf("STATUS: Todos os testes foram bem-sucedidos.\n");
    }
    printf("Total de casos de teste executados: %d\n", g_testsRun);
    
    return result != 0;
}
