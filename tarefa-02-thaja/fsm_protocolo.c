#include "fsm_protocolo.h"
#include <stddef.h>

void fsm_init(FsmProtocolo *fsm) {
    if (fsm == NULL) return;
    fsm->state = ST_STX;
    fsm->indBuffer = 0;
    fsm->qtdBuffer = 0;
    fsm->chkBuffer = 0;
    fsm->pacote_processado = false;
}

void fsm_process_char(FsmProtocolo *fsm, uint8_t c) {
    if (fsm == NULL) return;

    switch (fsm->state) {
        case ST_STX:
            // Aguarda o byte de início (STX = 0x02)
            if (c == STX) {
                fsm->indBuffer = 0;
                fsm->qtdBuffer = 0;
                fsm->chkBuffer = 0;
                fsm->pacote_processado = false;
                fsm->state = ST_QTD;
            }
            break;

        case ST_QTD:
            // Define o tamanho esperado do campo DADOS
            fsm->qtdBuffer = c;
            if (fsm->qtdBuffer == 0) {
                // Se o pacote tem zero bytes de dados, vai direto verificar o Checksum
                fsm->state = ST_CHK;
            } else {
                fsm->state = ST_DATA;
            }
            break;

        case ST_DATA:
            // Salva o caractere no buffer interno de dados úteis (protegido contra estouro de array)
            if (fsm->indBuffer < MAX_BUFFER) {
                fsm->buffer[fsm->indBuffer++] = c;
            }
            
            // O checksum é calculado fazendo XOR dos bytes do campo DADOS
            fsm->chkBuffer ^= c;
            
            // Decrementa a quantidade de bytes pendentes
            fsm->qtdBuffer--;
            if (fsm->qtdBuffer == 0) {
                fsm->state = ST_CHK;
            }
            break;

        case ST_CHK:
            // Compara o byte recebido com o XOR acumulado calculado no estado ST_DATA
            if (c == fsm->chkBuffer) {
                fsm->state = ST_ETX;
            } else {
                // Se houver discrepância no Checksum, descarta o pacote e reinicia a FSM
                fsm->state = ST_STX;
            }
            break;

        case ST_ETX:
            // Valida o byte de fechamento final (ETX = 0x03)
            if (c == ETX) {
                fsm->pacote_processado = true;
            }
            // Retorna ao estado inicial para aguardar o próximo frame
            fsm->state = ST_STX;
            break;

        default:
            fsm->state = ST_STX;
            break;
    }
}
