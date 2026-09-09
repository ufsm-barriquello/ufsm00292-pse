#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "pt.h"

/* Protocol framing constants */
#define PROTOCOL_STX 0x02
#define PROTOCOL_ETX 0x03

/* Estado da máquina de leitura byte-a-byte. */
typedef enum {
    STATE_START = 0,  /* aguardando STX (0x02) */
    STATE_QTD,        /* aguardando QTD_DADOS (1 byte) */
    STATE_DATA,       /* recebendo DADOS (QTD_DADOS bytes) */
    STATE_CHKSUM,     /* aguardando CHK (XOR dos dados) */
    STATE_END         /* aguardando ETX (0x03) -> volta para START */
} Estado;

typedef struct {
    uint8_t n;
    uint8_t qtd;
    uint8_t chksum;
} Rundata;

/* Contexto event-driven para protothreads.
 *  pt/qbuf/qhead/qtail/qcount - fila de eventos para processamento assíncrono
 */
struct protocolo_pt {
    struct pt pt;
    Estado estado;
    Rundata data;
    uint8_t byte;
    volatile uint8_t byte_ready;
    uint8_t frame_complete;
    uint8_t frame_error;
    uint8_t qbuf[16];
    uint8_t qhead;
    uint8_t qtail;
    uint8_t qcount;
};

void rundata_reset(Rundata *r);

/* === API protothread (substitui o antigo process_byte com switch) === */

/* Inicializa contexto event-driven */
void protocolo_pt_init(struct protocolo_pt *ctx);

/* Enfileira um byte como evento (não bloqueante) */
void protocolo_pt_input(struct protocolo_pt *ctx, uint8_t byte);

/* Processa UM byte de forma síncrona usando protothread.
 * Internamente agenda protocolo_thread_step (protothread).
 * Substitui o antigo: Estado process_byte(byte, estado, &data)
 * Uso novo: ctx.estado = STATE_START; protocolo_pt_step(&ctx, byte);
 */
Estado protocolo_pt_step(struct protocolo_pt *ctx, uint8_t byte);

/* Alimenta um buffer inteiro via protothreads (event-driven) */
size_t protocolo_pt_feed(struct protocolo_pt *ctx, const uint8_t *buf, size_t len);

static inline int protocolo_pt_has_frame(struct protocolo_pt *ctx) {
    return ctx ? ctx->frame_complete : 0;
}
static inline void protocolo_pt_consume_frame(struct protocolo_pt *ctx) {
    if (ctx) ctx->frame_complete = 0;
}
static inline int protocolo_pt_has_error(struct protocolo_pt *ctx) {
    return ctx ? ctx->frame_error : 0;
}

/* Protothread principal event-driven (fila).
 * Consome bytes enfileirados via protocolo_pt_input().
 * Bloqueia cooperativamente com PT_WAIT_UNTIL(qcount>0).
 */
PT_THREAD(protocolo_thread(struct pt *pt, struct protocolo_pt *ctx));

/* Protothread de passo único (um byte).
 * Implementa a transição de estado SEM switch, usando protothread.
 * É a base para protocolo_pt_step() e para o wrapper process_byte().
 */
PT_THREAD(protocolo_thread_step(struct pt *pt, struct protocolo_pt *ctx, uint8_t byte));

/* Wrapper de compatibilidade: antigo process_byte agora DELEGA para protothread.
 * Mantido para testes legados, mas internamente usa protothreads.
 * Novo código deve usar protocolo_pt_*.
 */
Estado process_byte(uint8_t entrada, Estado atual, Rundata *armazenamento);

#endif
