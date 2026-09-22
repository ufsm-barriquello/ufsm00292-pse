#include <stdio.h>
#include <stdlib.h>

/* =========================================================
   1. MACROS DA PROTOTHREAD (Baseado na lib do Adam Dunkels)
   ========================================================= */
struct pt { unsigned short lc; };
#define PT_INIT(pt)   (pt)->lc = 0
#define PT_BEGIN(pt)  switch((pt)->lc) { case 0:
#define PT_WAIT_UNTIL(pt, condition) \
  do { \
    (pt)->lc = __LINE__; case __LINE__: \
    if(!(condition)) return 0; \
  } while(0)
#define PT_END(pt)    } (pt)->lc = 0; return 2

/* =========================================================
   2. MACROS DE TESTE (TDD)
   ========================================================= */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

/* =========================================================
   3. DEFINIÇÕES DO PROTOCOLO E VARIÁVEIS DE SIMULAÇÃO
   ========================================================= */
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define MAX_TIMEOUT 10

// Variáveis para simular o meio de comunicação e o tempo
unsigned char barramento_tx = 0;
unsigned char barramento_rx = 0;
int timer_sistema = 0;
int envios_realizados = 0;

/* =========================================================
   4. A PROTOTHREAD TRANSMISSORA (TX)
   ========================================================= */
struct pt pt_tx;
unsigned char tx_buffer[3] = {0xAA, 0xBB, 0xCC}; // 3 Bytes de dados
unsigned char tx_qtd = 3;

int thread_transmissora(struct pt *pt) {
    static int i;
    static unsigned char chk;
    
    PT_BEGIN(pt);

    while(1) { // Laço principal (tenta enviar até conseguir)
        envios_realizados++;
        
        // 1. Envia Cabeçalho e QTD
        barramento_tx = STX;
        barramento_tx = tx_qtd;
        chk = 0;
        
        // 2. Envia Dados 
        for(i = 0; i < tx_qtd; i++) {
            barramento_tx = tx_buffer[i];
            chk ^= tx_buffer[i]; // Calcula checksum simples (XOR)
        }
        
        // 3. Envia Checksum e Fim
        barramento_tx = chk;
        barramento_tx = ETX;

        // 4. Aguarda o ACK ou o Timeout
        timer_sistema = 0; 
        PT_WAIT_UNTIL(pt, (barramento_rx == ACK) || (timer_sistema >= MAX_TIMEOUT));

        // 5. Verifica se foi sucesso
        if (barramento_rx == ACK) {
            barramento_rx = 0; // Limpa o buffer de recepção
            break; // Sai do laço de reenvio!
        }
        // Se foi timeout, o while(1) repete o envio
    }
    
    PT_END(pt);
}

/* =========================================================
   5. A PROTOTHREAD RECEPTORA (RX)
   ========================================================= */
struct pt pt_rx;
// Aqui simulamos uma recepção perfeita instantânea para facilitar o TDD
int thread_receptora(struct pt *pt) {
    PT_BEGIN(pt);
    
    while(1) {
        // Fica aguardando chegar um STX
        PT_WAIT_UNTIL(pt, barramento_tx == ETX); // Simplificação: espera o pacote todo
        
        // Se o pacote for processado com sucesso (simulado), envia ACK
        barramento_rx = ACK;
        barramento_tx = 0; // Limpa
    }
    
    PT_END(pt);
}

/* =========================================================
   6. TESTES TDD
   ========================================================= */
static char * teste_transmissao_sucesso(void) {
    PT_INIT(&pt_tx);
    envios_realizados = 0;
    timer_sistema = 0;
    barramento_rx = ACK; // Simulando que o receptor já respondeu rápido
    
    thread_transmissora(&pt_tx); // Roda a thread
    
    verifica("erro: TX deveria ter enviado 1 vez e recebido ACK", envios_realizados == 1);
    return 0;
}

static char * teste_timeout_e_reenvio(void) {
    PT_INIT(&pt_tx);
    envios_realizados = 0;
    barramento_rx = 0; // Sem resposta do RX (vai dar erro)
    
    // 1º Envio
    thread_transmissora(&pt_tx);
    verifica("erro: TX deveria ter tentado enviar", envios_realizados == 1);
    
    // Simula o tempo passando (Timeout)
    timer_sistema = MAX_TIMEOUT; 
    
    // 2º Envio (Reenvio automático)
    thread_transmissora(&pt_tx);
    verifica("erro: TX nao reenviou apos timeout", envios_realizados == 2);
    
    return 0;
}

/* =========================================================
   BLOCO DE EXECUÇÃO
   ========================================================= */
static char * executa_testes(void) {
    executa_teste(teste_transmissao_sucesso);
    executa_teste(teste_timeout_e_reenvio);
    return 0;
}

int main() {
    char *resultado = executa_testes();
    
    if (resultado != 0) printf("%s\n", resultado);
    else printf("TODOS OS TESTES PASSARAM\n");
    
    printf("Testes executados: %d\n", testes_executados);
    return 0;
}