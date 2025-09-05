#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Definições mínimas da biblioteca Protothreads (Adam Dunkels) */
struct pt { unsigned short lc; };  /* Variável de continuação local */
#define PT_THREAD(name_args) char name_args
#define PT_BEGIN(pt) switch((pt)->lc) { case 0:
#define PT_END(pt) } (pt)->lc = 0; return 2;
#define PT_INIT(pt) ((pt)->lc = 0)
#define PT_WAIT_UNTIL(pt, condition) \
    do { (pt)->lc = __LINE__; case __LINE__: if(!(condition)) return 0; } while(0)
#define PT_YIELD(pt) \
    do { (pt)->lc = __LINE__; return 0; case __LINE__:; } while(0)

/* Constantes do protocolo */
#define STX 0x02   /* Start of Text */
#define ETX 0x03   /* End of Text */
#define ACK 0x06   /* Acknowledgment (ACK) */
#define TIMEOUT 5  /* Timeout em "ticks" simulados (iterações) */

/* Variáveis globais de comunicação (canal e ACK) */
static unsigned char channel_byte;
static int channel_flag = 0;   /* 0 = canal vazio, 1 = byte disponível */
static unsigned char ack_byte;
static int ack_flag = 0;       /* 0 = sem ACK, 1 = ACK disponível */

/* Dados de teste a serem enviados (embutidos no código) */
static const unsigned char test_data[] = {'H','E','L','L','O'};  /* Exemplo de dados */
#define TEST_LEN 5  /* Quantidade de bytes de dados */

/* Variáveis globais para verificação (TDD) */
static int attempts_count = 0;
static int transmission_success = 0;

/* Armazenamento do último pacote recebido (para verificação) */
static unsigned char last_message[256];
static int last_length = 0;
static int success_count = 0;

/* Temporizador para simular timeout de ACK */
struct timer { int start; int interval; };
static int ticks = 0;  /* "Relógio" global de tempo (ticks de iteração) */

static void timer_set(struct timer *t, int interval) {
    t->start = ticks;
    t->interval = interval;
}

static int timer_expired(struct timer *t) {
    return (ticks - t->start) >= t->interval;
}

/* Verifica se ACK (0x06) foi recebido (consome o ACK) */
static int ack_received(void) {
    if (ack_flag && ack_byte == ACK) {
        ack_flag = 0;  /* consumir ACK para não ler duas vezes */
        return 1;
    }
    return 0;
}

/* Protothread Transmissora (envio) */
PT_THREAD(transmitter_thread(struct pt *pt)) {
    /* Variáveis static para preservar estado entre yields */
    static struct timer timer;
    static unsigned char checksum;
    static unsigned char pkt_chk;
    static int j;

    PT_BEGIN(pt);
    attempts_count = 0;
    do {
        /* Prepara envio do pacote (introduz erro proposital na 1ª tentativa) */
        attempts_count++;
        // Calcula checksum (soma) dos dados
        checksum = 0;
        for (j = 0; j < TEST_LEN; ++j) {
            checksum += test_data[j];
        }
        // Define byte de checksum do pacote (erro na 1ª tentativa)
        if (attempts_count == 1) {
            pkt_chk = (unsigned char)((checksum + 1) & 0xFF);  // checksum incorreto na primeira tentativa
        } else {
            pkt_chk = (unsigned char)(checksum & 0xFF);        // checksum correto nas tentativas subsequentes
        }
        /* Envia o pacote byte a byte */
        // Envia STX (início)
        PT_WAIT_UNTIL(pt, channel_flag == 0);
        channel_byte = STX;
        channel_flag = 1;
        PT_WAIT_UNTIL(pt, channel_flag == 0);  // espera ser consumido pela receptora
        // Envia QTD (quantidade de bytes de dados)
        PT_WAIT_UNTIL(pt, channel_flag == 0);
        channel_byte = (unsigned char)TEST_LEN;
        channel_flag = 1;
        PT_WAIT_UNTIL(pt, channel_flag == 0);
        // Envia bytes de DADOS
        for (j = 0; j < TEST_LEN; ++j) {
            PT_WAIT_UNTIL(pt, channel_flag == 0);
            channel_byte = test_data[j];
            channel_flag = 1;
            PT_WAIT_UNTIL(pt, channel_flag == 0);
        }
        // Envia CHK (checksum calculado)
        PT_WAIT_UNTIL(pt, channel_flag == 0);
        channel_byte = pkt_chk;
        channel_flag = 1;
        PT_WAIT_UNTIL(pt, channel_flag == 0);
        // Envia ETX (fim)
        PT_WAIT_UNTIL(pt, channel_flag == 0);
        channel_byte = ETX;
        channel_flag = 1;
        PT_WAIT_UNTIL(pt, channel_flag == 0);

        // Pacote enviado, agora espera ACK ou timeout
        timer_set(&timer, TIMEOUT);
        PT_WAIT_UNTIL(pt, ack_received() || timer_expired(&timer));
        /* Se o tempo expirar sem ACK, loop repete para reenviar o pacote */
    } while (timer_expired(&timer));
    /* Saindo do loop: ACK recebido antes do timeout */
    transmission_success = 1;
    PT_END(pt);
}

/* Protothread Receptora (recebimento) */
PT_THREAD(receiver_thread(struct pt *pt)) {
    /* Variáveis static para preservar estado entre yields */
    static unsigned char byte;
    static int len;
    static unsigned char chk;
    static unsigned char checksum_calc;
    static int i;

    PT_BEGIN(pt);
    /* Loop aguardando pacotes continuamente */
    while (1) {
        // Aguarda byte de início STX
        PT_WAIT_UNTIL(pt, channel_flag == 1);
        byte = channel_byte;
        channel_flag = 0;
        if (byte != STX) {
            // Se não for STX, ignora e continua esperando
            continue;
        }
        // STX recebido, lê agora o comprimento QTD
        PT_WAIT_UNTIL(pt, channel_flag == 1);
        len = channel_byte;
        channel_flag = 0;
        // Lê 'len' bytes de dados
        checksum_calc = 0;
        for (i = 0; i < len; ++i) {
            PT_WAIT_UNTIL(pt, channel_flag == 1);
            last_message[i] = channel_byte;
            channel_flag = 0;
            checksum_calc += last_message[i];
        }
        last_length = len;
        // Lê byte de checksum (CHK)
        PT_WAIT_UNTIL(pt, channel_flag == 1);
        chk = channel_byte;
        channel_flag = 0;
        // Lê byte de fim ETX
        PT_WAIT_UNTIL(pt, channel_flag == 1);
        byte = channel_byte;
        channel_flag = 0;
        if (byte != ETX) {
            // Erro de protocolo (ETX incorreto)
            continue;  // não envia ACK, aguarda retransmissão
        }
        // Verifica checksum
        if (checksum_calc != chk) {
            // Checksum incorreto
            continue;  // não envia ACK, aguarda retransmissão
        }
        // Pacote correto: envia ACK (0x06)
        ack_byte = ACK;
        ack_flag = 1;
        success_count++;
        // Interrompe loop após um pacote válido (teste concluído)
        break;
    }
    PT_END(pt);
}

int main() {
    struct pt pt_tx, pt_rx;
    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);

    /* Inicializa as protothreads e executa loop de escalonamento cooperativo */
    int done_tx = 0, done_rx = 0;
    while (1) {
        char tx_status = 0, rx_status = 0;
        if (!done_tx) tx_status = transmitter_thread(&pt_tx);
        if (!done_rx) rx_status = receiver_thread(&pt_rx);
        ticks++;  // avança tempo simulado a cada iteração
        if (tx_status == 2) done_tx = 1;
        if (rx_status == 2) done_rx = 1;
        // Sai do loop se transmissora finalizou
        if (done_tx) break;
        // (Segurança) Interrompe se ultrapassar número de iterações esperado
        if (ticks > 1000) {
            printf("Erro: Protocolo não completou no tempo esperado.\n");
            break;
        }
    }

    /* Validações básicas de TDD (asserts para verificar funcionamento) */
    assert(transmission_success == 1);          // Transmissão completada com sucesso
    assert(success_count == 1);                 // Um pacote recebido com sucesso
    assert(last_length == TEST_LEN);            // Comprimento recebido confere com o enviado
    assert(memcmp(last_message, test_data, TEST_LEN) == 0);  // Dados recebidos conferem com os enviados
    assert(attempts_count == 2);                // Verifica que houve 2 tentativas (1ª falhou, 2ª teve sucesso)

    printf("Todos os testes passaram. Protocolo funcionando com %d tentativa(s).\n", attempts_count);
    return 0;
}
