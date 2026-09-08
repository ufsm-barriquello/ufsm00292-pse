#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "pt.h" 

#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define TIMEOUT_MAX 5

// simulação hardware
uint8_t buffer_canal[256];
int canal_tamanho = 0;
bool tem_dados = false;
bool ack_recebido = false;
int ticks_tempo = 0;
int retransmissoes = 0;

struct pt pt_tx, pt_rx;

// protothread decodificador
PT_THREAD(thread_receptor(struct pt *pt)) {
    static int i;
    static uint8_t chk_calc;
    
    PT_BEGIN(pt);
    
    while(1) {
        PT_WAIT_UNTIL(pt, tem_dados); // Aguarda algo chegar
        
        // Valida se o formato básico confere (STX e ETX nos lugares certos)
        if (buffer_canal[0] == STX && buffer_canal[canal_tamanho - 1] == ETX) {
            chk_calc = buffer_canal[1]; // Inicia com o tamanho
            for (i = 2; i < canal_tamanho - 2; i++) {
                chk_calc ^= buffer_canal[i]; // Calcula o CHK dos dados
            }
            
            // Verifica o Checksum
            if (chk_calc == buffer_canal[canal_tamanho - 2]) {
                ack_recebido = true; // Pacote válido, envia ACK
            }
        }
        tem_dados = false; // Limpa o canal
    }
    
    PT_END(pt);
}

// protothread codificadr 
PT_THREAD(thread_transmissor(struct pt *pt, uint8_t *dados, uint8_t qtd)) {
    static int tempo_inicio;
    static int i;
    static uint8_t chk;
    
    PT_BEGIN(pt);
    
    while(1) {
        // Monta o pacote
        buffer_canal[0] = STX;
        buffer_canal[1] = qtd;
        chk = qtd;
        
        for (i = 0; i < qtd; i++) {
            buffer_canal[2 + i] = dados[i];
            chk ^= dados[i];
        }
        
        buffer_canal[2 + qtd] = chk;
        buffer_canal[3 + qtd] = ETX;
        canal_tamanho = 4 + qtd;
        
        tem_dados = true;
        ack_recebido = false;
        tempo_inicio = ticks_tempo;
        
        // Aguarda o ACK ou o tempo limite
        PT_WAIT_UNTIL(pt, ack_recebido || (ticks_tempo - tempo_inicio >= TIMEOUT_MAX));
        
        if (ack_recebido) {
            break; // Sucesso, sai do loop
        } else {
            retransmissoes++; // Timeout, o loop vai repetir e reenviar
        }
    }
    
    PT_END(pt);
}

// tdd
void test_comunicacao_com_sucesso() {
    uint8_t payload[] = {0xAA, 0xBB};
    
    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);
    ack_recebido = false;
    retransmissoes = 0;
    
    // Ciclo de escalonamento
    thread_transmissor(&pt_tx, payload, 2); 
    thread_receptor(&pt_rx);
    thread_transmissor(&pt_tx, payload, 2); // Retoma para verificar o ACK
    
    assert(ack_recebido == true);
    assert(retransmissoes == 0);
    printf("[OK] TDD - Dados transmitidos, decodificados e ACK recebido com sucesso.\n");
}

void test_comunicacao_com_retransmissao() {
    uint8_t payload[] = {0x11};
    
    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);
    ack_recebido = false;
    retransmissoes = 0;
    
    // Transmissor envia o pacote
    thread_transmissor(&pt_tx, payload, 1);
    
    // Corrompe o pacote no meio do caminho para forçar falha no checksum
    buffer_canal[2] = 0xFF; 
    
    // Receptor lê o pacote corrompido (não vai gerar ACK)
    thread_receptor(&pt_rx);
    
    // Simula o tempo passando (Timeout)
    ticks_tempo += TIMEOUT_MAX + 1;
    
    // Transmissor percebe o timeout e reenvia
    thread_transmissor(&pt_tx, payload, 1);
    assert(retransmissoes == 1);
    
    // Desta vez o receptor lê o pacote intacto e gera o ACK
    thread_receptor(&pt_rx);
    thread_transmissor(&pt_tx, payload, 1);
    
    assert(ack_recebido == true);
    printf("[OK] TDD - Erro injetado, Timeout processado e pacote retransmitido com sucesso.\n");
}

int main() {
    printf("\n INICIANDO TESTES TDD (PROTOTHREADS) \n\n");
    
    test_comunicacao_com_sucesso();
    test_comunicacao_com_retransmissao();
    
    printf("         TODOS OS TESTES FORAM REALIZADOS COM SUCESSO!      \n");
    return 0;
}